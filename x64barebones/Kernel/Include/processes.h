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
			   ZOMBIE } ProcessStatus;

typedef struct PCB {
	PID_t pid;
	PID_t parentPid;
	uint8_t *stackBasePointer; // base del stack
	uint8_t *stackPointer;	   // puntero a la posicion actual
	char *name;
	uint8_t **argv;
	ProcessRunMode runMode;
	int8_t returnValue;
	uint16_t fileDescriptors[MAX_FILE_DESCRIPTORS];
	uint16_t fileDescriptorsInUse;
	int8_t priority;
	ProcessStatus status;
	uint64_t lastTickRun;
} PCB;

PCB *createProcess(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode);
PID_t execute(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode);
void exitProcess(int returnValue);
void setProcessPriority(uint16_t pid, int8_t priority);
void blockProcess(uint16_t pid);
extern uint8_t *fabricateProcessStack(uint8_t *stackBasePointer, int argc, char **argv, int (*processMain)(int argc, char **argv));
#endif
