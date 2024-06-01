#include <libc.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <memoryManager.h>


#define MAX_PROCESSES	   250
#define DEFAULT_QTY_FDS	   3
#define STACK_DEFAULT_SIZE (1 << 12)

static uint16_t nextPid = 1;

static PCB processes[MAX_PROCESSES] = {0};

static int16_t getNextPosition() {
	int16_t availablePos = 0;
	while (availablePos < MAX_PROCESSES && processes[availablePos].stackBasePointer != NULL) {
		availablePos++;
	}
	if (availablePos == MAX_PROCESSES) {
		return -1;
	}
	return availablePos;
}

int8_t createProcess(const char *name, uint8_t **argv, ProcessRunMode runMode, uint16_t parentPid) {
	int16_t pos = getNextPosition();
	if (pos < 0) {
		return -1;
	}
	processes[pos].pid = nextPid++;
	// processes[pos].parentPid = getCurrentProcessPid();
	processes[pos].stackBasePointer = (uint8_t *) allocMemory(STACK_DEFAULT_SIZE) + STACK_DEFAULT_SIZE;
	processes[pos].stackPointer = processes[pos].stackBasePointer;
	processes[pos].status = READY;
	processes[pos].argv = argv;
	processes[pos].runMode = runMode;
	processes[pos].returnValue = 0;

	for (int i = 0; i < DEFAULT_QTY_FDS; i++) {
		processes[pos].fileDescriptors[i] = i;
	}
	processes[pos].fileDescriptorsInUse = DEFAULT_QTY_FDS;
	processes[pos].priority = 1;

	// printf("LLego a crearse el proceso %s con modo %d y el padre es PID: %d\n", name, runMode, parentPid);
	return processes[pos].pid;
}

uint64_t ps() {
	printf("Dentro del ps\n");
	return 0;
}

/*
int8_t getProcessIndex(int16_t pid) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		if (processes[i].pid == pid) {
			return i;
		}
	}
	return -1;
}

void killProcess(uint16_t pid) {
	int8_t index = getProcessIndex(pid);
	if (index < 0) {
		return;
	}
	processes[index].status = STOPPED; // Para que no vuelva a ser elegido por el scheduler.
	processes[index].stackBase = NULL;
	processes[index].stackPointer = NULL;
}

void setProcessPriority(uint16_t pid, uint8_t priority) {
	int8_t index = getProcessIndex(pid);
	if (index < 0) {
		return;
	}
	processes[index].priority = priority;
}

void setProcessState(uint16_t pid, ProcessStatus ps) {

}
*/