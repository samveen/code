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
    return(sieve[index]&(0x01<<offset));
}

void setComposite(uint32_t x) {
    size_t index =(--x)/(sizeof(uint32_t)*8);
    size_t offset=(x)%(sizeof(uint32_t)*8);
    sieve[index]|=(0x01<<offset);
}

int main(int argc, char * argv[]) {
    uint32_t root_max=0;
    uint32_t begin=3;
    uint32_t index;
    uint32_t temp;

    if(argc==2)
        MAX=atoi(argv[1]);
    
    init();
    root_max=(uint32_t)(sqrt(MAX))+1;
    
    printf("2 ");
    while(begin<root_max) {
        printf("%d ", begin);
	/* One */
        index=begin*begin;
        while(index <=MAX) {
            setComposite(index);
            index+=begin*2;
        }
	/* two 
	temp=begin;
	index=temp*begin;
        while(index <=MAX) {
            setComposite(index);
            do { temp+=2; } while(isComposite(temp) && temp <root_max);
	    index=temp*begin;
        }
	* done */
        do { begin+=2; } while(isComposite(begin) && begin <root_max);
    }
    for(begin=root_max; begin<=MAX ; ++ begin)
        if(!isComposite(begin))
            printf("%d ", begin);

    return(0);
}
