#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define ullong unsigned long long
#define BUF_ST 3
#define BUF_MAX 1594323
#define TIMEOUT 10
#define MAX(a,b) (a > b) ? a : b 
#define CHECKR(condition,message);\
        if (condition)\
        {\
            perror(message);\
            exit(EXIT_FAILURE);\
        }\

struct timeval timeout = {.tv_sec = TIMEOUT};

typedef struct connection
{
    int fd_wr;
    int fd_rd;
    size_t buffer_size;
    char *buffer;
    size_t loc;
    size_t disloc;
}con;

size_t count(ullong num, ullong i)
{
    size_t size = BUF_ST;
    for(ullong j = 1; j < num - i + 4; j++)
    {
        size *= BUF_ST;
        if (size > BUF_MAX)
        {
            size = BUF_MAX;
            break;
        }
    }

    return size;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("#	wrong amount of args\n");
		exit(EXIT_FAILURE);
	}

	char *endptr;
	ullong num = strtoull(argv[1], &endptr, 10);
	if (*endptr != 0)
	{
		printf("#	wrong argument 1\n");
		exit(EXIT_FAILURE);
	}

    int nfds = 0;
    fd_set rfds;
    fd_set wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    con child  = {};
    con *parent = (con *) calloc(num, sizeof(con));
    if (parent == 0)
    {
        fprintf(stderr, "calloc() error");
        exit(EXIT_FAILURE);
    }

    int pipefd[4];

    pid_t ppid = getpid();
    pid_t pid = 0;

    for (ullong i = 1; i <= num; i++)
    {
        size_t buf_size = count(num, i);

        if ((i == (ullong) 1) || (i == num))
        {
            errno = 0;
            pipe(pipefd);
            CHECKR(errno, "pipe() error");

            errno = 0;
            pid = fork();
            CHECKR(errno, "fork() error");

            if (!pid)
            {
                if (i == (ullong) 1)
                {
                    errno = 0;
                    close(pipefd[0]);
                    CHECKR(errno, "close() error");

                    errno = 0;
                    child->fd_rd = open(argv[2], O_RDONLY);
                    CHECKR(errno, "open() error");

                    nfds = MAX(nfds, pipefd[1] + 1);

                    child->fd_wr = pipefd[1];
                }
                else
                {
                    errno = 0;
                    close(pipefd[1]);
                    CHECKR(errno, "close() error");

                    child->fd_wr = STDOUT_FILENO;

                    nfds = MAX(nfds, pipefd[0] + 1);

                    child->fd_rd = pipefd[0];
                }

                child->buffer_size = buf_size;
                child->buffer = (char *) calloc(buf_size, sizeof(char));
                if (child->buffer == 0)
                {
                    fprintf(stderr, "calloc() error");
                    exit(EXIT_FAILURE);
                }

                for (ullong j = 0; j < i; j++)
                    {
                        close(parent[j]->fd_wr);
                        close(parent[j]->fd_rd);
                    }
                free(parent->buffer);
                free(parent);
                break;
            }
            else
            {
                if (i = (ullong) 1)
                {
                    errno = 0;
                    close(pipefd[1]);
                    CHECKR(errno, "pipe() error");

                    parent[i]->fd_rd = pipefd[0];

                    FD_SET(pipefd[0], &rfds);
                    nfds = MAX(nfds, pipefd[0] + 1);
                
                    parent[i]->buffer_size = buf_size;
                    parent[i]->buffer = (char *) calloc(buf_size, sizeof(char));
                    if (parent[i]->buffer == 0)
                    {
                        fprintf(stderr, "calloc() error");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    errno = 0;
                    close(pipefd[0]);
                    CHECKR(errno, "pipe() error");

                    parent[i - 1]->fd_wr = pipefd[1];

                    FD_SET(pipefd[1], &wfds);
                    nfds = MAX(nfds, pipefd[1] + 1);                    
                }
            }
        }
        else
        {
            errno = 0;
            pipe(pipefd);
            CHECKR(errno, "pipe() error");

            errno = 0;
            pipe(pipefd + 2);
            CHECKR(errno, "pipe() error");

            errno = 0;
            pid = fork();
            CHECKR(errno, "fork() error");

            if (!pid)
            {
                    errno = 0;
                    close(pipefd[0]);
                    CHECKR(errno, "close() error");
                    errno = 0;
                    close(pipefd[3]);
                    CHECKR(errno, "close() error");

                    nfds = MAX(nfds, MAX(pipefd[2] + 1, pipefd[1] + 1));

                    child->fd_wr = pipefd[1];
                    child->fd_rd = pipefd[2];
                    child->buffer_size = buf_size;
                    child->buffer = (char *) calloc(buf_size, sizeof(char));
                    if (child->buffer == 0)
                    {
                        fprintf(stderr, "calloc() error");
                        exit(EXIT_FAILURE);
                    }

                    for (ullong j = 0; j < i; j++)
                    {
                        close(parent[j]->fd_wr);
                        close(parent[j]->fd_rd);
                    }
                    free(parent->buffer);
                    free(parent);
                    break;              
            }
            else
            {
                    errno = 0;
                    close(pipefd[1]);
                    CHECKR(errno, "close() error");
                    errno = 0;
                    close(pipefd[2]);
                    CHECKR(errno, "close() error");

                    nfds = MAX(nfds, MAX(pipefd[3] + 1, pipefd[0] + 1));

                    parent[i - 1]->fd_wr = pipefd[3];
                    parent[i]->fd_rd = pipefd[0];
                    parent[i]->buffer_size = buf_size;
                    parent[i]->buffer = (char *) calloc(buf_size, sizeof(char));
                    if (parent[i]->buffer == 0)
                    {
                        fprintf(stderr, "calloc() error");
                        exit(EXIT_FAILURE);
                    }

                    FD_SET(pipefd[0], &rfds);
                    FD_SET(pipefd[3], &wfds);
            }
        }
    }
}

if (ppid == getpid())
    {
        size_t fnsh = 0;

        for (ullong i = 1; i <= num; i++)
        {
            errno = 0;
            fcntl(parent[i]->fd_rd, F_SETFL, O_RDONLY | O_NONBLOCK);
            CHECKR(errno, "fcntl() error");

            errno = 0;
            fcntl(parent[i]->fd_wr, F_SETFL, O_WRONLY | O_NONBLOCK);
            CHECKR(errno, "fcntl() error");
        }

        while (fnsh != num)
        {
            int fd_rdy = 0;
            fd_set rfds_cpy = rfds;
            fd_set wfds_cpy = wfds;
            
            errno = 0;
            fd_rdy = select (nfds, &rfds_cpy, &wfds_cpy, NULL, &timeout);
            CHECKR(errno, "select() error");

            if (!fd_rdy)
                exit (EXIT_SUCCESS);

            for (ullong i = 1; i <= num; i++)
            {
                if (FD_ISSET(parent[i]->fd_rd, &rfds_cpy))
                {
                    errno = 0;
                    parent[i]->loc = read(parent[i]->fd_rd, parent[i]->buffer, parent[i]->buffer_size);
                    CHECKR(errno, "read() error")

                    if (!parent[i]->loc)
                    {
                        FD_CLR(parent[i]->fd_rd, &rfds);
                        close(parent[i]->fd_rd);
                        close(parent[i]->fd_wr);
                        free(parent[i]->buffer);

                        fnsh++;
                    }
                    else
                    {
                        FD_CLR(parent[i]->fd_rd, &rfds);
                        FD_SET(parent[i]->fd_wr, &wfds);
                    }
                }

                if (FD_ISSET(parent[i]->fd_wr, &wfds_cpy))
                {
                    errno = 0;
                    size_t b_amnt = write (parent[i]->fd_wr, parent[i]->buffer + parent[i]->disloc, parent[i]->loc);
                    CHECKR(errno, "write() error");
    
                    parent[i]->disloc += b_amnt;
                    parent[i]->loc -= b_amnt;

                    if (!parent[i]->loc)
                    {
                        FD_CLR(parent[i]->fd_wr, &wfds);
                        FD_SET(parent[i]->fd_rd, &rfds);
                        parent[i]->disloc = 0;
                    }
                }
            }
        }          

        free (parent);
        exit (EXIT_SUCCESS);
    }
    else
    {
        while (TRUE)
        {
            errno = 0;
            child.loc = read(child.fd_rd, child.buffer, child.buffer_size);
            CHECKR(errno, "read() error");

            if (!child.loc)
                exit (EXIT_SUCCESS);

            errno = 0;
            write(child.fd_wr, child.buffer, child.loc);
            CHECKR(errno, "write() error");
        }
    }
}