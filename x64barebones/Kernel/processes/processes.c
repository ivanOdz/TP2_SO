#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>

#define MAX_PROCESSES	   250
#define DEFAULT_QTY_FDS	   3
#define STACK_DEFAULT_SIZE (1 << 12)

static int16_t getNextPosition();
static int8_t getProcessIndex(int16_t pid);

static uint16_t nextPid = 1;
static PCB processes[MAX_PROCESSES] = {0};
/*static PCB processes[MAX_PROCESSES] = {
	{
		.pid = 1,
		.parentPid = 0,
		.stackBasePointer = 0x400000,
		.stackPointer = 0x500000,
		.name = (uint8_t*)"Process1",
		.argv = NULL,
		.runMode = FOREGROUND,
		.returnValue = 0,
		.fileDescriptors = {0},
		.fileDescriptorsInUse = 0,
		.priority = 5,
		.status = READY
	},
	{
		.pid = 2,
		.parentPid = 0,
		.stackBasePointer = 0x300000,
		.stackPointer = 0x200000,
		.name = (uint8_t*)"Process2",
		.argv = NULL,
		.runMode = BACKGROUND,
		.returnValue = 0,
		.fileDescriptors = {0},
		.fileDescriptorsInUse = 0,
		.priority = 10,
		.status = RUNNING
	},
	{
		.pid = 3,
		.parentPid = 1,
		.stackBasePointer = 0x100000,
		.stackPointer = 0x200000,
		.name = (uint8_t*)"Process3",
		.argv = NULL,
		.runMode = FOREGROUND,
		.returnValue = -1,
		.fileDescriptors = {0},
		.fileDescriptorsInUse = 0,
		.priority = 1,
		.status = BLOCKED
	}
};*/
static uint16_t numberOfProcesses = 0;

int8_t createProcess(uint8_t *name, ProcessRunMode runMode) {
	int16_t pos = getNextPosition();
	if (pos < 0) {
		return -1;
	}
	processes[pos].name = name;
	processes[pos].pid = nextPid++;
	// processes[pos].parentPid = getCurrentProcessPid();
	processes[pos].stackBasePointer = (uint8_t *) allocMemory(STACK_DEFAULT_SIZE) + STACK_DEFAULT_SIZE;
	processes[pos].stackPointer = processes[pos].stackBasePointer;
	processes[pos].status = READY;
	//processes[pos].argv = argv;
	processes[pos].runMode = runMode;
	processes[pos].returnValue = 0;

	for (int i = 0; i < DEFAULT_QTY_FDS; i++) {
		processes[pos].fileDescriptors[i] = i;
	}
	processes[pos].fileDescriptorsInUse = DEFAULT_QTY_FDS;
	processes[pos].priority = 1;
	numberOfProcesses++;
	return processes[pos].pid;
}
/*
int8_t finishProcess() {
	int8_t index = getCurrentIndex();
	freeMemory(processes[index].stackBasePointer);
	processes[index].status = ZOMBIE;
	processes[index].stackBasePointer = 0;
	processes[index].stackPointer = 0;
}

int8_t killProcess(uint16_t pid) {
	int8_t index = getProcessIndex(pid);
	if (index < 0) {
		return;
	}
	freeMemory(processes[index].stackBasePointer);
	processes[index].status = ZOMBIE;
	processes[index].stackBasePointer = 0;
	processes[index].stackPointer = 0;
}*/

uint64_t ps() {
	int16_t i = 0, processesFound = 0;
	char *status;
	printf("NOMBRE\t\t PID\tPID DEL PADRE\tMODO\tSTACK BASE POINTER\tSTACK POINTER\tESTADO\tPRIORIDAD\n");
	printf("=========================================================================================================\n");
	while (i < MAX_PROCESSES && processesFound < numberOfProcesses) {
		if(processes[i].stackBasePointer != NULL){
		switch (processes[i].status) {
			case 0:
				status = "BL";
				break;
			case 1:
				status = "RD";
				break;
			case 2:
				status = "RN";
				break;
			default:
				status = "ZB";
				break;
		}
		printf("%s\t\t%d\t\t %d\t\t\t  %s\t\t\t%x\t\t\t%x\t\t  %s\t\t  %d\t\t\n", processes[i].name, processes[i].pid, processes[i].parentPid, ((processes[i].runMode == 0) ? "F" : "B"),
			   processes[i].stackBasePointer, processes[i].stackPointer, status, processes[i].priority);
			   processesFound++;
		}
		i++;	
	}
	return 0;
}

void setProcessPriority(uint16_t pid, int8_t priority) {
	int8_t index = getProcessIndex(pid);
	if (index < 0 || priority < 0 || priority > 31) {
		return;
	}
	processes[index].priority = priority;
}


/*
void setProcessState(uint16_t pid, ProcessStatus ps) {

}
*/

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

static int8_t getProcessIndex(int16_t pid) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		if (processes[i].pid == pid) {
			return i;
		}
	}
	return -1;
}