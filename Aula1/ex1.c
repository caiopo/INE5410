#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
	pid_t pid = fork();

	if(pid >= 0)
		printf("Novo processo criado!\n");
}