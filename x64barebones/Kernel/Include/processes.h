#ifndef PROCESSES_H
#define PROCESSES_H

#define MAX_FILE_DESCRIPTORS 64
#define MAX_NAME_SIZE		 25
#define MAX_PROCESSES		 250

#include <libc.h>
#include <stdint.h>

typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE
} ProcessStatus;

typedef struct BlockedOn {
	ReturnStatus *waitPID; // NULL means not blocked on PID, internal PID == 0 means any child, internal PID > 0 means that one
	int64_t fd;
	int64_t timer;
	// sem_t semaphore;
} BlockedOn;

typedef struct PCB {
	PID_t pid;
	PID_t parentPid;
	uint8_t *stackBasePointer; // base del stack
	uint8_t *stackPointer;	   // puntero a la posicion actual
	char *name;
	uint8_t **argv;
	ProcessRunMode runMode;
	int8_t returnValue;
	uint8_t killed;
	uint16_t fileDescriptors[MAX_FILE_DESCRIPTORS];
	uint16_t fileDescriptorsInUse;
	int8_t priority;
	ProcessStatus status;
	BlockedOn blockedOn;
	uint64_t lastTickRun;
} PCB;

PCB *createProcess(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode);
PID_t execute(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode);
void exitProcess(int returnValue);
void waitPID(PID_t PID, ReturnStatus *wstatus);
void setProcessPriority(uint16_t pid, int8_t priority);
void blockProcess(uint16_t pid);
extern uint8_t *fabricateProcessStack(uint8_t *stackBasePointer, int argc, char **argv, int (*processMain)(int argc, char **argv));

#endif