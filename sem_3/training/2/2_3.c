#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define PLUS 10000

void *thread_func (void *thread_data)
{
	*(int *)thread_data = *(int *)thread_data + 1;

	return NULL;
}

int main (int argc, char *argv[])
{
	if (argc != 3)
	{
		printf ("wrong amount of argc\n");
		exit (1);
	}
	char *endptr;
	int n = strtol (argv[1], &endptr, 10);
	if (*endptr)
	{
		printf ("wrong argument 1\n");
		exit (2);
	}
	int num = strtol (argv[2], &endptr, 10);
	if (*endptr)
	{
		printf ("wrong argument 2\n");
		exit (2);
	}

	int thread_data = num;
	pthread_t *threads = (pthread_t *) malloc (PLUS * sizeof (pthread_t));
	for (int i = 0; i < n; i++)
	{
		for (int x = 0; x < PLUS; x++)
			pthread_create (&(threads[x]), NULL, thread_func, &thread_data);
		for (int x = 0; x < PLUS; x++)
			pthread_join (threads[x], NULL);
	}
	printf ("%d\n", thread_data);
	free (threads);

	return 0;
}
