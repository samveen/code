#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



#define NUMBER UINT64_C(600851475143)

int main (int argc, char *argv[])
{
	int8_t *sieve=NULL;
	int8_t *factors=NULL;
	uint64_t size=0, limit=0, i=0, j=0;

	size=llrint(sqrt(NUMBER));
	//size=NUMBER/UINT64_C(2); ++size;
	sieve=malloc((size_t) size*sizeof(int8_t));
	memset((void*) sieve, 0, size*sizeof(int8_t));

	factors=malloc((size_t) size*sizeof(int8_t));
	memset((void*) factors, 0, size*sizeof(int8_t));

	sieve[0]=sieve[1]=sieve[2]=sieve[3]=1;

	for(i=4;i<size;++i) {
		limit=llrint(sqrt(i));
		for (j=2;j<=limit;++j)
			if (sieve[j]==1) { 
				if (i%j==0) {
					break;
				}
			}
		if (j>limit)
			sieve[i]=1;
		if (NUMBER%i==0) {
			factors[i]=1;
		}
        }

	for(i=size-1;factors[i]!=1 || sieve[i]!=1;--i);
	printf("%lld\n", i);
}
