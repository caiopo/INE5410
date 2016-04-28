#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

int global_counter = 0;

sem_t* semaphore;

void* inc(void* args) {

	for (int i = 0; i < 100; ++i) {
		sem_wait(semaphore);
		++global_counter;
		sem_post(semaphore);
	}

	pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		printf("You must specify the number of threads!");
		return 1;
	}

	int max_threads = atoi(argv[1]);
	pthread_t threads[max_threads];

	semaphore = malloc(sizeof(sem_t));
	sem_init(semaphore, 0, 1);

	for (int i = 0; i < max_threads; ++i) {
		pthread_create(&threads[i], NULL, inc, NULL);
	}

	for (int i = 0; i < max_threads; ++i) {
		pthread_join(threads[i], NULL);
	}

	sem_destroy(semaphore);

	free(semaphore);

	printf("Final value of counter: %d\n", global_counter);
}
