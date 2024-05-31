#ifndef PROCESS_H
#define PROCESS_H

#define MAX_FILE_DESCRIPTORS 64
#define MAX_NAME_SIZE 25


#include <stdint.h>

typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING } ProcessStatus;

typedef enum { FOREGROUND,
			   BACKGROUND } ProcessRunMode;

typedef struct {
	uint16_t pid;
	uint16_t parentPid;
	uint8_t *stackBase;	   // base del stack
	uint8_t *stackPointer; // puntero a la posicion actual
	ProcessStatus status;
	uint8_t name[MAX_NAME_SIZE];
	uint8_t **argv;
	ProcessRunMode runMode;
	uint8_t returnValue;
	uint16_t fileDescriptors[MAX_FILE_DESCRIPTORS];
	uint16_t fileDescriptorsInUse;
	uint8_t priority;
} PCB;

#endif