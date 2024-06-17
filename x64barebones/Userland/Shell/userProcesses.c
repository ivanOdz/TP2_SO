// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <userProcesses.h>

#define MAX_PHYLOS	   100
#define INITIAL_PHYLOS 5
#define PHYLO_NAMES	   50
static const char *philosopherNames[PHYLO_NAMES] = {
	"Aristotle",
	"Plato",
	"Socrates",
	"Diogenes",
	"Epicurus",
	"Confucius",
	"Immanuel Kant",
	"Simone de Beauvoir",
	"Jean-Paul Sartre",
	"Nietzsche",
	"Musashi Miyamoto",
	"Sun Tzu",
	"Hannah Arendt",
	"Hypatia of Alexandria",
	"Judith Butler",
	"Bell Hooks",
	"Voltaire",
	"Karl Marx",
	"Jean-Jacques Rousseau",
	"David Hume",
	"Niccolo Machiavelli",
	"John Dewey",
	"Rene Descartes",
	"Karl Popper",
	"Cornel West",
	"Angela Davis",
	"Dante",
	"Blaise Pascal",
	"John Locke",
	"Mary Wollstonecraft",
	"Ayn Rand",
	"Parmenides",
	"Mozi",
	"Avicenna",
	"Thomas Hobbes",
	"Adam Smith",
	"Leo Tolstoy",
	"Michel Foucault",
	"Dong Zhongshu",
	"Mencius",
	"Ramanuja",
	"Noam Chomsky",
	"Daniel Dennett",
	"Jennifer Saul",
	"Sally Haslanger",
	"Linda Martin Alcoff",
	"Kitaro Nishida",
	"Fukuzawa Yukichi",
	"Masanobu Fukuoka",
	"Daisaku Ikeda",
};

static int *phyloSemaphores;
static int printMutex;
static int editMutex;
static char *phyloStatus;

void ps(int argc, char **argv) {
	ProcessInfo *info = SyscallProcessInfo();
	printf("NAME\t\t\t\t PID   PARENT  MODE\t\tSTACK BASE\tSTACK POINTER\t STATUS\tPRIORITY\n");
	printf("==================================================================================================\n");
	while (info != NULL) {
		printf("%-20s %5d %5d   %-10s  0x%8lx\t0x%8lx\t\t%-9s %d\n", info->name, info->pid, info->parent_PID, (info->runMode == 'F') ? "Foreground" : ((info->runMode == 'B') ? "Background" : "Relegated"), (uint64_t) info->stackBasePointer, (uint64_t) info->stackPointer, info->processStatus, info->priority);
		ProcessInfo *temp = info;
		info = info->nextProcessInfo;
		free(temp);
	}
	exit(0);
}

void lsof(int argc, char **argv) {
	FdInfo *info = SyscallFdInfo(stringToInt(argv[1], strlen(argv[1])));
	printf("PROCESS NAME\t\tPID   FD  MODE\tPIPE NAME\n");
	printf("=============================================================\n");
	while (info != NULL) {
		printf("%-20s %u %5u   %-10c  %-20s\n", info->processName, info->pid, info->fd, ((info->mode == 0) ? 'R' : 'W'), info->pipeName);
		FdInfo *temp = info;
		info = info->nextFdInfo;
		free(temp);
	}
	exit(0);
}

void time(int argc, char **argv) {
	time_type time;
	int8_t offset = 0;
	if (argc > 1) {
		offset = stringToInt(argv[1], strlen(argv[1]));
	}
	if (offset > 12 || offset < -12) {
		fprintf(STD_ERR, "invalid offset\n");
		offset = 0;
	}
	if (SyscallGetRTC(&time)) {
		time.hora -= 6 * (time.hora >> 4);
		if (offset != 0) {
			time.hora += 24;
			time.hora += offset;
			time.hora %= 24;
		}
		printf("It's %2d:%2x:%2x (offset%s \b%d)\n", time.hora, time.min, time.seg,
			   (offset >= 0) ? "+" : "-", (uint8_t) ((offset >= 0) ? offset : -offset));
	}
	exit(0);
}

void nice(int argc, char **argv) {
	if (argc != 3) {
		fprintf(STD_ERR, "Wrong amount of arguments...\n");
		exit(1);
	}
	int16_t pid = stringToInt(argv[1], strlen(argv[1]));
	int8_t newPriority = stringToInt(argv[2], strlen(argv[2]));
	if (newPriority < 0 || newPriority > 31) {
		fprintf(STD_ERR, "Priority must be between 0 and 31...\n");
		exit(1);
	}
	SyscallNice(pid, newPriority);
	exit(0);
}

void block(int argc, char **argv) {
	if (argc != 2) {
		fprintf(STD_ERR, "Wrong amount of arguments...\n");
		exit(1);
	}
	int16_t pid = stringToInt(argv[1], strlen(argv[1]));
	SyscallToggleBlockProcess(pid);
	exit(0);
}

void mem(int argc, char **argv) {
	MemoryInfo mminfoo;
	memoryManagerStats(&mminfoo);
	printf("Total Memory: 0x%8lx\tFree Memory: 0x%8lx\tOcupied Memory: 0x%8lx\nFragmented Memory: 0x%8lx\tMax Frag Block Size:%8lx\tMin Frag Block Size:%8lx\tAssigned Nodes:%4lu\nStart address: 0x%8lx\tEnd address: 0x%8lx\tMM Type: %c\n", mminfoo.totalMemory, mminfoo.freeMemory, mminfoo.occupiedMemory, mminfoo.fragmentedMemory, mminfoo.maxFragmentedSize, mminfoo.minFragmentedSize, mminfoo.assignedNodes, mminfoo.startAddress, mminfoo.endAddress, mminfoo.mmType);
	exit(0);
}

void greeting(int argc, char **argv) {
	int sleepTime = 1000;
	if (argc > 1) {
		sleepTime = stringToInt(argv[1], strlen(argv[1])) * 1000;
		if (!sleepTime) {
			fprintf(STD_ERR, "Please provide a valid sleep time or use the default\n");
			exit(-1);
		}
	}
	PID_t myPID = getPID();
	while (TRUE) {
		printf("\nHello! I'm PID %u, and the machine has been up for %lu seconds, yay!\0", myPID, SyscallGetTicks() / HZ);
		sleep(sleepTime);
	}
	exit(0);
}

void cat(int argc, char **argv) {
	char c;
	while ((c = getchar()) != EOF) {
		if (c) {
			putchar(c);
		}
	}
	putchar(c);
	exit(0);
}

void filter(int argc, char **argv) {
	char c;
	while ((c = getchar()) != EOF) {
		if (c) {
			if (c != 'a' && c != 'A' && c != 'e' && c != 'E' && c != 'i' && c != 'I' && c != 'o' && c != 'O' && c != 'u' && c != 'U') {
				putchar(c);
			}
		}
	}
	putchar(c);
	exit(0);
}

void wc(int argc, char **argv) {
	char c;
	int lines = 0;
	while ((c = getchar()) != EOF) {
		if (c == '\n') {
			lines++;
		}
	}
	putchar(c);
	exit(lines);
}

void phyloProcess(int argc, char **argv) {
	int phyloNumber = stringToInt(argv[1], strlen(argv[1]));
	while (phyloSemaphores[phyloNumber] != -1) {
		sleep(randBetween(1, 5000));
		sem_wait(editMutex);
		if (phyloSemaphores[phyloNumber] == -1) {
			sem_post(editMutex);
			exit(0);
		}
		sem_wait(phyloSemaphores[phyloNumber]);
		sem_wait((phyloSemaphores[phyloNumber + 1] == -1) ? phyloSemaphores[0] : phyloSemaphores[phyloNumber + 1]);
		sem_post(editMutex);
		phyloStatus[phyloNumber] = 'E';
		sem_post(printMutex);
		sleep(randBetween(1, 5000));
		sem_post(phyloSemaphores[phyloNumber]);
		sem_post((phyloSemaphores[phyloNumber + 1] == -1) ? phyloSemaphores[0] : phyloSemaphores[phyloNumber + 1]);
		phyloStatus[phyloNumber] = '.';
		sleep(5000);
	}
	exit(0);
}

void phylo(int argc, char **argv) {
	int initPhylos = INITIAL_PHYLOS;
	if (argc > 1) {
		initPhylos = stringToInt(argv[1], strlen(argv[1]));
		if (initPhylos < 2) {
			initPhylos = 2;
		}
	}
	time_type time;
	SyscallGetRTC(&time);
	srand(time.hora << 12 | time.min << 6 | time.seg);
	phyloSemaphores = malloc(MAX_PHYLOS * sizeof(int));
	phyloStatus = malloc(MAX_PHYLOS * sizeof(char));
	printMutex = sem_init(0);
	if (printMutex == -1) {
		exit(1);
	}
	editMutex = sem_init(initPhylos);
	if (editMutex == -1) {
		exit(1);
	}
	ReturnStatus wstatus;
	char *phyloArgv[3];
	char phyloNumber[4];
	for (int i = 0; i < initPhylos; i++) {
		phyloSemaphores[i] = sem_init(1);
		phyloStatus[i] = '.';
		if (phyloSemaphores[i] < 0) {
			exit(1);
		}
	}
	for (int i = initPhylos; i < MAX_PHYLOS; i++) {
		phyloStatus[i] = '0';
		phyloSemaphores[i] = -1;
	}
	for (int i = 0; i < initPhylos; i++) {
		phyloArgv[0] = "Glutton philosopher";
		phyloArgv[1] = phyloNumber;
		phyloArgv[2] = NULL;
		uintToBase(i, phyloNumber, 10);
		execv(phyloProcess, phyloArgv, BACKGROUND);
	}
	char c;
	while (TRUE) {
		c = tryGetChar(STD_IN);
		if (c == 'q' || c == 'Q' || c == EOF) {
			for (int i = 0; i < MAX_PHYLOS; i++) {
				if (phyloSemaphores[i + 1] == -1) {
					printf("See you next dinner!\n");
					for (int j = 0; j <= i; j++) {
						sem_wait(editMutex); // Blockea todos los filosofos de comer para que podamos manipular los semaforos sin romper nada
					}
					for (int j = 0; j < i; j++) {
						sem_destroy(phyloSemaphores[j]);
						phyloSemaphores[j] = -1;
					}
					for (int j = 0; j <= i; j++) {
						sem_post(editMutex);
					}
					printf("Diogenes doesn't wanna leave again. Please wait while we evict him... (this may take a moment while we call the cops on him)\n");
					for (int j = 0; j < i; j++) {
						waitpid(0, &wstatus);
					}
					free(phyloSemaphores);
					free(phyloStatus);
					sem_destroy(editMutex);
					sem_destroy(printMutex);
					printf("Done! Now you leave as well or so help me.\n");
					break;
				}
			}
			exit(0);
		}
		if (c == 'a' || c == 'A') {
			for (int i = 0; i < MAX_PHYLOS; i++) {
				if (phyloSemaphores[i] == -1) {
					printf("Setting up table for another philosopher, please wait...");

					for (int j = 0; j < i; j++) {
						sem_wait(editMutex); // Blockea todos los filosofos de comer para que podamos manipular los semaforos sin romper nada
					}
					sem_wait(phyloSemaphores[i - 1]);
					sem_post(phyloSemaphores[i - 1]);
					phyloSemaphores[i] = sem_init(1);
					phyloStatus[i] = '.';
					if (phyloSemaphores[i] < 0) {
						exit(1);
					}
					phyloArgv[0] = "Glutton phylosopher";
					phyloArgv[1] = phyloNumber;
					phyloArgv[2] = NULL;
					uintToBase(i, phyloNumber, 10);
					execv(phyloProcess, phyloArgv, BACKGROUND);
					sem_destroy(printMutex);
					printMutex = sem_init(0);
					for (int j = 0; j <= i; j++) {
						sem_post(editMutex);
					}
					printf(" %s has joined the chat!\n", philosopherNames[randBetween(0, PHYLO_NAMES)]);
					break;
				}
				if (i + 1 == MAX_PHYLOS) {
					exit(2);
				}
			}
		}
		if (c == 'r' || c == 'R') {
			for (int i = 0; i < MAX_PHYLOS; i++) {
				if (phyloSemaphores[i + 1] == -1) {
					printf("Setting up table for one less philosopher, please wait...");

					for (int j = 0; j <= i; j++) {
						sem_wait(editMutex); // Blockea todos los filosofos de comer para que podamos manipular los semaforos sin romper nada
					}
					if (i == 1) {
						sem_destroy(phyloSemaphores[1]);
						phyloSemaphores[1] = -1;
						sem_destroy(phyloSemaphores[0]);
						phyloSemaphores[0] = -1;
						printf("\nThere are not enough philosophers left for an intresting evening. See you next dinner!\n");
						sem_post(editMutex);
						sem_post(editMutex);
						waitpid(0, &wstatus);
						waitpid(0, &wstatus);
						free(phyloSemaphores);
						free(phyloStatus);
						sem_destroy(editMutex);
						sem_destroy(printMutex);
						exit(0);
					}
					sem_wait(phyloSemaphores[i - 1]);
					sem_post(phyloSemaphores[i - 1]);
					sem_destroy(phyloSemaphores[i]);
					phyloSemaphores[i] = -1;
					phyloStatus[i] = 0;
					sem_destroy(printMutex);
					printMutex = sem_init(0);
					for (int j = 0; j < i; j++) {
						sem_post(editMutex);
					}
					waitpid(0, &wstatus);
					printf(" %s has left the chat!\n", philosopherNames[randBetween(0, PHYLO_NAMES)]);
					break;
				}
				if (i + 1 == MAX_PHYLOS) {
					exit(2);
				}
			}
		}
		sem_wait(printMutex);
		for (int i = 0; phyloSemaphores[i] != -1; i++) {
			if (phyloStatus[i]) {
				printf("%c ", phyloStatus[i]);
			}
		}
		putchar('\n');
	}
}
