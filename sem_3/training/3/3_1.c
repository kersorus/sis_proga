#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF 4096

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("#  wrong arg amnt\n");
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    char buf[BUF] = "";
    if (pipe(pipefd) == -1)
    {
        printf("#   prblema w pipe\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        printf("#   prblema w fork\n");
        exit(EXIT_FAILURE);
    }

    if (!pid)
    {
        close(pipefd[0]);

        int file = open(argv[1], O_RDONLY);
        if (file == -1)
        {
            printf("#  problema w file");
            exit(EXIT_FAILURE);
        }

        int samnt = 1;
        while ( (samnt = read(file, &buf, 4096) ) > 0 )
            write(pipefd[1], &buf, samnt);

        close(pipefd[1]);
        _exit(EXIT_SUCCESS);
    }
    else
    {
        close(pipefd[1]);
        int samnt = read(pipefd[0], &buf, 4096); 
        while (samnt > 0)
        {
            write(1, &buf, samnt);
            samnt = read(pipefd[0], &buf, 4096); 
        }

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }
}
