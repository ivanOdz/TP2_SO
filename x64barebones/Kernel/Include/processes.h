#ifndef PROCESSES_H
#define PROCESSES_H

#define MAX_FILE_DESCRIPTORS 64
#define MAX_NAME_SIZE		 25
#define MAX_PROCESSES		 250

#include <libc.h>
#include <pipesManager.h>
#include <stdint.h>

typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE
} ProcessStatus;

typedef struct BlockedOn {
	ReturnStatus *waitPID; // NULL means not blocked on PID, internal PID == 0 means any child, internal PID > 0 means that one
	int8_t fd;
	int64_t timer;
	uint8_t manual;
} BlockedOn;

typedef struct FileDescriptors {
	FifoBuffer *pipe;
	FifoMode mode;
} FileDescriptors;

typedef struct PCB {
	PID_t pid;
	PID_t parentPid;
	void *stackBasePointer; // Base del stack
	void *stackPointer;		// Puntero a la posicion actual
	char *name;
	char **argv;
	ProcessRunMode runMode;
	int64_t returnValue;
	uint8_t killed;
	FileDescriptors fileDescriptors[MAX_FILE_DESCRIPTORS]; // El indice del arreglo determina el fd.
	int8_t priority;
	ProcessStatus status;
	BlockedOn blockedOn;
	uint64_t lastTickRun;
	uint64_t raxPreserve;
} PCB;

PCB *initProcess(void *kernelStack);
PCB *createProcess(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode);
void freeProcess(PCB *process);
PID_t execute(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode);
uint64_t exitProcess(int returnValue);
PID_t killProcess(PID_t PID);
void killRunningForegroundProcess();
PID_t waitPID(PID_t PID, ReturnStatus *wstatus);
void processSleep(uint64_t ms);
bool setProcessPriority(uint16_t pid, int8_t priority);
bool blockProcess(uint16_t pid);
extern uint8_t *fabricateProcessStack(uint8_t *stackBasePointer, int argc, char **argv, int (*processMain)(int argc, char **argv));
bool getFDEmptyIndexes(PCB *process, int index[2]);
int64_t createPipe(char *name, int index[2]);
int64_t openFD(char *name, FifoMode mode);
int64_t closeFD(int fd);
int64_t duplicateFD(int fd);

#endif