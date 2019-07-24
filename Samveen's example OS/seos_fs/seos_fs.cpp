/* This file containts the SEOS floppy file system creation and description.
 *
 * Copyright (c) 2003 onwards, Samveen S. Gulati.
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "functions.h"

/* Total file size is 1474560 -> 2880 sectors of 512 bytes each (LBA addressing)

   The bootsector will come as a file of size 512 bytes. The last two
   bytes are signature bytes of value 0x55,0xAA. The bootsector will be
   complete with the signature bytes included so no work is to be done
   for the boot sector except a size check.
   
   The kernel image goes into the 89 sectors after the boot sector. This
   means that the kernel image is constrained in size to a maximum of
   89 * 512 = 45568 bytes.
   
   The 90st LBA sector to the 179th LBA sector contain the file Information
   Table. This space also behaves as the root directory. each sector here
   contains information about the files that are on the file system.
   Beginning from sector 90 onwards each sector 90 + (X -1) gives the
   information of the file located from LBA sector 180 + (X-1)*30 to LBA
   sector to 180 + (X * 30) - 1

   The information contained in the file information sector is as follows:
      1) file number in the file information table
      2) is this a valid file (is it being used or not)
      3) name of the file.
      4) is it a system file.
      5) is it a executable file.
      6) is it a readonly file.
      7) size of the file in bytes.
      
      File number will contain the number of the file in sequence that is
      LBA sector 90 (first file) will contain 1, LBA sector 179(90th file)
      will contain 90 etc
      
      Attributes 4, 5, and 6 will be all contained in one atributes byte
      saving space and logical computations
      
      size of the file will be a 2 byte quantity
      
      name of the file will be a 64 byte string including the terminating
      zero byte.
      
      This information sector will be implemented here as a structure of
      size 512 bytes*/
      
const char SYSTEM     = 0x01;
const char EXECUTABLE = 0x02;
const char READONLY   = 0x04;

typedef struct
{
    char file_number;  // 1 byte
    char valid;        // 1 byte
    short size;        // 2 bytes
    char attr;         // 1 byte
    char filler[443];  // 443 bytes
    char name[64];     // 64 bytes
} file_info;           // total: 512 bytes
   
typedef struct FILE_ENTRY
{
    char path[255];
    file_info info;
} entry ;

entry file_table[91];

string name;


int getsize(char path[])
{
    fstream file(path,ios::binary|ios::in);
    if (!file)
        return(-1);
    else
    {
        int i=0,j=0;
        char ch[513];
        file.read(ch,512);
        while((j=file.gcount())>0)
        {
            i+=j;
            file.read(ch,512);
        }
        return (i);
    }
}

void parse_name(const char p[], char name[])
{
    char path[255];
    bool ispath=true;
    int i;
    strcpy(path,p);

    while(ispath==true)
    {
        ispath=false;
        for(i=0; path[i]!=0 && ispath==false ; i++)
            if (path[i]=='\\')
                ispath=true;
        
        if (ispath==true)
        {
            int j=i;
            for(i;path[i]!=0;i++)
                path[i-j]=path[i];
            path[i-j]=path[i];
        }
    }

    for(i=0; path[i]!=0;i++)
            name[i]=path[i];
    name[i]=0;
    return;
}

void help_message()
{
    cout<<"SEOS Bootable Floppy and File System Creator. This program copies the boot\n";
    cout<<"sector and kernel onto a floppy img and and then interactively adds files\n";
    cout<<"to the file system on the floppy\n\n";
    cout<<name<<" /h | /? | -h | bsect kernel dest\n";
    cout<<"   /h      Displays this help\n";
    cout<<"   /?      Displays this help\n";
    cout<<"   -h      Displays this help\n";
    cout<<"   bsect   name of bootsector file\n";
    cout<<"   kernel  name of kernel file\n";
    cout<<"   dest    name of floppy img\n";
    cout<<"\nMade by Samveen Gulati\n";
}

void Error(char message[],int retval,bool help)
{
    cout<<"Error: "<<message<<endl;
    if (help==true)
        help_message();
    exit(retval);
}
   
int main(int argc,char * argv[])
{
    name=argv[0];
    if(argc==1)
    {
            help_message();
            return(2);
    }
    else
        if(argc==4)
        {
            char ch[514];
            int i=0,j=0;
            
            cout<<"using bootsector :"<<argv[1]<<endl;
            cout<<"using kernel :"<<argv[2]<<endl;
            cout<<"using destination :"<<argv[3]<<endl;
            cout<<"press enter to continue or CTRL-C to quit";
            getchar();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
            // opening bootsector
            int size=getsize(argv[1]);
            if (size<0)
                Error("cannot open bootsector",64,false);
            if (size!=512)
                Error("wrong size of bootsector",64,false);
            fstream bsect(argv[1],ios::binary|ios::in);
            
            // opening kernel
            size=getsize(argv[2]);
            if (size<0)
                Error("cannot open kernel",65,false);
            if (size>45568)
                Error("kernel is bigger than 89 sectors",65,false);
            fstream kern(argv[2],ios::binary|ios::in);

            // opening floppy image
            fstream image(argv[3],ios::binary|ios::out);
            if (!image)
            {
                Error("Problem in Destination",66,false);
            }
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
            // write out the boot sector
            cout<<"Writing the boot sector....";
            bsect.read(ch,512);
            image.write(ch,512);
            cout<<"Done"<<endl;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
            //write out the kernel
            cout<<"Writing the kernel....";
            i=0;
            kern.read(ch,512);
            while((j=kern.gcount())>0)
            {
                i+=j;
                image.write(ch,j);
                kern.read(ch,512);
            }
            j=(512*89)-i; //fill out the rest of the space in the 89 sectors with a char 0
            ch[0]=0;
            for (j;j>0;image.write(ch,1),--j);
            cout<<"Done"<<endl;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
            // now to make the file system on the floppy
            cout<<"Now doing the filesystem"<<endl;
            int count=0;

            bzero((void*)&file_table[0],sizeof(file_table));
            for (i=1;i<=90;i++)
                file_table[i].info.file_number=char(i);

            while (count<=90)
            {
                cout<<"enter the file number (1-90 : 0 to stop)) :";
                cin>>i;
                if (i==0)
                    break;
                if ( file_table[i].info.valid!=1 )
                {
                    ++count;
                    file_table[i].info.valid=1;
                    cout<<"enter filename :";
                    cin>>file_table[i].path; cin.ignore();
                    file_table[i].info.valid=1;

                    size=getsize(file_table[i].path);

                    if(size<0)
                    {
                        cout<<"Cannot open file. go to next name"<<endl;
                        file_table[i].info.valid=0;
                        --count;
                        continue;
                    }
                    else
                        if (size>15360)
                        {
                            cout<<"File too big. go to next name"<<endl;
                            file_table[i].info.valid=0;
                            --count;
                            continue;
                        }
                        else
                            file_table[i].info.size=size;
                    
                    cout<<"Is file a system file (y/n (default:n))     :";
                    cin.get(ch[0]); cin.ignore();
                    cout<<"Is file an executable file (y/n (default:n)):";
                    cin.get(ch[1]); cin.ignore();
                    cout<<"Is file a readonly file (y/n (default:n))   :";
                    cin.get(ch[2]); cin.ignore();

                    if(ch[0]=='y' || ch[0]=='Y')
                        file_table[i].info.attr|=SYSTEM;
                    if(ch[1]=='y' || ch[1]=='Y')
                        file_table[i].info.attr|=EXECUTABLE;
                    if(ch[2]=='y' || ch[2]=='Y')
                        file_table[i].info.attr|=READONLY;

                    parse_name(file_table[i].path,file_table[i].info.name);
                }
                else
                    cout<<"place is already filled"<<endl;

            }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
            // writing out the file table
            cout<<"Writing the filesystem table....";
            for(i=1;i<=90;++i)
                image.write((char *) &(file_table[i].info),512);
            cout<<"Done"<<endl;

            // write out the files
            cout<<"Writing the filesystem....";
            for(i=1;i<=90;++i)
            {
                
                for(j=0;j<514;ch[j++]=0);
                fstream fil(file_table[i].path,ios::binary|ios::in);
                if(!fil)
                    for(j=0;j<30;++j)
                        image.write(ch,512);
                else
                {
                    j=0;
                    fil.read(ch,512);
                    int k;
                    while((k=fil.gcount())>0)
                    {
                        j+=k;
                        image.write(ch,k);
                        fil.read(ch,512);
                    }
                    k=15360-j; // fill out the rest of the space in the 30
                    ch[0]=0;   // sectors with a char 0
                    for (k;k>0;image.write(ch,1),--k);
                }
            }
            cout<<"Done"<<endl;
        }
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
        else
        {
            if (strcmpi(argv[1],"/h")!=0 && strcmpi(argv[1],"/?")!=0 && strcmpi(argv[1],"-h")!=0)
                Error("Invalid switch or argument\n",0,true);
            help_message();
        }
      
}

