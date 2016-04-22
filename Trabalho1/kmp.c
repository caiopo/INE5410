#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>


#define RANDNUM_W 521288629;
#define RANDNUM_Z 362436069;

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


int main(int argc, char const *argv[]) {
	if (argc != 6) {
		printf("Usage: npoints dimension ncentroids mindistance seed\n");
		exit (1);
	}

	npoints = atoi(argv[1]);
	dimension = atoi(argv[2]);
	ncentroids = atoi(argv[3]);
	mindistance = atoi(argv[4]);
	seed = atoi(argv[5]);

	srandnum(seed);


	return 0;
}