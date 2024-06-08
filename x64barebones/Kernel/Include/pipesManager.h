#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>

#define PIPES_QTY		 (1 << 12)
#define PIPES_BUFFER_SIZE (1 << 12)
#define PIPES_NAME_SIZE	 20

// El proceso que tenga solamente para leer de un pipe, le voy a dar el readCursor
// El proceso que tenga solamente para escribir un pipe, le voy a dar el writeCursor
typedef struct {
	char name[PIPES_NAME_SIZE];
	char buffer[PIPES_BUFFER_SIZE];
	uint8_t *readCursor;
	uint8_t *writeCursor;
    uint16_t readEnds;
    uint16_t writeEnds;
} FifoBuffer;


#endif