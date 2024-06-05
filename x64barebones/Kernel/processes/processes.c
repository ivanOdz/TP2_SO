#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <time.h>

#define DEFAULT_QTY_FDS	   3
#define STACK_DEFAULT_SIZE (1 << 12)

static uint16_t nextPid = 0;
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
	PCB *process = allocMemory(sizeof(PCB));
	if (!process) {
		return NULL;
	}
	process->stackBasePointer = (uint8_t *) allocMemory(STACK_DEFAULT_SIZE);
	if (!process->stackBasePointer) {
		free(process);
		return NULL;
	}
	process->stackBasePointer += STACK_DEFAULT_SIZE - 1; // stack works backwards
	int argc = 0;
	while (argv[argc++]) {
	}
	process->stackPointer = fabricateProcessStack(process->stackBasePointer, argc, argv, processMain);
	process->name = argv[0];
	process->pid = nextPid++;
	process->parentPid = getCurrentPID();
	process->status = READY;
	process->runMode = runMode;
	process->returnValue = 0;
	process->killed = FALSE;
	process->lastTickRun = get_ticks();
	process->blockedOn.waitPID = NULL;
	process->blockedOn.fd = 0;
	process->blockedOn.timer = 0;

	for (int i = 0; i < DEFAULT_QTY_FDS; i++) {
		process->fileDescriptors[i] = i;
	}
	process->fileDescriptorsInUse = DEFAULT_QTY_FDS;
	process->priority = 1;
	return process;
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
	process->stackPointer = NULL;
	schedyield();
}
PID_t waitPID(PID_t PID, ReturnStatus *wstatus) {
	PCB *process = getCurrentProcess();
	PCB *child = getProcess(PID);
	if (PID && child && child->parentPid == process->pid) {
		process->blockedOn.waitPID = wstatus;
		process->blockedOn.waitPID->pid = PID;
		process->status = BLOCKED;
		process->stackPointer = forceyield(stackSwitcharoo);
		process->blockedOn.waitPID = NULL;
		return wstatus->pid;
	}
}

void freeProcess(PCB *process) {
	free(process->stackBasePointer);
	removeProcess(process);
	free(process);
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
	PCB *process = getProcess(pid);
	if (process < 0 || priority < 0 || priority > 9) {
		return;
	}
	process->priority = priority;
}

/*
void setProcessState(uint16_t pid, ProcessStatus ps) {

}
*/

void blockProcess(uint16_t pid) {
	PCB *process = getProcess(pid);
	if (!process) {
		return;
	}
	if (process->status == BLOCKED && process->blockedOn.waitPID == NULL && process->blockedOn.fd == 0) {
		process->status = READY;
	}
	else if (process->status == READY) {
		process->status = BLOCKED;
	}
}