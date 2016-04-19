#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int global_counter = 0;

void* inc(void* args) {

	for (int i = 0; i < 100; ++i)
		++global_counter;

	pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		printf("You must specify the number of threads!");
		return 1;
	}

	int max_threads = atoi(argv[1]);
	pthread_t threads[max_threads];

	for (int i = 0; i < max_threads; ++i) {
		pthread_create(&threads[i], NULL, inc, NULL);
	}

	for (int i = 0; i < max_threads; ++i) {
		pthread_join(threads[i], NULL);
	}

	printf("Final value of counter: %d", global_counter);
}
