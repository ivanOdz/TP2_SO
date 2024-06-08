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

void time(int argc, char **argv) {
	time_t time;
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