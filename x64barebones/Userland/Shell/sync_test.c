// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <libc.h>
#include <tests.h>

#define DEFAULT_PROCESSES 10

static uint64_t shared = 0;

/*typedef struct Sem {
  int value;
} sem;
*/

void racyInc() {
	uint64_t aux = shared;
	yield(); // This makes the race condition highly probable
	aux++;
	shared = aux;
}

void semSlaveProcess(int argc, char **argv) {
	uint64_t cycles;
	int16_t idSemaphore;
	if (argc < 2) {
		exit(0);
	}
	cycles = stringToInt(argv[1], strlen(argv[1]));
	if (argv[2][0])
		idSemaphore = stringToInt(argv[2], strlen(argv[2]));
	else
		idSemaphore = -1;

	for (uint64_t cont = 0; cont < cycles; cont++) {
		if (idSemaphore >= 0) {
			sem_wait(idSemaphore);
			racyInc();
			sem_post(idSemaphore);
		}
		else {
			racyInc();
		}
	}
	exit(0);
}

void testSemaphores(int argc, char **argv) { // Cant incrementos, cant procesos, usa semaforos o no

	uint64_t cycles;
	uint16_t nProcesses = DEFAULT_PROCESSES;
	PID_t *processesPids;
	char *argvForProcesses[5];
	int16_t idSemaphore = -1;
	bool useSemaphore = FALSE;

	if (argc < 4) {
		printf("Argument missing?\n");
		exit(0);
	}
	cycles = stringToInt(argv[1], strlen(argv[1]));
	nProcesses = stringToInt(argv[2], strlen(argv[2]));
	useSemaphore = argv[3][0] == '1';

	if (cycles == 0) {
		cycles = 65535;
	}
	if (nProcesses == 0) {
		nProcesses = 2;
	}
	if (useSemaphore) {
		idSemaphore = sem_init(1);
		if (idSemaphore == -1) {
			exit(-1);
		}
	}

	processesPids = malloc(nProcesses * sizeof(PID_t));
	if (!processesPids)
		exit(-1);
	shared = 0;
	char semIDstr[4];
	if (idSemaphore >= 0)
		uintToBase(idSemaphore, semIDstr, 10);
	else
		semIDstr[0] = 0;

	argvForProcesses[0] = "test_sem_process";
	argvForProcesses[1] = argv[1];
	argvForProcesses[2] = semIDstr;
	for (PID_t cont = 0; cont < nProcesses; cont++) {
		processesPids[cont] = execv(semSlaveProcess, argvForProcesses, FOREGROUND);
	}
	printf("Huzzah!\n");
	ReturnStatus status;
	for (PID_t cont = 0; cont < nProcesses; cont++) {
		waitpid(0, &status);
	}

	if (useSemaphore) {
		sem_destroy(idSemaphore);
	}
	printf("VALUE: %lu\n", shared);
	exit(0);
}