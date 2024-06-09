// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <stdint.h>

#define MAX_PROCESSES_DEFAULT 20
#define BIAS_DEFAULT		  50
#define BURN_IN_DEFAULT		  3
#define SLEEP_DEFAULT		  50
#define HELP_STRING_PROC	  "\
\ntest_processes is a program designed to stress test the system scheduler and process management alghorithms by spawning a lot\n\
of processes and randomly blocking and killing them. This test can run many times in a row. You can also set a kill-block bias\
and a sleep time between operations. If one of these operations fails, you'll see a red warning.\n\
Arguments:\n\n\
-help\t\t\tdisplays this help message\n\n\
-maxprocesses x  sets how many processes this test should create. Bear in mind that, while the kernel can spawn unlimited\n\
\t\t\t\t processes, it will become quite slow (range 1 - 500) (ex. -maxprocesses 200) (default %d)\n\n\
-bias x\t\t  sets the randomness bias from 1-99, where 1 means always kill and 99 means kill once every 100 times. This\n\
\t\t\t\t is useful to stress-test the blocking operations (ex. -bias 50 meaning half the time) (default %d)\n\n\
-sleep x\t\t sets the sleep interval in ms between block / kill attempts, mostly for output readability, otherwise it may\n\
\t\t\t\t go by too fast to read. Can be set to 0 to disable sleep altogether (ex. -sleep 500) (default %d)\n\n\
-burnin x\t\tsets how many times this test should loop (ex.- burnin 3)(default %d)\n\n\
Usage example:\n\t\ttest_processes - maxprocesses 100 - bias 20 - burnin 1\n\n "

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
	uint64_t sleepAmount = SLEEP_DEFAULT;
	uint64_t burnin = BURN_IN_DEFAULT;
	for (int arg = 1; arg < argc; arg++) {
		if (strcmp(argv[arg], "-help") == 0) {
			printf(HELP_STRING_PROC, MAX_PROCESSES_DEFAULT, BIAS_DEFAULT, SLEEP_DEFAULT, BURN_IN_DEFAULT);
			exit(0);
		}
		else if (strcmp(argv[arg], "-maxprocesses") == 0) {
			maxProcesses = argumentParse(arg++, argc, argv);
			if (!maxProcesses || maxProcesses > 999) {
				fprintf(STD_ERR, "INVALID -maxprocesses paramenter. Expected 1-999, got %s", argv[arg]);
				exit(1);
			}
		}
		else if (strcmp(argv[arg], "-bias") == 0) {
			bias = argumentParse(arg++, argc, argv);
			if (!bias || bias > 99) {
				fprintf(STD_ERR, "INVALID -bias paramenter. Expected 1-99, got %s", argv[arg]);
				exit(2);
			}
		}
		else if (strcmp(argv[arg], "-burnin") == 0) {
			burnin = argumentParse(arg++, argc, argv);
			if (!burnin) {
				fprintf(STD_ERR, "INVALID -burnin paramenter. Expected 1+, got %s", argv[arg]);
				exit(3);
			}
		}
		else if (strcmp(argv[arg], "-sleep") == 0) {
			sleepAmount = argumentParse(arg++, argc, argv);
		}
		else {
			fprintf(STD_ERR, "Invalid argument provided (got %s)\n", argv[arg]);
			printf(HELP_STRING_PROC, MAX_PROCESSES_DEFAULT, BIAS_DEFAULT, SLEEP_DEFAULT, BURN_IN_DEFAULT);
			exit(4);
		}
	}
	SyscallClear();
	SyscallNice(getPID(), 9);
	printf("Max processes: %lu\tbias: %lu\tsleep: %lums\tburn-in: %lu\n", maxProcesses, bias, sleepAmount, burnin);
	time_type time;
	SyscallGetRTC(&time);
	srand(time.hora << 16 | time.min << 8 | time.seg);
	ProcessTestType *processes = malloc(maxProcesses * sizeof(ProcessTestType));
	if (!processes)
		exit(5);
	char *args[2];
	while (burnin--) {
		printf("\nNew iteration (%lu left)\n", burnin);
		printf("Instantiating processes (0000 of %4lu processes)", maxProcesses);
		int spawned;
		for (spawned = 0; spawned < maxProcesses; spawned++) {
			args[0] = "Test process";
			args[1] = NULL;
			PID_t newProcess = execv(processTestLoop, args, BACKGROUND);
			if (!newProcess) {
				fprintf(STD_ERR, "%cERROR CREATING PROCESS N#%d, WILL RETRY          \n", 0xD, spawned);
				puts("Instantiating processes (0000 of 0000 processes)");
				processes[spawned].PID = 0;
				processes[spawned].status = KILLED;
				spawned--;
			}
			else {
				processes[spawned].PID = newProcess;
				processes[spawned].status = RUNNING;
			}
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%4d of %4lu processes)", spawned + 1, maxProcesses);
			if (sleepAmount)
				sleep(sleepAmount);
		}
		printf("\nInstantiated %d processes\n", spawned);
		printf("\nKilling / blocking processes (0000 processes alive, 0000 processes blocked)");
		int blocked = 0;
		while (spawned) {
			int index;
			do {
				index = randBetween(0, maxProcesses);
			} while (!processes[index].PID);
			if (randBetween(0, 100) >= bias) {
				PID_t newProcess = kill(processes[index].PID);

				if (!newProcess) {
					fprintf(STD_ERR, "%cERROR KILLING PROCESS %d    \n", 0xD, processes[index].PID);
					puts("Killing / blocking processes (0000 processes alive, 0000 processes blocked)");
				}
				else {
					if (processes[index].status == BLOCKED)
						blocked--;
					spawned--;
					processes[index].status = KILLED;
					processes[index].PID = 0;
					ReturnStatus f;
					waitpid(newProcess, &f);
				}
			}
			else {
				if (SyscallToggleBlockProcess(processes[index].PID)) {
					if (processes[index].status == RUNNING) {
						blocked++;
						processes[index].status = BLOCKED;
					}
					else {
						fprintf(STD_ERR, "%cERROR BLOCKING PROCESS %u\n", 0xD, processes[index].PID);
						puts("Killing / blocking processes (0000 processes alive, 0000 processes blocked)");
					}
				}
				else {
					if (processes[index].status == BLOCKED) {
						blocked--;
						processes[index].status = RUNNING;
					}
					else {
						fprintf(STD_ERR, "%cERROR UNBLOCKING PROCESS %u\n", 0xD, processes[index].PID);
						puts("Killing / blocking processes (0000 processes alive, 0000 processes blocked)");
					}
				}
			}
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%4d processes alive, %4d processes blocked)", spawned, blocked);
			if (sleepAmount)
				sleep(sleepAmount);
		}
		puts("\n\n");
	}
	exit(0);
}

void processTestLoop(int argc, char **argv) {
	SyscallNice(getPID(), 1);
	while (TRUE) {
		yield();
	}
	exit(0);
}