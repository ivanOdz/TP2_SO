// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <libc.h>
#include <tests.h>

#define BOTH 2

#define PROCESSES_DEFAULT 10
#define CYCLES_DEFAULT	  20000
#define SEM_DEFAULT		  BOTH
#define HELP_STRING_PROC  "\
\nsync_test is a program designed to the system semaphore implementation by spawning processes that'll try to increment a shared\
varible in a very racey manner, and compare the result (with or without semaphores in place) with the expected result. Without\
semaphores enabled, the value will differ significantly, while with semaphores enabled, it should be spot-on.\n\
Arguments:\n\n\
-help\t\t\tdisplays this help message\n\n\
-processes x\t sets how many processes this test should create. Bear in mind that, while the kernel can spawn unlimited\n\
\t\t\t\t processes (as long as theres mem), it will become quite slow (range 1 - 999) (ex. -processes 50) (default %d)\n\n\
-cycles x\t\tSets how many increments each process should perform over the shared variable. Said increments will be \n\
\t\t\t\t explicitally racey, to show the effectiveness of semaphores (range 1-99999) (ex. -cycles 5000)(default %d)\n\n\
-usesemaphore x  toggles semaphore use for this test (values: yes, no, both). Setting this parameter to 'both' means it will\n\
\t\t\t\t perform both tests consecutively (without, then with) (ex. -usesemaphore no) (default 'both')\n\n\
Usage example:\n\t\tsync_test -processes 100 -cycles 2000 -usesemphores yes\n\n"

static uint64_t shared = 0;

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

	uint64_t cycles = CYCLES_DEFAULT;
	uint16_t nProcesses = PROCESSES_DEFAULT;
	char *argvForProcesses[5];
	int16_t idSemaphore = -1;
	bool useSemaphore = SEM_DEFAULT;

	for (int arg = 1; arg < argc; arg++) {
		if (strcmp(argv[arg], "-help") == 0) {
			printf(HELP_STRING_PROC, PROCESSES_DEFAULT, CYCLES_DEFAULT);
			exit(0);
		}
		else if (strcmp(argv[arg], "-processes") == 0) {
			nProcesses = argumentParse(arg++, argc, argv);
			if (!nProcesses || nProcesses > 999) {
				fprintf(STD_ERR, "INVALID -processes paramenter. Expected 1-999, got %s", argv[arg]);
				exit(1);
			}
		}
		else if (strcmp(argv[arg], "-cycles") == 0) {
			cycles = argumentParse(arg++, argc, argv);
			if (cycles > 99999) {
				fprintf(STD_ERR, "INVALID -cycles paramenter. Expected 0-99999, got %s", argv[arg]);
				exit(2);
			}
		}
		else if (strcmp(argv[arg], "-usesemaphore") == 0) {
			arg++;
			if (!strcmp(argv[arg], "no")) {
				useSemaphore = FALSE;
			}
			else if (!strcmp(argv[arg], "yes")) {
				useSemaphore = TRUE;
			}
			else if (!strcmp(argv[arg], "both")) {
				useSemaphore = BOTH;
			}
			else {
				fprintf(STD_ERR, "INVALID -usesemaphores paramenter. Expected 'yes', 'no' or 'both', got %s", argv[arg]);
				exit(3);
			}
		}
		else {
			fprintf(STD_ERR, "Invalid argument provided (got %s)\n", argv[arg]);
			printf(HELP_STRING_PROC, PROCESSES_DEFAULT, CYCLES_DEFAULT);
			exit(4);
		}
	}
	char countStr[6];
	uintToBase(cycles, countStr, 10);
	putchar('\e');
	do {
		printf("Running test for %d processes and %lu cycles with%s semaphores. Aiming for %lu\n", nProcesses, cycles, (useSemaphore) ? "" : "out", nProcesses * cycles);
		if (useSemaphore) {
			idSemaphore = sem_init(1);
			if (idSemaphore == -1) {
				fprintf(STD_ERR, "Error creating semaphore. This is pointless without it.\n");
				exit(5);
			}
		}

		shared = 0;
		char semIDstr[4];
		if (idSemaphore >= 0)
			uintToBase(idSemaphore, semIDstr, 10);
		else
			semIDstr[0] = 0;

		argvForProcesses[0] = "test_sem_process";
		argvForProcesses[1] = countStr;
		argvForProcesses[2] = semIDstr;
		int spawned;
		putchar('\n');
		for (spawned = 0; spawned < nProcesses; spawned++) {
			if (!execv(semSlaveProcess, argvForProcesses, BACKGROUND)) {
				fprintf(STD_ERR, "Error creating process #%u. This may cause inconsistencies with the test results, so it'll be aborted\n", spawned);
				exit(-1);
			}
			printf("\rInstantiating %u of %u processes...", spawned + 1, nProcesses);
		}
		putchar('\n');
		ReturnStatus status;
		for (int cont = 0; cont < nProcesses; cont++) {
			printf("\rWaiting for %d processes to return (shared counter thus far is %lu. Expected to be %d)", nProcesses - cont - 1, shared, nProcesses * cycles);
			waitpid(0, &status);
		}

		printf("\nFinal counter with%s semaphores is %lu, it was expected to be %lu. %s.\n\n", useSemaphore ? "" : "out", shared, nProcesses * cycles, (shared == nProcesses * cycles) ? "Hurray!" : "F");
		if (useSemaphore) {
			sem_destroy(idSemaphore);
		}
		if (useSemaphore == BOTH)
			useSemaphore--;
	} while (useSemaphore--);
	exit(0);
}