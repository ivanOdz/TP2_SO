#include <lib.h>
#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <time.h>

static ProcessListNode *currentProcess;
uint8_t checkRemoveNode(ProcessListNode *node, PID_t pid);
uint64_t getProcessRunPriority(ProcessListNode *candidate, uint16_t distanceFromCurrent, uint16_t processCount);
uint8_t *pickNextProcess();
uint16_t getProcessCount();

void initScheduler() {
	// hardwire halt process
	currentProcess = allocMemory(sizeof(ProcessListNode));
	currentProcess->next = currentProcess;
	currentProcess->last = currentProcess;
	char *argv[2] = {"System idle process", NULL};
	currentProcess->process = createProcess(haltProcess, argv, BACKGROUND);
}

void yield() {
	if (currentProcess && currentProcess->process) {
		currentProcess->process->stackPointer = stackSwitcharoo;
		currentProcess->process->lastTickRun = get_ticks();
		stackSwitcharoo = pickNextProcess();
	}
}

// returns stack of next process to run
uint8_t *pickNextProcess() {
	uint64_t winningPriority = 0;
	ProcessListNode *winningProcess = currentProcess;
	uint16_t processCount = getProcessCount();
	uint16_t distanceFromCurrent = 1;
	for (ProcessListNode *candidate = currentProcess->next; candidate != currentProcess; candidate = candidate->next) {
		uint64_t myPriority = getProcessRunPriority(candidate, distanceFromCurrent++, processCount);
		if (myPriority > winningPriority) {
			winningPriority = myPriority;
			winningProcess = candidate;
		}
	}
	currentProcess = winningProcess;
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
	if (candidate->process->status == BLOCKED || candidate->process->status == ZOMBIE) {
		return 0;
	}
	// is hlt
	if (candidate->process->pid == 0) {
		return 1;
	}
	return (uint64_t) ((1 << candidate->process->priority) * ((double) processCount / distanceFromCurrent)) + (1 << (get_ticks() - candidate->process->lastTickRun));
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
uint8_t removeProcess(PID_t pid) {
	if (checkRemoveNode(currentProcess, pid)) {
		return TRUE;
	}
	ProcessListNode *nextNode = currentProcess->next;
	while (nextNode != currentProcess) {
		if (checkRemoveNode(nextNode, pid)) {
			return TRUE;
		}
	}
	return FALSE;
}

// removes node from the list if matches PID. Returns TRUE if deleted, FALSE if not.
uint8_t checkRemoveNode(ProcessListNode *node, PID_t pid) {
	if (node->process && node->process->pid == pid) {
		node->last->next = node->next;
		node->next->last = node->last;
		free(node);
		return TRUE;
	}
	return FALSE;
}