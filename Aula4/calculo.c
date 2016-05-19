#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SIZE 1000000

double c[SIZE];

int main() {
	int i;

	for (i = 0; i < SIZE; i++) {
		c[i] = sqrt(i * 32) + sqrt(i * 16 + i * 8) + sqrt(i * 4 + i * 2 + i);
		c[i] -= sqrt(i * 32 * i * 16 + i * 4 + i * 2 + i);
		c[i] += pow(i * 32, 8) + pow(i * 16, 12);
	}

	return 0;
}