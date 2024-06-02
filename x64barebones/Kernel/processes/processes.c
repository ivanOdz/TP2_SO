#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <time.h>

#define DEFAULT_QTY_FDS	   3
#define STACK_DEFAULT_SIZE (1 << 12)

static int16_t getNextPosition();
static int8_t getProcessIndex(int16_t pid);

static uint16_t nextPid = 0;
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

// TODO copy argv to its own memory (on stack??)
PCB *createProcess(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode) {
	int16_t pos = getNextPosition();
	if (pos < 0) {
		return NULL;
	}
	processes[pos].stackBasePointer = (uint8_t *) allocMemory(STACK_DEFAULT_SIZE);
	if (!processes[pos].stackBasePointer)
		return NULL;
	processes[pos].stackBasePointer += STACK_DEFAULT_SIZE - 1; // stack works backwards
	int argc = 0;
	while (argv[argc++]) {
	}
	processes[pos].stackPointer = fabricateProcessStack(processes[pos].stackBasePointer, argc, argv, processMain);
	processes[pos].name = argv[0];
	processes[pos].pid = nextPid++;
	processes[pos].parentPid = getCurrentPID();
	processes[pos].status = READY;
	processes[pos].runMode = runMode;
	processes[pos].returnValue = 0;
	processes[pos].lastTickRun = get_ticks();

	for (int i = 0; i < DEFAULT_QTY_FDS; i++) {
		processes[pos].fileDescriptors[i] = i;
	}
	processes[pos].fileDescriptorsInUse = DEFAULT_QTY_FDS;
	processes[pos].priority = 1;
	return &processes[pos];
}

PID_t execute(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode) {
	PCB *process = createProcess(processMain, argv, runMode);
	if (!process)
		return 0;
	if (!addProcess(process))
		return 0;
	return process->pid;
}
void exitProcess(int returnValue) {
	PCB *process = getCurrentProcess();
	// removeProcess(process->pid);
	process->returnValue = returnValue;
	process->status = ZOMBIE;
	free(process->stackBasePointer);
	process->stackPointer = NULL;
	yield();
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

void blockProcess(uint16_t pid) {
	int8_t index = getProcessIndex(pid);
	if (index < 0 /*|| pid == getCurrentProcessPid()*/) {
		return;
	}
	if (processes[index].status == BLOCKED) {
		processes[index].status = READY;
	}
	else if (processes[index].status == READY) {
		processes[index].status = BLOCKED;
	}
}

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