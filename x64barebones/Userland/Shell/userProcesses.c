// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>

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
	if (argc > 1)
		offset = stringToInt(argv[1], strlen(argv[1]));
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
		printf("Son %s %2d:%2x:%2x (offset%s \b%d)\n", (time.hora != 1) ? "las" : "la", time.hora, time.min, time.seg,
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
		if (c)
			putchar(c);
	}
	putchar(c);
	exit(0);
}

void filter(int argc, char **argv) {
	char c;
	while ((c = getchar()) != EOF) {
		if (c)
			if (c != 'a' && c != 'A' && c != 'e' && c != 'E' && c != 'i' && c != 'I' &&
				c != 'o' && c != 'O' && c != 'u' && c != 'U')
				putchar(c);
	}
	putchar(c);
	exit(0);
}

void wc(int argc, char **argv) {
	char c;
	int lines = 0;
	while ((c = getchar()) != EOF) {
		if (c == '\n')
			lines++;
	}
	putchar(c);
	exit(lines);
}