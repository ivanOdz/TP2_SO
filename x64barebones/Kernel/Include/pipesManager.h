#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>
#include <libc.h>

#define PIPES_QTY		  (1 << 12)
#define PIPES_BUFFER_SIZE (1 << 12)
#define PIPES_NAME_SIZE	  20

// El proceso que tenga solamente para leer de un pipe, le voy a dar el readCursor
// El proceso que tenga solamente para escribir un pipe, le voy a dar el writeCursor

typedef enum { READ = 0,
			   WRITE
} WaitingFor;

typedef struct BlockedProcessesNode{
	PID_t blockedPid;
	WaitingFor event;
	struct BlockedProcessesNode * next;
} BlockedProcessesNode;

typedef struct {
	char name[PIPES_NAME_SIZE];
	char buffer[PIPES_BUFFER_SIZE];
	char *readCursor;
	char *writeCursor;
	uint16_t readEnds;
	uint16_t writeEnds;
	BlockedProcessesNode * blockedProcesses;
} FifoBuffer;

#endif