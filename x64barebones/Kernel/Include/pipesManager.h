#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>

#define PIPES_QTY		 (1 << 12)
#define PIPES_BUFFER_SIZE (1 << 12)
#define PIPES_NAME_SIZE	 20

typedef struct {
	char name[PIPES_NAME_SIZE];
	char buffer[PIPES_BUFFER_SIZE];
	uint8_t *readCursor;
	uint8_t *writeCursor;
    uint16_t readEnds;
    uint16_t writeEnds;
} FifoBuffer;


#endif