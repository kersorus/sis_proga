#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define CHECKR(condition,message)\
        if (condition)\
        {\
            perror(message);\
            exit(EXIT_FAILURE);\
        }\

#define ALRM_TIME 3

void usr1_handler(int signum);
void usr2_handler(int signum);
void alrm_handler(int signum);

uint8_t end;

struct buffer
{
    uint8_t data[BUFSIZ];
    size_t  byte;
    uint8_t bit;
} buff;

struct sigaction act = {};

pid_t pid;

int main(int argc, char **argv)
{    
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <name_of_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sigset_t set = {};

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGALRM);
    errno = 0;
    sigprocmask(SIG_SETMASK, &set, NULL);
    CHECKR(errno, "sigprocmask() error");

    sigemptyset(&set);

    act.sa_handler = alrm_handler;
    errno = 0;
    sigaction(SIGALRM, &act, NULL);
    CHECKR(errno, "sigaction() error");

    sigaddset(&act.sa_mask, SIGALRM);

    struct sigaction act = {.sa_handler = usr1_handler};
    errno = 0;
    sigaction(SIGUSR1, &act, NULL);
    CHECKR(errno, "sigaction() error");

    act.sa_handler = usr2_handler;
    errno = 0;
    sigaction(SIGUSR2, &act, NULL);
    CHECKR(errno, "sigaction() error");

    errno = 0;
    pid_t pid = fork();
    CHECKR(errno, "fork() error");

    if (pid > 0)
    {
        errno = 0;
        int fd = open(argv[1], O_RDONLY);
        CHECKR(errno, "open() error");

        while (1)
        {
            errno = 0;
            ssize_t nbytes = read(fd, buff.data, BUFSIZ);
            CHECKR(errno, "read() error");

            if (!nbytes)
                exit (EXIT_SUCCESS);
        
            buff.byte = 0;
            buff.bit  = 0;

            while (buff.byte < (size_t) nbytes)
            {
                if (buff.data[buff.byte] & (1 << buff.bit))
                    kill(pid, SIGUSR1);
                else
                    kill(pid, SIGUSR2); 

                alarm(ALRM_TIME);

                sigsuspend(&set);

                alarm(0);

                if (buff.bit >= 8)  
                {
                    buff.bit   = 0;
                    buff.byte += 1;
                }
            } 
        }
    }
    else
    {
        pid = getppid();

        while (1)
        {
            alarm(ALRM_TIME);        

            sigsuspend(&set);

            alarm(0);

            if (end)
            {
                errno = 0;
                write(STDOUT_FILENO, buff.data, buff.byte);
                CHECKR(errno, "write() error");

                exit (EXIT_SUCCESS);
            }

            if (buff.bit >= 8)
            {
                buff.byte++;
                buff.bit = 0;
            }

            if (buff.byte == BUFSIZ)
            {
                errno = 0;
                write(STDOUT_FILENO, buff.data, BUFSIZ);
                CHECKR(errno, "write() error");

                buff.byte = 0;
            }

            kill(pid, SIGUSR1);
        }
    }
}

void usr1_handler(int signum)
{
    buff.data[buff.byte] |= (1 << buff.bit);
    buff.bit++;
}

void usr2_handler(int signum)
{
    buff.data[buff.byte] &= ~(1 << buff.bit);
    buff.bit++;
}

void alrm_handler(int signum)
{
    end = 1;
}
