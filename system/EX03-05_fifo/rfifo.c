#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define MAX_BUF 512
#define PATH_FIFO "./my_fifo"

pid_t pid;

int main(int argc, char **argv)
{
	int fd_fifo;
	int ret;
	char rbuf[MAX_BUF];
	int i;


	printf("[%d] running %s\n", pid = getpid(), argv[0]);

	/* make fifo */
	ret = mkfifo(PATH_FIFO, 0777);
	if(ret == -1) {
		if(errno == EEXIST) {
			printf("[%d] %s already exists\n", pid, PATH_FIFO);
		}
		else {
			printf("[%d] error: %s\n", pid, strerror(errno));
			return EXIT_FAILURE;
		}
	}

	/* open fifo */
	fd_fifo = open(PATH_FIFO, O_RDONLY);
	if(fd_fifo == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		return EXIT_FAILURE;
	}

	/* read data to pipe */
	ret = read(fd_fifo, rbuf, MAX_BUF);
	if(ret == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("[%d] read %d bytes from %s [%s]\n", pid, ret, PATH_FIFO, rbuf);

	close(fd_fifo);

	printf("[%d] terminted\n", pid);

	return EXIT_SUCCESS;
}
