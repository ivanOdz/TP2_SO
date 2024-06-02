#include <processes.h>

typedef struct ProcessListNode {
	PCB *process;
	struct ProcessListNode *next;
	struct ProcessListNode *last;
} ProcessListNode;

void initScheduler();
uint8_t addProcess(PCB *pcb);
uint8_t removeProcess(PID_t pid);
PID_t getCurrentPID();
PCB *getCurrentProcess();
void yield();
ProcessInfo *processInfo();
extern uint8_t *stackSwitcharoo;