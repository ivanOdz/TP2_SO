#include <libc.h>
#include <stdint.h>
#include <processes.h>

#define MAX_PROCESSES	250
#define DEFAULT_QTY_FDS 3

static uint16_t nextPid = 1;

static PCB processes[MAX_PROCESSES] = {0};

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

int8_t createProcess(const char *name, const char **argv, ProcessRunMode runMode, uint16_t parentPid) {
	int16_t pos = getNextPosition();
	if (pos < 0) {
		return -1;
	}
	processes[pos].pid = nextPid++;
	processes[pos].parentPid = parentPid;
	//processes[pos].stackBase = getStackBase();
	//processes[pos].stackPointer = processes[pos].stackBase;
	processes[pos].status = READY;
	//processes[pos].argv = argv;
	processes[pos].runMode = runMode;
	processes[pos].returnValue = 0;

	for (int i = 0; i < DEFAULT_QTY_FDS; i++) {
		processes[pos].fileDescriptors[i] = i;
	}
	processes[pos].fileDescriptorsInUse = DEFAULT_QTY_FDS;
	processes[pos].priority = 1;

	//printf("LLego a crearse el proceso %s con modo %d y el padre es PID: %d\n", name, runMode, parentPid);
	return processes[pos].pid;
}
/*
int8_t getProcessIndex(int16_t pid) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		if (processes[i].pid == pid) {
			return i;
		}
	}
	return -1;
}

void killProcess(uint16_t pid) {
	int8_t index = getProcessIndex(pid);
	if (index < 0) {
		return;
	}
	processes[index].status = STOPPED; // Para que no vuelva a ser elegido por el scheduler.
	processes[index].stackBase = NULL;
	processes[index].stackPointer = NULL;
}

void yieldProcess(uint16_t pid) {
	int8_t index = getProcessIndex(pid);
	if (index < 0 && processes[index].pid != pid) {
		return;
	}
	if (processes[index].status == RUNNING) {
		processes[index].status = READY;
		interesting_handler(); // Cambio el estado en caso de que dicho proceso estaba corriendo y fuerzo la interrupcion
	}
}

void setProcessPriority(uint16_t pid, uint8_t priority) {
	int8_t index = getProcessIndex(pid);
	if (index < 0) {
		return;
	}
	processes[index].priority = priority;
}

void setProcessState(uint16_t pid, ProcessStatus ps) {
	int8_t index = getProcessIndex(pid);
	if (index < 0 || ps == RUNNING) { // No puedo dejar que cambien el estado a running, eso lo hace el scheduler.
		return;
	}
	// Si cambio el estado a READY del proceso actual, tengo que llamar al scheduler
	// Si cambio el estado a BLOCKED del proceso actual, tengo que llamar al scheduler.
	processes[index].status = ps;
	if (getCurrentPid() == pid) {
		interesting_handler();
	}
}
*/