// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <time.h>

#define DEFAULT_QTY_FDS	   3
#define STACK_DEFAULT_SIZE (1 << 12)

static uint16_t nextPid = 1;

PCB *initProcess(void *kernelStack) {
	PCB *process = allocMemory(sizeof(PCB));
	if (!process) {
		return NULL;
	}
	process->stackBasePointer = kernelStack;
	if (!process->stackBasePointer) {
		freeMemory(process);
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
	// process->fileDescriptorsInUse = 0;
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
		freeMemory(process);
		return NULL;
	}
	process->stackBasePointer += STACK_DEFAULT_SIZE - sizeof(uint64_t); // stack works backwards
	process->stackPointer = process->stackBasePointer;
	int argc = 0;
	while (argv[argc]) {
		process->stackPointer -= strlen(argv[argc]) + 1;
		strcpy((char *) process->stackPointer, argv[argc]);
		argv[argc++] = (char *) process->stackPointer;
	}
	process->stackPointer = fabricateProcessStack(process->stackPointer, argc, argv, processMain);
	process->name = argv[0];
	process->pid = nextPid++;
	process->parentPid = getCurrentPID();
	process->status = READY;
	process->runMode = runMode;
	process->priority = 5;

	if (process->parentPid && process->runMode == FOREGROUND) {
		PCB *parent = getProcess(process->parentPid);
		if (parent && parent->runMode == FOREGROUND)
			parent->runMode = RELEGATED;
	}

	process->returnValue = 0;
	process->killed = FALSE;
	process->lastTickRun = get_ticks();
	process->blockedOn.waitPID = NULL;
	process->blockedOn.fd = 0;
	process->blockedOn.timer = 0;
	process->blockedOn.manual = FALSE;

	/*for (int i = 0; i < DEFAULT_QTY_FDS; i++) {
		//process->fileDescriptors[i] = i;
	}
	process->fileDescriptorsInUse = DEFAULT_QTY_FDS;

	*/

	// defineDefaultFileDescriptors(process);

	return process;
}

PID_t execute(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode) {
	PCB *process = createProcess(processMain, argv, runMode);
	if (!process)
		return 0;
	if (!addProcess(process)) {
		freeMemory(process->stackBasePointer);
		freeMemory(process);
		return 0;
	}
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
	freeMemory(process->stackBasePointer);
	removeProcess(process);
	freeMemory(process);
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
	PCB *parent = getProcess(process->parentPid);
	if (!parent) {
		freeProcess(process);
	}
	return PID;
}

void killRunningForegroundProcess() {
	PCB *process = getForegroundProcess();
	if (process && process->pid >= 2) {
		killProcess(process->pid);
	}
}

void setProcessPriority(uint16_t pid, int8_t priority) {
	PCB *process = getProcess(pid);
	if (!process || priority < 1 || priority > 9) {
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

int8_t getFDIndex(PCB *process, int index[2]) {
	int i, found = 0;
	for (i = 0; i < MAX_FILE_DESCRIPTORS && found < 2; i++) {
		if (process->fileDescriptors[i].isBeingUsed == 0) {
			index[found++] = i;
		}
	}
	if (i == MAX_FILE_DESCRIPTORS) {
		return -1;
	}
	return 0;
}

/*int8_t createPipe(char *name, int index[2]) {
	PCB *process = getCurrentProcess();
	if (getFDIndex(process, index) == -1) {
		return -1;
	}
	FifoBuffer *fifo = createFifo(name);
	process->fileDescriptors[index[0]].pipe = fifo;
	process->fileDescriptors[index[0]].mode = 'r';
	process->fileDescriptors[index[0]].isBeingUsed = 1;

	fifo->readEnds++;

	process->fileDescriptors[index[1]].pipe = fifo;
	process->fileDescriptors[index[1]].mode = 'w';
	process->fileDescriptors[index[1]].isBeingUsed = 1;

	fifo->writeEnds++;

	return 0;
}*/
