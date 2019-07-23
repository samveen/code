#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

uint32_t MAX=2000000;

uint32_t *sieve;

void init() {
    int mask= 0xaaaaaaaa; /* Mask for even numbers to  be marked composite*/
    size_t size=(MAX%(sizeof(uint32_t)*8)) ? ( (MAX/(sizeof(uint32_t)*8)+1)*sizeof(uint32_t) ) : ( (MAX/(sizeof(uint32_t)*8))*sizeof(uint32_t) );
    sieve=(uint32_t *)malloc(size); /* Bit array rounded up to multiple of sizeof(uint32_t)*/
    memset((void *)sieve, mask, size); /* mark even numbers composite */
}

uint32_t isComposite(uint32_t x) {
    size_t index =(--x)/(sizeof(uint32_t)*8);
    size_t offset=(x)%(sizeof(uint32_t)*8);
    return(sieve[index]&(0x01<<offset));
}

uint32_t isCompositeIO(size_t index, size_t offset) {
    return(sieve[index]&offset);
}

void setComposite(uint32_t x) {
    size_t index =(--x)/(sizeof(uint32_t)*8);
    size_t offset=(x)%(sizeof(uint32_t)*8);
    sieve[index]|=(0x01<<offset);
}

int main(int argc, char * argv[]) {
    uint32_t root_max=0;
    uint32_t begin=3,beginI=0,beginO=0x04;
    uint32_t number;
    uint32_t temp,tempI,tempO;

    if(argc==2)
        MAX=atoi(argv[1]);
    
    init();
    root_max=(uint32_t)(sqrt(MAX))+1;
    
    printf("2 ");
    while(begin<root_max) {
        printf("%d ", begin);
        number=begin*begin;
        while(number<=MAX) {
            setComposite(number);
            number+=begin*2;
        }
        do { begin+=2; beginO<<=2; if(!beginO){beginI+=1;beginO=0x1;} } while(isCompositeIO(beginI,beginO) && begin <root_max);
    }
    while(begin<=MAX) {
        if(!isCompositeIO(beginI,beginO))
            printf("%d ", begin);
        begin+=2; beginO<<=2; if(beginO==0){beginI+=1;beginO=0x1;}
    }

    return(0);
}
