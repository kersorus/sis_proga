#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define ullong unsigned long long

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		printf ("wrong amount of argc\n");
		exit (1);
	}

	char *endptr;
	ullong n = strtoull (argv[1], &endptr, 10);
	if (*endptr)
	{
		printf ("wrong argument\n");
		exit (2);
	}

	ullong i = 0;
	pid_t pid = 1;

	printf ("parent: ppid = %d\n", getppid());

	while (pid && (i < n))
	{
		pid = fork ();
		if (!pid)
			printf ("child: pid = %d\n", getpid ());
        wait(NULL);
		i++;
	}

	return 0;
}
