#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>

pid_t pid;
int g_count;
pthread_mutex_t g_mutex;

void *thread_func1(void *arg)
{
	int i, temp, count = 0;

	printf("[%d] thread1 started with arg \"%d\"\n", pid, *(int *)arg);
	for(i=0; i<*(int *)arg; i++) {
		pthread_mutex_lock(&g_mutex);
		temp = g_count;
		usleep(1);
		g_count = ++temp;
		count++;
		pthread_mutex_unlock(&g_mutex);
		usleep(2);
	}

	printf("[%d] thread1 counted %d\n", pid, count);
	pthread_exit(NULL);
}

void *thread_func2(void *arg)
{
	int i, temp, count = 0;

	printf("[%d] thread2 started with arg \"%d\"\n", pid, *(int *)arg);
	for(i=0; i<*(int *)arg; i++) {
		pthread_mutex_lock(&g_mutex);
		temp = g_count;
		usleep(1);
		g_count = ++temp;
		count++;
		pthread_mutex_unlock(&g_mutex);
		usleep(2);
	}

	printf("[%d] thread2 counted %d\n", pid, count);
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	pthread_t thread_id1, thread_id2;
	void *thread_ret;
	int ret;
	int n = 1000;

	printf("[%d] running %s\n", pid = getpid(), argv[0]);

	ret = pthread_mutex_init(&g_mutex, NULL);
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}

	printf("[%d] creating thread (g_count = %d)\n", pid, g_count);
	ret = pthread_create(&thread_id1, NULL, thread_func1, &n);
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}
	ret = pthread_create(&thread_id2, NULL, thread_func2, &n);
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}

	printf("[%d] waiting to join with a terminated thread\n", pid);
	ret = pthread_join(thread_id1, NULL);
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}
	ret = pthread_join(thread_id2, NULL);
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}
	printf("[%d] all threads terminated (g_count = %d)\n", pid, g_count);

	pthread_mutex_destroy(&g_mutex);
	printf("[%d] terminted\n", pid);

	return EXIT_SUCCESS;
}

/* OUTPUT
	[15027] running ./pthread_mutex
	[15027] creating thread (g_count = 0)
	[15027] waiting to join with a terminated thread
	[15027] thread1 started with arg "1000"
	[15027] thread2 started with arg "1000"
	[15027] thread1 counted 1000
	[15027] thread2 counted 1000
	[15027] all threads terminated (g_count = 2000)
	[15027] terminted
*/
