#include <stdio.h>
#include <stdlib.h>

#define ullong unsigned long long

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf ("#	wrong amount of args\n");
		exit (1);
	}

	char *endptr;
	ullong num = strtoull (argv[1], &endptr, 10);
	if (*endptr != 0)
	{
		printf ("#	wrong argument\n");
		exit (2);
	}

	ullong i = 1;
	for (i = 1; i <= num; i++)
	{
		printf ("%lld ", i);
	}
	printf ("\n");

	return 0;
}
