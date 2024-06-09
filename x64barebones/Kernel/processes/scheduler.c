// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <time.h>

static ProcessListNode *currentProcess;
static uint8_t enableScheduler = FALSE;
uint8_t checkRemoveNode(ProcessListNode *node, PCB *pcb);
uint64_t getProcessRunPriority(ProcessListNode *candidate, uint16_t distanceFromCurrent, uint16_t processCount);
uint8_t *pickNextProcess();
uint16_t getProcessCount();
ProcessListNode *getZombieChild(PID_t parentPID, PID_t childPID);

void initScheduler(void *kernelStack) {
	// hardwire halt process
	currentProcess = allocMemory(sizeof(ProcessListNode));
	currentProcess->next = currentProcess;
	currentProcess->last = currentProcess;
	currentProcess->process = initProcess(kernelStack);
	enableScheduler = TRUE;
}

void schedyield() {
	if (enableScheduler && currentProcess && currentProcess->process) {
		currentProcess->process->stackPointer = stackSwitcharoo;
		currentProcess->process->lastTickRun = get_ticks();
		stackSwitcharoo = pickNextProcess();
	}
}

void updateCurrentStack() {
	currentProcess->process->stackPointer = stackSwitcharoo;
}

void setProcessInfo(ProcessInfo *info, ProcessListNode *node) {
	strcpy(info->name, node->process->name);
	info->pid = node->process->pid;
	info->parent_PID = node->process->parentPid;
	info->runMode = (node->process->runMode == FOREGROUND) ? 'F' : ((node->process->runMode == BACKGROUND) ? 'B' : 'R');
	info->stackBasePointer = node->process->stackBasePointer;
	info->stackPointer = node->process->stackPointer;
	switch (node->process->status) {
		case RUNNING:
			info->processStatus = "Running";
			break;
		case BLOCKED:
			info->processStatus = "Blocked";
			break;
		case READY:
			info->processStatus = "Ready";
			break;
		case ZOMBIE:
			info->processStatus = "Zombie";
			break;
		default:
			info->processStatus = "INVALID";
	}
	info->priority = node->process->priority;
	info->nextProcessInfo = NULL;
	return;
}

ProcessInfo *processInfo() {
	ProcessInfo *currentProcessInfo = allocMemory(sizeof(ProcessInfo));
	setProcessInfo(currentProcessInfo, currentProcess);
	ProcessInfo *newProcessInfo = currentProcessInfo;
	for (ProcessListNode *otherProcess = currentProcess->next; otherProcess != currentProcess; otherProcess = otherProcess->next) {
		newProcessInfo->nextProcessInfo = allocMemory(sizeof(ProcessInfo));
		if (newProcessInfo->nextProcessInfo) {
			newProcessInfo = newProcessInfo->nextProcessInfo;
			setProcessInfo(newProcessInfo, otherProcess);
		}
	}
	return currentProcessInfo;
}

// returns stack of next process to run
uint8_t *pickNextProcess() {
	uint16_t processCount = getProcessCount();
	uint16_t distanceFromCurrent = 1;
	uint64_t winningPriority = getProcessRunPriority(currentProcess, processCount, processCount);
	ProcessListNode *winningProcess = currentProcess;
	for (ProcessListNode *candidate = currentProcess->next; candidate != currentProcess; candidate = candidate->next) {
		uint64_t myPriority = getProcessRunPriority(candidate, distanceFromCurrent++, processCount);
		if (myPriority > winningPriority) {
			winningPriority = myPriority;
			winningProcess = candidate;
		}
	}
	if (currentProcess->process->status == RUNNING)
		currentProcess->process->status = READY;
	if (winningProcess != currentProcess) {
		winningProcess->next->last = winningProcess->last;
		winningProcess->last->next = winningProcess->next;
		winningProcess->next = currentProcess->next;
		winningProcess->last = currentProcess;
		currentProcess->next->last = winningProcess;
		currentProcess->next = winningProcess;
		currentProcess = currentProcess->next;
	}
	currentProcess->process->status = RUNNING;
	return currentProcess->process->stackPointer;
}

uint16_t getProcessCount() {
	uint16_t count = 1;
	for (ProcessListNode *current = currentProcess->next; current != currentProcess; current = current->next) {
		count++;
	}
	return count;
}

// round-robin w/ priorities doesn't necessarily have to be many lists. It can be only one but with magic.
// basically returns how worthy a process is to run now based on its priority, list position relative to current, and time since last run
uint64_t getProcessRunPriority(ProcessListNode *candidate, uint16_t distanceFromCurrent, uint16_t processCount) {
	if (candidate->process->status == ZOMBIE) {
		return 0;
	}
	// is hlt
	if (candidate->process->pid == 0) {
		return 1;
	}
	if (candidate->process->status == BLOCKED) {
		ProcessListNode *zombieChild = getZombieChild(candidate->process->pid, candidate->process->blockedOn.waitPID->pid);
		if (zombieChild) {
			candidate->process->status = READY;
			candidate->process->blockedOn.waitPID->pid = zombieChild->process->pid;
			candidate->process->blockedOn.waitPID->aborted = zombieChild->process->killed;
			candidate->process->blockedOn.waitPID->returnValue = zombieChild->process->returnValue;
			freeProcess(zombieChild->process);
		}
		else {
			return 0;
		}
	}
	return (uint64_t) ((1L << candidate->process->priority) * ((double) processCount / distanceFromCurrent)) + (1L << (get_ticks() - candidate->process->lastTickRun));
}

ProcessListNode *getZombieChild(PID_t parentPID, PID_t childPID) {
	if (currentProcess->process->status == ZOMBIE)
		return currentProcess;
	for (ProcessListNode *child = currentProcess->next; child != currentProcess; child = child->next) {
		if (child->process->status == ZOMBIE && child->process->parentPid == parentPID && (childPID == 0 || child->process->pid == childPID))
			return child;
	}
	return NULL;
}

uint8_t addProcess(PCB *pcb) {
	ProcessListNode *newProcess = allocMemory(sizeof(ProcessListNode));
	if (!newProcess) {
		return FALSE;
	}
	newProcess->process = pcb;
	newProcess->next = currentProcess->next;
	newProcess->last = currentProcess;
	currentProcess->next = newProcess;
	newProcess->next->last = newProcess;
	return TRUE;
}
PID_t getCurrentPID() {
	if (currentProcess && currentProcess->process) {
		return currentProcess->process->pid;
	}
	return 0;
}
PCB *getCurrentProcess() {
	return currentProcess->process;
}
PCB *getForegroundProcess() {
	ProcessListNode *candidate = currentProcess;
	do {
		if (candidate->process->runMode == FOREGROUND)
			return candidate->process;
		candidate = candidate->next;
	} while (candidate != currentProcess);
	return NULL;
}
PCB *getProcess(PID_t pid) {
	ProcessListNode *candidate = currentProcess;
	do {
		if (candidate->process->pid == pid)
			return candidate->process;
		candidate = candidate->next;
	} while (candidate != currentProcess);
	return NULL;
}
uint8_t removeProcess(PCB *process) {
	ProcessListNode *nextNode = currentProcess->next;
	if (checkRemoveNode(currentProcess, process)) {
		currentProcess = nextNode;
		return TRUE;
	}
	while (nextNode != currentProcess) {
		if (checkRemoveNode(nextNode, process)) {
			return TRUE;
		}
		nextNode = nextNode->next;
	}
	return FALSE;
}

// removes node from the list if matches PID. Returns TRUE if deleted, FALSE if not.
uint8_t checkRemoveNode(ProcessListNode *node, PCB *pcb) {
	if (node->process->pid == pcb->pid) {
		node->last->next = node->next;
		node->next->last = node->last;
		freeMemory(node);
		return TRUE;
	}
	return FALSE;
}