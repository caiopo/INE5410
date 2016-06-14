#include <stdio.h>
#include <mpi.h>
#include <string.h>

char msg[6];

int rank;

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (!rank)
		strcpy(msg, "teste");

	MPI_Bcast(&msg, sizeof(msg), MPI_CHAR, 0, MPI_COMM_WORLD);

	// printf("%s from %d\n", msg, rank);

	if (rank)
		MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	else {
		int size, recv_msg;

		MPI_Comm_size(MPI_COMM_WORLD, &size);

		for (int i = 0; i < size-1; ++i) {
			MPI_Recv(&recv_msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Received message from rank %d\n", recv_msg);
		}
	}

	MPI_Finalize();
}