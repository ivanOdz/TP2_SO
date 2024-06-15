#ifndef PIPES_H
#define PIPES_H

#include <libc.h>
#include <stdint.h>

#define PIPES_QTY			(1 << 12)
#define PIPES_BUFFER_SIZE	(1 << 12)
#define PIPES_NAME_SIZE		20

// El proceso que tenga solamente para leer de un pipe, le voy a dar el readCursor y al proceso que tenga solamente para escribir un pipe, le voy a dar el writeCursor
typedef struct BlockedProcessesNode {
	PID_t blockedPid;
	struct BlockedProcessesNode *next;
} BlockedProcessesNode;

typedef struct {
	char *name;
	char buffer[PIPES_BUFFER_SIZE];
	char *readCursor;
	char *writeCursor;
	uint16_t readEnds;
	uint16_t writeEnds;
	BlockedProcessesNode *blockedProcessesOnRead;
	BlockedProcessesNode *blockedProcessesOnWrite;
} FifoBuffer;

FifoBuffer *createFifo(char *name);
FifoBuffer *openFifo(char *name, FifoMode mode);
void closeFifo(FifoBuffer *fifo, FifoMode mode);
bool wouldBlock(FifoBuffer *fifo, FifoMode blockMode);
void unblockFifo(FifoBuffer *fifo, FifoMode mode);
void blockFifo(FifoBuffer *fifo, FifoMode mode);
int64_t writeFifo(FifoBuffer *fifo, char *src, uint64_t size, bool blocking);
int64_t readFifo(FifoBuffer *fifo, char *dest, uint64_t size, bool blocking);
bool putFifo(FifoBuffer *fifo, char c, bool blocking);
char getFifo(FifoBuffer *fifo, bool blocking);
void printFileDescriptors(PID_t pid);
FdInfo *fdInfo(PID_t pid);

#endif