#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
		for (int i = 0; i < 2; ++i) {
			pid_t pid = fork();
			if(pid > 0)
				printf("Processo %d criou %d\n", getpid(), pid);
			else {
				printf("Processo %d filho de %d\n", getpid(), getppid());
				i = 1;
				pid_t pid2 = fork();

				if(pid2 > 0) {
					printf("Processo %d criou %d\n", getpid(), pid2);
					pid_t pid3 = fork();

					if(pid3 > 0)
						printf("Processo %d criou %d\n", getpid(), pid3);
					else
						printf("Processo %d filho de %d\n", getpid(), getppid());
				} else 
					printf("Processo %d filho de %d\n", getpid(), getppid());

			}
		}
		
		wait(NULL);
}

