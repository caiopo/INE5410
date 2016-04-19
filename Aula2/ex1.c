#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

void* inc(void* args) {
	printf("Hello, world! I'm %lu.\n", pthread_self());
	pthread_exit(NULL);
}

int main() {
	pthread_t thread;

	pthread_create(&thread, NULL, inc, NULL);
	pthread_join(thread, NULL);
}