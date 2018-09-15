#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
	FILE* fp = fopen("/sys/tasks", "r");
	if(!fp) {
		perror("Could not read tasks");
		exit(EXIT_FAILURE);
	}

	char* data = malloc(1024);
	fgets(data, 1024, fp);


	printf("%-4s %-10s %-4s %-15s\n", "PID", "State", "PPID", "Name");
	printf("================================\n");

	while(true) {
		if(feof(fp)) {
			return EXIT_SUCCESS;
		}

		uint32_t pid;
		uint32_t ppid;
		char cstate;
		char name[300];
		uint32_t entry;
		uint32_t sbrk;
		uint32_t stack;

		if(fscanf(fp, "%d %d %c %s 0x%x 0x%x 0x%x\n", &pid, &ppid, &cstate, name, &entry, &sbrk, &stack) != 7) {
			fprintf(stderr, "Matching error.\n");
		}

		char* state = "Unknown";
		switch(cstate) {
			case 'K': state = "Killed"; break;
			case 'T': state = "Terminated"; break;
			case 'B': state = "Blocking"; break;
			case 'S': state = "Stopped"; break;
			case 'R': state = "Running"; break;
			case 'W': state = "Waiting"; break;
			case 'C': state = "Syscall"; break;
		}

		printf("%-4d %-10s %-4d %-15s\n", pid, state, ppid, name);
	}

	exit(EXIT_SUCCESS);
}