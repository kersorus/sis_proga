#include <unistd.h>

int main (int argc, char *argv[])
{
	execv ("/bin/ls", argv);

	return 0;
}
