// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <time.h>

#define DEFAULT_QTY_FDS	   3
#define STACK_DEFAULT_SIZE (1 << 16)

static uint16_t nextPid = 1;

PCB *initProcess(void *kernelStack) {
	nextPid = 1;
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
	process->blockedOn.fd = FALSE;
	process->blockedOn.timer = 0;
	process->blockedOn.manual = FALSE;
	process->priority = 1;
	for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
		process->fileDescriptors[i].pipe = NULL;
	}
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
	PCB *parent = getCurrentProcess();

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
	process->parentPid = parent->pid;
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
	for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
		process->fileDescriptors[i].pipe = parent->fileDescriptors[i].pipe;
		process->fileDescriptors[i].mode = parent->fileDescriptors[i].mode;
	}

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
	if ((PID && child && child->parentPid == process->pid) || !PID) {
		process->blockedOn.waitPID = wstatus;
		process->blockedOn.waitPID->pid = PID;
		process->status = BLOCKED;
		process->stackPointer = forceyield();
		process->blockedOn.waitPID = NULL;
		return wstatus->pid;
	}
	return 0;
}

void processSleep(uint64_t ms) {
	PCB *process = getCurrentProcess();
	uint64_t wakeUpTick = get_ticks() + ((ms * HZ) / 1000);
	process->blockedOn.timer = wakeUpTick;
	process->status = BLOCKED;
	process->stackPointer = forceyield();
	return;
}

void freeProcess(PCB *process) {
	freeMemory(process->stackBasePointer - STACK_DEFAULT_SIZE + sizeof(uint64_t));
	removeProcess(process);
	freeMemory(process);
}

PID_t killProcess(PID_t PID) {
	if (PID < 2)
		return 0;
	PCB *process = getProcess(PID);
	if (!process)
		return 0;
	process->blockedOn.manual = 0;
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

uint8_t blockProcess(uint16_t pid) {
	PCB *process = getProcess(pid);
	if (!process) {
		return FALSE;
	}
	if (process->status == BLOCKED) {
		if (process->blockedOn.waitPID == NULL && process->blockedOn.fd == 0)
			process->status = READY;
		process->blockedOn.manual = FALSE;
		return FALSE;
	}
	else if (process->status == READY) {
		process->status = BLOCKED;
		process->blockedOn.manual = TRUE;
		return TRUE;
	}
	return FALSE;
}

bool getFDEmptyIndexes(PCB *process, int pipefd[2]) {
	int i, found = 0;
	for (i = 0; i < MAX_FILE_DESCRIPTORS && found < 2; i++) {
		if (!process->fileDescriptors[i].pipe) {
			pipefd[found++] = i;
		}
	}
	if (i == MAX_FILE_DESCRIPTORS) {
		return FALSE;
	}
	return TRUE;
}

int64_t createPipe(char *name, int pipefd[2]) {
	PCB *process = getCurrentProcess();
	if (!getFDEmptyIndexes(process, pipefd)) {
		return -1;
	}
	FifoBuffer *fifo = createFifo(name);
	if (!fifo) {
		return -1;
	}
	process->fileDescriptors[pipefd[0]].pipe = fifo;
	process->fileDescriptors[pipefd[0]].mode = READ;
	fifo->readEnds++;

	process->fileDescriptors[pipefd[1]].pipe = fifo;
	process->fileDescriptors[pipefd[1]].mode = WRITE;
	fifo->writeEnds++;

	return 0;
}

int64_t openFD(char *name, FifoMode mode) {
	PCB *process = getCurrentProcess();
	if (!process)
		return -1;
	FifoBuffer *fifo = openFifo(name, mode);
	if (!fifo)
		return -1;
	for (int index = 0; index < MAX_FILE_DESCRIPTORS; index++) {
		if (!process->fileDescriptors[index].pipe) {
			process->fileDescriptors[index].pipe = fifo;
			process->fileDescriptors[index].mode = mode;
			return index;
		}
	}
	return -1;
}

int64_t closeFD(int fd) {
	PCB *process = getCurrentProcess();
	if (!process)
		return -1;
	if (fd >= MAX_FILE_DESCRIPTORS || !process->fileDescriptors[fd].pipe)
		return -1;
	closeFifo(process->fileDescriptors[fd].pipe, process->fileDescriptors[fd].mode);
	process->fileDescriptors[fd].pipe = NULL;
	return 0;
}

int64_t duplicateFD(int fd) {
	PCB *process = getCurrentProcess();
	if (!process)
		return -1;
	if (fd >= MAX_FILE_DESCRIPTORS || !process->fileDescriptors[fd].pipe)
		return -1;
	for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
		if (!process->fileDescriptors[i].pipe) {
			process->fileDescriptors[i].pipe = process->fileDescriptors[fd].pipe;
			process->fileDescriptors[i].mode = process->fileDescriptors[fd].mode;
			if (process->fileDescriptors[i].mode == READ) {
				process->fileDescriptors[i].pipe->readEnds++;
			}
			else {
				process->fileDescriptors[i].pipe->writeEnds++;
			}
			return i;
		}
	}
	return -1;
}
