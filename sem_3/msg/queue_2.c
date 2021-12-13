#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define ullong unsigned long long
#define CHECKR(condition,message);\
        if (condition)\
        {\
            perror(message);\
            exit(EXIT_FAILURE);\
        }\

typedef struct msg
{
    ullong type;
    char data;
}msg_t;

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

    pid_t pid = getpid();
    pid_t ppid = pid;
    ullong ch_num = 0;

    errno = 0;
    int msqid = msgget(IPC_PRIVATE, 0666);
    CHECKR(errno, "msgget() error");

	for (ullong i = 1; pid && (i <= num); i++)
    {
        errno = 0;
        pid = fork();
        CHECKR(errno, "fork() error");

        if (!pid)
            ch_num = i;
    }

    msg_t msg = {};
    if (getpid() == ppid)
    {
        msg.type = 1;

        errno = 0;
        msgsnd(msqid, &msg, 1, 0);
        CHECKR(errno, "msgsnd() error");

        errno = 0;
        msgrcv(msqid, &msg, 1, num + 1, 0);
        CHECKR(errno, "msgrcv() error");
    }
    else
    {
        errno = 0;
        msgrcv(msqid, &msg, 1, ch_num, 0);
        CHECKR(errno, "msgrcv() error");

        printf("%llu ", ch_num);
        fflush(stdout);

        msg.type = ch_num + 1;

        errno = 0;
        msgsnd(msqid, &msg, 1, 0);
        CHECKR(errno, "msgsnd() error");
    }

    if (getpid() == ppid)
        msgctl(msqid, IPC_RMID, NULL);

	return EXIT_SUCCESS;
}
