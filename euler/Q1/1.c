#include <stdio.h>

#define LIMIT 1000

int main (int argc, char *argv[])
{
	int i=0;
	int sum=0;

	for (i=1;i<LIMIT;++i)
		if (i%3==0 || i%5==0 )
			sum+=i;
	printf("Sum = %d\n",sum);
	return 0;
}
