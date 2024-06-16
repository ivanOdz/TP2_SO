#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <processes.h>

typedef struct ProcessListNode {
	PCB *process;
	struct ProcessListNode *next;
	struct ProcessListNode *last;
} ProcessListNode;

void initScheduler(void *kernelStack);
void updateCurrentStack();
bool addProcess(PCB *pcb);
bool removeProcess(PCB *process);
PID_t getCurrentPID();
PCB *getCurrentProcess();
PCB *getProcess(PID_t pid);
uint64_t schedyield();
PCB *getForegroundProcess();
void *forceyield();
ProcessInfo *processInfo();
extern void *stackSwitcharoo;
extern uint8_t *yieldInterrupt(uint8_t *preserveStack);

#endif