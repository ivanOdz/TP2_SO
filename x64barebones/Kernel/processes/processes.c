#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <time.h>

#define DEFAULT_QTY_FDS	   3
#define STACK_DEFAULT_SIZE (1 << 12)

static uint16_t nextPid = 1;
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

PCB *initProcess() {
	PCB *process = allocMemory(sizeof(PCB));
	if (!process) {
		return NULL;
	}
	process->stackBasePointer = getStackBase();
	if (!process->stackBasePointer) {
		free(process);
		return NULL;
	}
	process->name = "System Idle Process";
	process->pid = 0;
	process->parentPid = 0;
	process->status = RUNNING;
	process->runMode = BACKGROUND;
	process->returnValue = 0;
	process->killed = FALSE;
	process->lastTickRun = get_ticks();
	process->blockedOn.waitPID = NULL;
	process->blockedOn.fd = 0;
	process->blockedOn.timer = 0;
	process->blockedOn.manual = FALSE;
	process->fileDescriptorsInUse = 0;
	process->priority = 1;
	return process;
}

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
	process->stackBasePointer += STACK_DEFAULT_SIZE - sizeof(uint64_t); // stack works backwards
	process->stackPointer = process->stackBasePointer;
	int argc = 0;
	while (argv[argc]) {
		process->stackPointer -= strlen(argv[argc]) + 1;
		strcpy(process->stackPointer, argv[argc]);
		argv[argc++] = process->stackPointer;
	}
	process->stackPointer = fabricateProcessStack(process->stackPointer, argc, argv, processMain);
	process->name = argv[0];
	process->pid = nextPid++;
	process->parentPid = getCurrentPID();
	if (process->parentPid) {
		PCB *parent = getProcess(process->parentPid);
		if (parent->runMode == FOREGROUND)
			parent->runMode = RELEGATED;
	}
	process->status = READY;
	process->runMode = runMode;
	process->returnValue = 0;
	process->killed = FALSE;
	process->lastTickRun = get_ticks();
	process->blockedOn.waitPID = NULL;
	process->blockedOn.fd = 0;
	process->blockedOn.timer = 0;
	process->blockedOn.manual = FALSE;

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
	if (process->parentPid) {
		PCB *parent = getProcess(process->parentPid);
		if (parent->runMode == RELEGATED)
			parent->runMode = FOREGROUND;
	}
	schedyield();
}
PID_t waitPID(PID_t PID, ReturnStatus *wstatus) {
	PCB *process = getCurrentProcess();
	PCB *child = getProcess(PID);
	if (PID && child && child->parentPid == process->pid) {
		process->blockedOn.waitPID = wstatus;
		process->blockedOn.waitPID->pid = PID;
		process->status = BLOCKED;
		process->stackPointer = forceyield();
		process->blockedOn.waitPID = NULL;
		return wstatus->pid;
	}
	return 0;
}

void freeProcess(PCB *process) {
	free(process->stackBasePointer);
	removeProcess(process);
	free(process);
}

PID_t killProcess(PID_t PID) {
	PCB *process = getProcess(PID);
	if (!process)
		return 0;
	process->returnValue = -1;
	process->killed = TRUE;
	process->status = ZOMBIE;
	if (process == getCurrentProcess())
		schedyield();
	return PID;
}

void killRunningForegroundProcess() {
	PCB *process = getForegroundProcess();
	if (process || process->pid >= 2) {
		killProcess(process->pid);
	}
}

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
	if (process->status == BLOCKED) {
		if (process->blockedOn.waitPID == NULL && process->blockedOn.fd == 0)
			process->status = READY;
		process->blockedOn.manual = FALSE;
	}
	else if (process->status == READY) {
		process->status = BLOCKED;
		process->blockedOn.manual = TRUE;
	}
}