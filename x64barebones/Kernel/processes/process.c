#include "../include/process.h"
#include <libc.h>
#include <stdint.h>

#define MAX_PROCESSES	250
#define DEFAULT_QTY_FDS 3

static uint16_t nextPid = 1;

static PCB processes[MAX_PROCESSES] = {0};

static int16_t getNextPosition() {
	int16_t availablePos = 0;
	while (availablePos < MAX_PROCESSES && processes[availablePos].stackBase != NULL){
        availablePos++;
    }
    if(availablePos == MAX_PROCESSES){
        return -1;
    }
	return availablePos;
}

int8_t createProcess(const char *name, const char **argv, ProcessRunMode runMode, uint16_t parentPid) {
	int16_t pos = getNextPosition();
    if(pos < 0) {
        return -1;
    }
	processes[pos].pid = nextPid++;
	processes[pos].parentPid = parentPid;
	processes[pos].stackBase = getStackBase();
	processes[pos].stackPointer = processes[pos].stackBase;
	processes[pos].status = READY;
	processes[pos].argv = argv;
	processes[pos].runMode = runMode;
	processes[pos].returnValue = 0;

	for (int i = 0; i < DEFAULT_QTY_FDS; i++) {
		processes[pos].fileDescriptors[i] = i;
	}
	processes[pos].fileDescriptorsInUse = DEFAULT_QTY_FDS;
	processes[pos].priority = 1;
    return processes[pos].pid;
}


