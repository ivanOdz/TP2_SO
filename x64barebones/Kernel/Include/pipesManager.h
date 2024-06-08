#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>

#define PIPES_QTY	   (1 << 12)
#define BUFFER_SIZE	   (1 << 12)
#define PIPE_NAME_SIZE 20

typedef struct {
	char name[PIPE_NAME_SIZE];
	char buffer[BUFFER_SIZE];
	uint8_t *readCursor;
	uint8_t *writeCursor;
} FifoBuffer;

#endif