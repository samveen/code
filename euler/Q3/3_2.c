#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



#define NUMBER UINT64_C(600851475143)

int main (int argc, char *argv[])
{
	uint64_t num=NUMBER, div=0, root=0, quot=0;

	while ((num&1)==0)
		num=num>>1;
	if (num==1)
		num=2;
	root=llrint(sqrt(num));

	div=3;
	while (div<=root) {
		if (num%div==0 /*&& num!=div*/) {
			num/=div;
			div=3;
			root=llrint(sqrt(num));
		} else
			div+=2;
	}

	printf("Largest factor: %lld\n", num);
}
