#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define RANDNUM_W 521288629
#define RANDNUM_Z 362436069

unsigned int randum_w = RANDNUM_W;
unsigned int randum_z = RANDNUM_Z;

void srandnum(int seed) {
	unsigned int w, z;
	w = (seed * 104623) & 0xffffffff;
	randum_w = (w) ? w : RANDNUM_W;
	z = (seed * 48947) & 0xffffffff;
	randum_z = (z) ? z : RANDNUM_Z;
}

unsigned int randnum(void) {
	unsigned int u;
	randum_z = 36969 * (randum_z & 65535) + (randum_z >> 16);
	randum_w = 18000 * (randum_w & 65535) + (randum_w >> 16);
	u = (randum_z << 16) + randum_w;
	return (u);
}

typedef float *vector_t;

int npoints;
int dimension;
int ncentroids;
float mindistance;
int seed;
vector_t *data, *centroids;
int *map;
int *dirty;
int too_far;
int has_changed;

//for rank 0 to gather everything
int *globalmap;
int *globaldirty;
int globalfar;
vector_t *globalcentroids;

int rank, size;

unsigned int min;
unsigned int max;

float v_distance(vector_t a, vector_t b) {
	int i;
	float distance = 0;
	for (i = 0; i < dimension; i++)
		distance += pow(a[i] - b[i], 2);
	return sqrt(distance);
}

// void sync_map() {
// 	for(int i = 0; i < npoints; ++i) {
// 		if(map[i] == -1)
// 			map[i] = 0;
// 	}
// 	MPI_Reduce(map, globalmap, npoints, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
// }

void sync_map() {
	for (int i = 0; i < size; ++i) {
		int minmax[] = {min, max};

		MPI_Bcast(minmax, 2, MPI_INT, i, MPI_COMM_WORLD);

		MPI_Bcast(map+minmax[0], minmax[1]-minmax[0], MPI_INT, i, MPI_COMM_WORLD);
	}

	min = (rank*npoints)/size;
	max = ((rank + 1) * npoints)/size;

}


void sync_centroids() {
	for(int i = 0; i < ncentroids; ++i)
		MPI_Bcast(globalcentroids[i], dimension, MPI_FLOAT, 0, MPI_COMM_WORLD);
	for(int i = 0; i < ncentroids; ++i) {
		for(int k = 0; k < dimension; ++k)
			centroids[i][k] = globalcentroids[i][k];
	}
}

static void populate(void) {
	int i, j;
	float tmp;
	float distance;
	too_far = 0;
	globalfar = 0;
	for (i = min; i < max; i++) {
		distance = v_distance(centroids[map[i]], data[i]);
		/* Look for closest cluster. */
		for (j = 0; j < ncentroids; j++) {
			/* Point is in this cluster. */
			if (j == map[i])
				continue;
			tmp = v_distance(centroids[j], data[i]);
			if (tmp < distance) {
				map[i] = j;
				distance = tmp;
				dirty[j] = 1;
			}
		}

		/* Cluster is too far away. */
		if (distance > mindistance)
			too_far = 1;
	}
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Allreduce(dirty, globaldirty, ncentroids, MPI_INT,
			  	  MPI_BOR, MPI_COMM_WORLD);

	MPI_Allreduce(&too_far, &globalfar, 1, MPI_INT,
			   	  MPI_BOR, MPI_COMM_WORLD);
}

static void compute_centroids(void) {
	int i, j, k;
	int population;
	int globalpopulation;
	has_changed = 0;
	/* Compute means. */
	for (i = 0; i < ncentroids; i++) {
		if (!globaldirty[i])
			continue;
		memset (centroids[i], 0, sizeof (float) * dimension);
		memset (globalcentroids[i], 0, sizeof (float) * dimension);
		/* Compute cluster's mean. */
		population = 0;
		globalpopulation = 0;
		for (j = min; j < max; j++) {
			if (map[j] != i)
				continue;
			for (k = 0; k < dimension; k++)
				centroids[i][k] += data[j][k];
			population++;
		}
		//NEED TO FIND A WAY TO SUM ALL THE CENTROIDS[i][k] INTO GLOBALCENTROIDS
		//TO RANK 0 UPDATE IT AND SEND TO EVERYONE
		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Reduce(&population, &globalpopulation, 1, MPI_INT, MPI_SUM,
				   0, MPI_COMM_WORLD);
		MPI_Reduce(centroids[i], globalcentroids[i], dimension, MPI_FLOAT,
				   MPI_SUM, 0, MPI_COMM_WORLD);

		if(rank == 0) {
			if (globalpopulation > 1) {
				for (k = 0; k < dimension; k++)
					globalcentroids[i][k] *= 1.0 / population;
			}
		}
		has_changed = 1;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	sync_centroids();
	MPI_Barrier(MPI_COMM_WORLD);
	memset (dirty, 0, ncentroids * sizeof (int));
	memset (globaldirty, 0, ncentroids * sizeof (int));
}

int* kmeans(void) {
	int i, j, k;
	too_far = 0;
	has_changed = 0;

	if (!(map = calloc (npoints, sizeof (int))))
		exit(1);
	if (!(globalmap = calloc (npoints, sizeof (int))))
		exit(1);
	if (!(dirty = malloc (ncentroids * sizeof (int))))
		exit(1);
	if (!(centroids = malloc (ncentroids * sizeof (vector_t))))
		exit(1);
	if (!(globaldirty = malloc (ncentroids * sizeof (int))))
		exit(1);
	if (!(globalcentroids = malloc (ncentroids * sizeof (vector_t))))
		exit(1);

	for (i = 0; i < ncentroids; i++) {
		centroids[i] = malloc (sizeof (float) * dimension);
		globalcentroids[i] = malloc (sizeof (float) * dimension);
	}
	for (i = 0; i < npoints; i++)
		map[i] = -1;
	for (i = 0; i < ncentroids; i++) {
		dirty[i] = 1;
		globaldirty[i] = 1;
		j = randnum() % npoints;
		for (k = 0; k < dimension; k++)
			centroids[i][k] = data[j][k];
		map[j] = i;
	}
	/* Map unmapped data points. */
	for (i = 0; i < npoints; i++)
		if (map[i] < 0)
			map[i] = randnum () % ncentroids;

	do {              /* Cluster data. */
		populate();
		compute_centroids();
	} while (globalfar && has_changed);
	sync_map();

	for (i = 0; i < ncentroids; i++) {
		free(centroids[i]);
		free(globalcentroids[i]);
	}
	free(centroids);
	free(globalcentroids);
	free(dirty);
	free(globaldirty);

	return map;
}

int main (int argc, char **argv) {
	int i, j;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (argc != 6) {
		printf ("Usage: npoints dimension ncentroids mindistance seed\n");
		exit(1);
	}

	npoints = atoi(argv[1]);
	dimension = atoi(argv[2]);
	ncentroids = atoi(argv[3]);
	mindistance = atoi(argv[4]);
	seed = atoi(argv[5]);

	min = (rank*npoints)/size;
	max = ((rank + 1) * npoints)/size;

	// printf("%d - %d", min, max);

	srandnum(seed);

	if (!(data = malloc (npoints * sizeof(vector_t))))
		exit(1);

	for (i = 0; i < npoints; i++) {
		data[i] = malloc(sizeof(float) * dimension);
		for (j = 0; j < dimension; j++)
			data[i][j] = randnum() & 0xffff;
	}

	map = kmeans();

	if(rank == 0) {
		for (i = 0; i < ncentroids; i++) {
			printf ("\nPartition %d:\n", i);
			for (j = 0; j < npoints; j++)
				if (map[j] == i)
					printf("%d ", j);
		}
		printf("\n");
	}


	free(map);
	for (i = 0; i < npoints; i++)
		free(data[i]);
	free(data);

	MPI_Finalize();

	return (0);
}
