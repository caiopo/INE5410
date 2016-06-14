#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

unsigned int compute_pi(unsigned int, unsigned int);

int rank;

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	unsigned int pontos;

	if(argc != 2){
		printf("Uso:\n");
		printf("\t%s <numero de pontos a serem sorteados>\n", argv[0]);
		return 1;
	}

	pontos = atoi(argv[1]);

	// retorna quantos pontos sorteados cairam dentro do circulo
	// aqui estamos considerando uma semente para o gerador de
	// numeros aleatorios fixa = 0

	if (!rank) {
		int size;
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		int pontos_por_processo = pontos/(size-1);

		for (int i = 0; i < size-1; ++i)
			MPI_Send(&pontos_por_processo, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);


		int total = 0;
		int received;

		for (int i = 0; i < size-1; ++i) {
			MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			total += received;
		}

		printf("Pi = %.040f\n", ((double)total/(double)pontos) * 4);
	} else {
		int msg;

		MPI_Recv(&msg, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int calculado = compute_pi(rank, msg);

		printf("Recebi: %d Calculei %d\n", msg, calculado);

		MPI_Send(&calculado, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	// calcula a aproximacao de Pi baseado nos pontos sorteados

	MPI_Finalize();

	return 0;
}

unsigned int compute_pi(unsigned int seed, unsigned int pontos){
	unsigned int i;
	unsigned int pontos_no_circulo;
	double x, y;

	pontos_no_circulo = 0;
	srand(seed);

	for(i = 0; i < pontos; i++){
		// sorteia um ponto: coordenadas x e y dentro do quadrado
		// consideramos que R = 1, então x e y pertencem ao intervalo [0; 1]
		y = (double)(rand())/(double)(RAND_MAX);
		x = (double)(rand())/(double)(RAND_MAX);

		// verifica se o ponto sorteado encontra-se dentro do circulo
		// um ponto (x, y) esta dentro do circulo se: x^2 + y^2 < R^2
		// nesse caso, consideramos R = 1
		if((x*x + y*y) < 1)
			pontos_no_circulo++;

	}

	return pontos_no_circulo;
}
