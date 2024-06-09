// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <stdint.h>

#define MAX_PROCESSES_DEFAULT 20
#define BIAS_DEFAULT		  50
#define BURN_IN_DEFAULT		  3
#define HELP_STRING_PROC	  "\
\ntest_processes is a program designed to stress test the system scheduler and process management alghorithms by spawning a lot\n\
of processes and randomly blocking/unblocking and killing them. If one of these operations fails, you'll see a red warning.\n\
Arguments:\n\n\
-help\t\t\tdisplays this help message\n\n\
-maxprocesses x  sets how many processes this test should create. Bear in mind that, while the kernel can spawn unlimited\n\
\t\t\t\t processes, it will become quite slow (ex. -maxprocesses 200) (default %d)\n\n\
-bias x\t\t  sets the randomness bias from 0-100, where 0 means always kill and 100 means kill once every 100 times. This\n\
\t\t\t\t is useful to stress-test the blocking operations (ex. -bias 50 meaning half the time) (default %d)\n\n\
-burnin x\t\tsets how many times this test should loop (ex. -burnin 3) (default %d)\n\n\
Usage example:\n\t\ttest_processes -maxprocesses 100 -bias 20 -burnin 1\n\n"

typedef struct ProcessTestType {
	PID_t PID;
	enum ProcessStatus { BLOCKED,
						 RUNNING,
						 KILLED } status;
} ProcessTestType;

void processTestLoop(int argc, char **argv);

void test_processes(int argc, char **argv) {
	uint64_t maxProcesses = MAX_PROCESSES_DEFAULT;
	uint64_t bias = BIAS_DEFAULT;
	uint64_t burnin = BURN_IN_DEFAULT;
	for (int arg = 1; arg < argc; arg++) {
		if (strcmp(argv[arg], "-help") == 0) {
			printf(HELP_STRING_PROC, MAX_PROCESSES_DEFAULT, BIAS_DEFAULT, BURN_IN_DEFAULT);
			exit(0);
		}
		else if (strcmp(argv[arg], "-maxprocesses") == 0) {
			maxProcesses = argumentParse(arg++, argc, argv);
			if (!maxProcesses)
				exit(1);
		}
		else if (strcmp(argv[arg], "-bias") == 0) {
			bias = argumentParse(arg++, argc, argv);
			if (!bias)
				exit(1);
		}
		else if (strcmp(argv[arg], "-burnin") == 0) {
			burnin = argumentParse(arg++, argc, argv);
			if (!burnin)
				exit(1);
		}
		else {
			fprintf(STD_ERR, "Invalid argument provided (got %s)\n", argv[arg]);
			printf(HELP_STRING_PROC, MAX_PROCESSES_DEFAULT, BIAS_DEFAULT, BURN_IN_DEFAULT);
			exit(1);
		}
	}
	SyscallClear();
	SyscallNice(getPID(), 9);

	time_type time;
	SyscallGetRTC(&time);
	srand(time.hora << 16 | time.min << 8 | time.seg);
	ProcessTestType *processes = malloc(maxProcesses * sizeof(ProcessTestType));
	if (!processes)
		exit(1);
	char *args[2];
	printf("Instantiating processes (0000 of %4lu processes)", maxProcesses);
	int spawned;
	for (spawned = 0; spawned < maxProcesses; spawned++) {
		args[0] = "Test process";
		args[1] = NULL;
		PID_t newProcess = execv(processTestLoop, args, BACKGROUND);
		if (!newProcess) {
			fprintf(STD_ERR, "\nError creating process N#%d\n", spawned);
			puts("Instantiating processes (0000 of 0000 processes)");
			processes[spawned].PID = 0;
			processes[spawned].status = KILLED;
		}
		else {
			processes[spawned].PID = newProcess;
			processes[spawned].status = RUNNING;
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%4d of %4lu processes)", spawned + 1, maxProcesses);
		yield();
	}
	printf("\nInstantiated %d processes\n", spawned);
	printf("\n\nKilling / blocking processes (0000 of %4lu processes alive)", spawned);
	while (spawned) {
		int index;
		do {
			index = randBetween(0, maxProcesses);
		} while (!processes[index].PID);
		if (randBetween(0, 101) >= bias) {
			PID_t newProcess = kill(processes[spawned].PID);
			spawned--;
			processes[index].PID = 0;
			processes[index].status = KILLED;
			if (!newProcess) {
				fprintf(STD_ERR, "\nError killing process N#%d (PID %d)\n", index, processes[index].PID);
				puts("Killing processes (0000 of 0000 processes alive)");
			}
			else {
				ReturnStatus f;
				waitpid(newProcess, &f);
			}
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%4d of %4lu processes alive)", spawned + 1, maxProcesses);
			yield();
		}
		else {
			processes[index].status = (SyscallToggleBlockProcess(processes[index].PID)) ? BLOCKED : RUNNING;
		}
	}
	puts("\n\n");
	exit(0);
}

void processTestLoop(int argc, char **argv) {
	SyscallNice(getPID(), 1);
	while (TRUE) {
		yield();
	}
	exit(0);
}