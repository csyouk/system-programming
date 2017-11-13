#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pid_t pid;

int main(int argc, char **argv)
{
	int i;
	char *s;

	printf("[%d] running %s", pid = getpid(), argv[0]);

	printf("[%d] sleeping 7 seconds\n", pid);
	sleep(7);

	printf("[%d] terminted\n", pid);

	return EXIT_SUCCESS;
}	
