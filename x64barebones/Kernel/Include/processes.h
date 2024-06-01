#ifndef PROCESSES_H
#define PROCESSES_H

#define MAX_FILE_DESCRIPTORS 64
#define MAX_NAME_SIZE		 25

#include <stdint.h>


typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE } ProcessStatus;

typedef enum { FOREGROUND = 0,
			   BACKGROUND } ProcessRunMode;

typedef struct {
	uint16_t pid;
	uint16_t parentPid;
	uint8_t *stackBasePointer;	   // base del stack
	uint8_t *stackPointer; // puntero a la posicion actual
	uint8_t name[MAX_NAME_SIZE];
	uint8_t **argv;
	ProcessRunMode runMode;
	int8_t returnValue;
	uint16_t fileDescriptors[MAX_FILE_DESCRIPTORS];
	uint16_t fileDescriptorsInUse;
	int8_t priority;
	ProcessStatus status;	
} PCB;

int8_t createProcess(const char *name, uint8_t **argv, ProcessRunMode runMode, uint16_t parentPid);
uint64_t ps();

#endif
