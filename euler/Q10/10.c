#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

uint32_t MAX=2000000;

uint32_t *sieve;
uint64_t sum=2;

void init() {
    size_t size=(MAX%(sizeof(uint32_t)*8)) ? ( (MAX/(sizeof(uint32_t)*8)+1)*sizeof(uint32_t) ) : ( (MAX/(sizeof(uint32_t)*8))*sizeof(uint32_t) );
    sieve=(uint32_t *)malloc(size); /* Bit array rounded up to multiple of sizeof(uint32_t)*/
    memset((void *)sieve, 0, size); /* zero it out */
}

uint32_t isComposite(uint32_t x) {
    size_t index =x/(sizeof(uint32_t)*8);
    size_t offset=x%(sizeof(uint32_t)*8);
    return(sieve[index]&(0x01<<offset));
}

void setComposite(uint32_t x) {
    size_t index =x/(sizeof(uint32_t)*8);
    size_t offset=x%(sizeof(uint32_t)*8);
    sieve[index]|=(0x01<<offset);
}

int main(int argc, char * argv[]) {
    uint32_t root_max=0;
    uint32_t begin=3;
    uint32_t index;

    if(argc==2)
        MAX=atoi(argv[1]);

    init();
    root_max=(uint32_t)(sqrt(MAX))+1;

    while(begin<root_max) {
        sum+=begin;
        index=begin*begin;
        while(index <=MAX) {
            setComposite(index);
            index+=begin;
        }
        do { begin+=2; } while(isComposite(begin) && begin <root_max);
    }
    if(root_max&0x1)
        begin=root_max;
    else
        begin=root_max-1;

    for(; begin<=MAX ; begin+=2)
        if(!isComposite(begin))
            sum+=begin;

    printf("%zu\n", sum);
    return(0);
}
