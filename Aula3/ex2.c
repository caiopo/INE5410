#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 10

int* buffer;

sem_t *cheio, *vazio, *lock_prod, *lock_cons;

int cons_i = 0;
int prod_i = 0;

void* produz(void* args) {

	while (1) {
		sem_wait(vazio);
		sem_wait(lock_prod);

		prod_i = (prod_i+1) % BUFFER_SIZE;
		buffer[prod_i] = 1;

		printf("Produzi no indice %d ID: %d\n", prod_i, (int)args);

		sem_post(lock_prod);
		sem_post(cheio);

		// sleep(1);
	}

	pthread_exit(NULL);
}

void* consome(void* args) {

	while (1) {
		sem_wait(cheio);
		sem_wait(lock_cons);

		cons_i = (cons_i+1) % BUFFER_SIZE;
		buffer[cons_i] = 0;

		printf("Consumi no indice %d ID: %d\n", cons_i, (int)args);

		sem_post(lock_cons);
		sem_post(vazio);

		sleep(1);
	}

	pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
	if (argc != 3) {
		printf("Usage: ./ex2 <nprod> <ncons>");
		return 1;
	}

	buffer = malloc(sizeof(int) * BUFFER_SIZE);

	cheio = malloc(sizeof(sem_t));
	vazio = malloc(sizeof(sem_t));
	lock_prod = malloc(sizeof(sem_t));
	lock_cons = malloc(sizeof(sem_t));

	sem_init(cheio, 0, 0);
	sem_init(vazio, 0, BUFFER_SIZE);
	sem_init(lock_prod, 0, 1);
	sem_init(lock_cons, 0, 1);

	int produtoras = atoi(argv[1]);
	int consumidoras = atoi(argv[2]);

	pthread_t threads[consumidoras+produtoras];

	int n = 0;

	for (; n < consumidoras; ++n) {
		pthread_create(&threads[n], NULL, consome, n);
	}

	for (; n < consumidoras+produtoras; ++n) {
		pthread_create(&threads[n], NULL, produz, n-consumidoras);
	}


	// for (int i = 0; 1; i = (i+1) % BUFFER_SIZE)
	// 	printf("%d %d\n", buffer[i], i);


	for (int i = 0; i < consumidoras+produtoras; ++i) {
		pthread_join(threads[i], NULL);
	}

	sem_destroy(cheio);
	sem_destroy(vazio);
	sem_destroy(lock_prod);
	sem_destroy(lock_cons);

	free(cheio);
	free(vazio);
	free(lock_prod);
	free(lock_cons);
}
