#include <processes.h>

typedef struct ProcessListNode {
	PCB *process;
	struct ProcessListNode *next;
	struct ProcessListNode *last;
} ProcessListNode;

void initScheduler();
uint8_t addProcess(PCB *pcb);
uint8_t removeProcess(PCB *process);
PID_t getCurrentPID();
PCB *getCurrentProcess();
PCB *getProcess(PID_t pid);
void schedyield();
PCB *getForegroundProcess();
void *forceyield();
ProcessInfo *processInfo();
extern uint8_t *stackSwitcharoo;
extern uint8_t *yieldInterrupt(uint8_t *preserveStack);