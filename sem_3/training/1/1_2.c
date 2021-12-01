#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

int main (int argc, char *argv[])
{
	if (argc != 3)
	{
		printf ("#	wrong amount of args\n");
		exit (1);
	}

	FILE *old = fopen (argv [1], "r");
	if (!old)
	{
		printf ("#	wrong filename\n");
		exit (2);
	}

	FILE *new = fopen (argv [2], "w");

	char temp[BUFSIZE] = "";
	while (!feof(old))
	{
		size_t amount1 = fread  (temp, sizeof (char), BUFSIZE, old);
		if (amount1 != BUFSIZE)
		{
			printf ("# error with reading file\n");
			exit (3);
		}
		size_t amount2 = fwrite (temp, sizeof (char), amount1, new);
		if (amount2 != BUFSIZE)
		{
			printf ("# error with writing file\n");
			exit (4);
		}

	}

	fclose(old);
	fclose(new);

	return 0;
}
