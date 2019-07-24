/* This is a simple implementation of the bzero function provided in the unix
 * development environment but not available in the the windows environment
 *
 * Copyright (c) 2003 onwards, Samveen S. Gulati.
 */

#ifndef _FUNCTIONS_H
#ifdef __GNUG__
#pragma interface
#endif
#define _FUNCTIONS_H

void* bzero(void* v,int size)
{
    char *arr=(char*)v;
    for(int i=0;i<size;arr[i]=0,i++);
    return(v);
}

#endif /*!_FUNCTIONS_H*/
