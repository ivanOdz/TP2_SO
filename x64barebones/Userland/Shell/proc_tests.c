// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <stdint.h>

#define PROCESSES_DEFAULT	 200
#define BIAS_DEFAULT		 50
#define BURN_IN_DEFAULT		 3
#define SLEEP_DEFAULT		 50
#define BUSY_DEFAULT		 500
#define BUSY_OFFSET			 100000
#define MAX_PRIORITY_DEFAULT 9
#define MIN_PRIORITY_DEFAULT 1
#define HELP_STRING_PROC	 "\
\nprocesses_test is a program designed to stress test the system scheduler and process management alghorithms by spawning a lot\n\
of processes and randomly blocking and killing them. This test can run many times in a row. You can also set a kill-block bias\
and a sleep time between operations. If one of these operations fails, you'll see a red warning. All arguments can be ommited.\n\
Arguments:\n\n\
-help\t\t\tdisplays this help message\n\n\
-processes x\t sets how many processes this test should create. Bear in mind that, while the kernel can spawn unlimited\n\
\t\t\t\t processes (as long as theres mem), it will become quite slow (range 1 - 999) (ex. -processes 200) (default %d)\n\n\
-bias x\t\t  sets the randomness bias from 1-99, where 1 means always kill and 99 means kill once every 100 times. This\n\
\t\t\t\t is useful to stress-test the blocking operations (ex. -bias 50 meaning half the time) (default %d)\n\n\
-sleep x\t\t sets the sleep interval in ms between block / kill attempts, mostly for output readability, otherwise it may\n\
\t\t\t\t go by too fast to read. Can be set to 0 to disable sleep altogether (ex. -sleep 500) (default %d)\n\n\
-burnin x\t\tsets how many times this test should loop (ex.- burnin 3) (default %d)\n\n\
Usage example:\n\t\ttest_processes -processes 100 -bias 20 -burnin 1 -sleep 100\n\n"

#define HELP_STRING_PRIO "\
\npriority_test is a test designed to prove that the scheduling alghorithm can effectively prioritize process execution\n\
by running many identical processes that will all run a simple yet long loop within, and count how many system ticks each\n\
one takes. This test does not throw errors (unless process creation fails). All arguments can be ommited.\n\
Arguments\n\n\
-help\t\t\tdisplays this help message\n\n\
-processes x\t sets how many processes this test should create. Bear in mind that, while the kernel can spawn unlimited\n\
\t\t\t\t processes (as long as theres mem), it will become quite slow (range 1 - 999) (ex. -processes 200) (default %d)\n\n\
-busytime x\t  sets the internal loop parameters for the processes to test. The higher this number is, the more the\n\
\t\t\t\t processes will run, yielding better results (range 1-10000) (ex. -busytime 1000) (default %d)\n\n\
-burnin x\t\tsets how many times this test should loop (ex.- burnin 3) (default %d)\n\n\
-minpriority x   sets the minimum priority to assign to the processes. The test will attempt to distribute the priorities\n\
\t\t\t\t uniformly between processes. (range 1-9) (ex. -minpriority 3) (default %d)\n\n\
-maxpriority x   sets the maximum priority to assign to the processes. The test will attempt to distribute the priorities\n\
\t\t\t\t uniformly between processes. (range 1-9) (ex. -maxpriority 7) (default %d)\n\n\
Usage example:\n\t\ttest_priorities -processes 100 -busytime 2000 - burnin 1 - minpriority 1 -maxpriority 9\n\n"

typedef struct ProcessTestType {
	PID_t PID;
	enum ProcessStatus { BLOCKED,
						 RUNNING,
						 KILLED } status;
} ProcessTestType;

typedef struct ProcessPriorityType {
	PID_t PID;
	uint8_t priority;
} ProcessPriorityType;

void processTestLoop(int argc, char **argv);
void processPriorityLoop(int argc, char **argv);

void test_processes(int argc, char **argv) {
	uint64_t maxProcesses = PROCESSES_DEFAULT;
	uint64_t bias = BIAS_DEFAULT;
	uint64_t sleepAmount = SLEEP_DEFAULT;
	uint64_t burnin = BURN_IN_DEFAULT;
	for (int arg = 1; arg < argc; arg++) {
		if (strcmp(argv[arg], "-help") == 0) {
			printf(HELP_STRING_PROC, PROCESSES_DEFAULT, BIAS_DEFAULT, SLEEP_DEFAULT, BURN_IN_DEFAULT);
			exit(0);
		}
		else if (strcmp(argv[arg], "-processes") == 0) {
			maxProcesses = argumentParse(arg++, argc, argv);
			if (!maxProcesses || maxProcesses > 999) {
				fprintf(STD_ERR, "INVALID -processes paramenter. Expected 1-999, got %s", argv[arg]);
				exit(1);
			}
		}
		else if (strcmp(argv[arg], "-bias") == 0) {
			bias = argumentParse(arg++, argc, argv);
			if (bias > 99) {
				fprintf(STD_ERR, "INVALID -bias paramenter. Expected 0-99, got %s", argv[arg]);
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
			printf(HELP_STRING_PROC, PROCESSES_DEFAULT, BIAS_DEFAULT, SLEEP_DEFAULT, BURN_IN_DEFAULT);
			exit(4);
		}
	}
	putchar('\e');
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
		printf("Instantiating processes (0 of %lu processes)", maxProcesses);
		int spawned;
		for (spawned = 0; spawned < maxProcesses; spawned++) {
			args[0] = "Test process";
			args[1] = NULL;
			PID_t newProcess = execv(processTestLoop, args, BACKGROUND);
			if (!newProcess) {
				fprintf(STD_ERR, "%cERROR CREATING PROCESS N#%d, WILL RETRY          \n", 0xD, spawned);
				processes[spawned].PID = 0;
				processes[spawned].status = KILLED;
				spawned--;
			}
			else {
				processes[spawned].PID = newProcess;
				processes[spawned].status = RUNNING;
			}
			printf("\rInstantiating processes (%d of %lu processes)", spawned + 1, maxProcesses);
			if (sleepAmount)
				sleep(sleepAmount);
		}
		printf("\nInstantiated %d processes\n", spawned);
		printf("\nKilling / blocking processes (0 processes alive, 0 processes blocked)");
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
					}
				}
				else {
					if (processes[index].status == BLOCKED) {
						blocked--;
						processes[index].status = RUNNING;
					}
					else {
						fprintf(STD_ERR, "%cERROR UNBLOCKING PROCESS %u\n", 0xD, processes[index].PID);
					}
				}
			}
			printf("\rKilling / blocking processes (%d processes alive, %d processes blocked)    ", spawned, blocked);
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

void test_priority(int argc, char **argv) {
	int maxProcesses = PROCESSES_DEFAULT;
	int busytime = BUSY_DEFAULT;
	int burnin = BURN_IN_DEFAULT;
	int minPriority = MIN_PRIORITY_DEFAULT;
	int maxPriority = MAX_PRIORITY_DEFAULT;
	for (int arg = 1; arg < argc; arg++) {
		if (strcmp(argv[arg], "-help") == 0) {
			printf(HELP_STRING_PRIO, PROCESSES_DEFAULT, BUSY_DEFAULT, BURN_IN_DEFAULT, MIN_PRIORITY_DEFAULT, MAX_PRIORITY_DEFAULT);
			exit(0);
		}
		else if (strcmp(argv[arg], "-processes") == 0) {
			maxProcesses = argumentParse(arg++, argc, argv);
			if (!maxProcesses || maxProcesses > 999) {
				fprintf(STD_ERR, "INVALID -processes paramenter. Expected 1-999, got %s\0", argv[arg]);
				exit(1);
			}
		}
		else if (strcmp(argv[arg], "-busytime") == 0) {
			busytime = argumentParse(arg++, argc, argv);
			if (busytime > 10000) {
				fprintf(STD_ERR, "INVALID -busytime paramenter. Expected 1-10000, got %s\0", argv[arg]);
				exit(2);
			}
		}
		else if (strcmp(argv[arg], "-burnin") == 0) {
			burnin = argumentParse(arg++, argc, argv);
			if (!burnin) {
				fprintf(STD_ERR, "INVALID -burnin paramenter. Expected 1+, got %s\0", argv[arg]);
				exit(3);
			}
		}
		else if (strcmp(argv[arg], "-minpriority") == 0) {
			minPriority = argumentParse(arg++, argc, argv);
			if (!minPriority) {
				fprintf(STD_ERR, "INVALID -minpriority paramenter. Expected 1-9, got %s\0", argv[arg]);
				exit(3);
			}
		}
		else if (strcmp(argv[arg], "-maxpriority") == 0) {
			maxPriority = argumentParse(arg++, argc, argv);
			if (!maxPriority) {
				fprintf(STD_ERR, "INVALID -maxpriority paramenter. Expected 1-9, got %s\0", argv[arg]);
				exit(3);
			}
		}
		else {
			fprintf(STD_ERR, "Invalid argument provided (got %s)\n", argv[arg]);
			printf(HELP_STRING_PRIO, PROCESSES_DEFAULT, BUSY_DEFAULT, BURN_IN_DEFAULT, MIN_PRIORITY_DEFAULT, MAX_PRIORITY_DEFAULT);
			exit(4);
		}
	}
	if (maxPriority < minPriority) {
		fprintf(STD_ERR, "Invalid argument combinations: maxPriority cannot be smaller than minPriority. It don't make sense!\n");
		printf(HELP_STRING_PRIO, PROCESSES_DEFAULT, BUSY_DEFAULT, BURN_IN_DEFAULT, MIN_PRIORITY_DEFAULT, MAX_PRIORITY_DEFAULT);
		exit(5);
	}
	putchar('\e');
	SyscallNice(getPID(), 9);
	printf("Processes: %d\tbusytime: %d\tmin priority: %d\tmax priority: %d\tburn-in: %d\n", maxProcesses, busytime, minPriority, maxPriority, burnin);
	ProcessPriorityType *processes = malloc(maxProcesses * burnin * sizeof(ProcessPriorityType));
	if (!processes)
		exit(5);

	char *args[3];
	char arg2buf[20];
	args[0] = "Test process";
	uintToBase(busytime * BUSY_OFFSET, arg2buf, 10);
	args[1] = arg2buf;
	args[2] = NULL;
	int perPriority[10] = {0};
	int times[10] = {0};

	putchar('\n');
	for (int spawned = 0, awaiting = 0; spawned < maxProcesses * burnin || awaiting > 0;) {
		printf("\rInstantiating %d processes (%d launched, %d live, %d returned)", maxProcesses * burnin, spawned, awaiting, spawned - awaiting);
		while (awaiting < maxProcesses && spawned < maxProcesses * burnin) {
			args[0] = "Test process";
			uintToBase(busytime * BUSY_OFFSET, arg2buf, 10);
			args[1] = arg2buf;
			args[2] = NULL;
			PID_t newProcess = execv(processPriorityLoop, args, BACKGROUND);
			if (!newProcess) {
				fprintf(STD_ERR, "%cERROR CREATING PROCESS N#%d, WILL RETRY          \n", 0xD, spawned);
				processes[spawned].PID = 0;
			}
			else {
				processes[spawned].PID = newProcess;
				processes[spawned].priority = spawned % (maxPriority + 1 - minPriority) + minPriority;
				SyscallNice(processes[spawned].PID, processes[spawned].priority);
				perPriority[processes[spawned].priority]++;
				spawned++;
				awaiting++;
			}
			printf("\rInstantiating %d processes (%d launched, %d live, %d returned)", maxProcesses * burnin, spawned, awaiting, spawned - awaiting);
		}
		ReturnStatus r;
		PID_t back = waitpid(0, &r);
		awaiting--;
		for (int i = 0; i < maxProcesses * burnin; i++) {
			if (processes[i].PID == back) {
				times[processes[i].priority] += r.returnValue;
				break;
			}
		}
	}
	puts("\n\n");

	for (int i = 1; i <= 9; i++) {
		if (perPriority[i])
			printf("Priority %d: %d ticks\n", i, times[i] / perPriority[i]);
	}
	puts("\n");
	exit(0);
}

void processPriorityLoop(int argc, char **argv) {
	uint64_t startTicks = SyscallGetTicks();
	long loopme = stringToInt(argv[1], strlen(argv[1]));
	for (long i = 0; i < loopme; i++) {
	}
	uint64_t ticksRunning = SyscallGetTicks() - startTicks;
	exit(ticksRunning);
}