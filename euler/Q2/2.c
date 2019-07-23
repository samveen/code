#include <stdio.h>

#define LIMIT 4000000

int main (int argc, char *argv[])
{
	int n1=0,n2=1,n3=0;
	int sum=0;

	while (n2<=LIMIT) {
		if ((n2&1)==0) {
			sum+=n2;
		}
		n3=n1+n2;
		n1=n2;
		n2=n3;
        }
	printf("Sum = %d\n",sum);
	return 0;
}
