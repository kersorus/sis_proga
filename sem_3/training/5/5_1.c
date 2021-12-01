#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define ullong unsigned long long
#define CHECKR(condition,message);\
        if (condition)\
        {\
            perror(message);\
            exit(EXIT_FAILURE);\
        }\

typedef struct _msg_
{
    long type;
    ullong num;
}msg_t;

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		printf ("wrong amount of argc\n");
		exit (EXIT_FAILURE);
	}

	char *endptr;
	ullong n = strtoull (argv[1], &endptr, 10);
	if (*endptr)
	{
		printf ("wrong argument\n");
		exit (EXIT_FAILURE);
	}




	return 0;
}
