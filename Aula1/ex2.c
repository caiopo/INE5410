#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
		for (int i = 0; i < 200; ++i) {
			pid_t pid = fork();
			if(pid > 0)
				printf("Processo %d criou %d\n", getpid(), pid);
			else {
				printf("Processo filho. PID=%d\n", getpid());
				break;
			}
		}
		
		wait(NULL);
}

