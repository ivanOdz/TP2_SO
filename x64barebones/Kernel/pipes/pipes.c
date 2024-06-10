// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>

static FifoBuffer *pipesList[PIPES_QTY] = {0};

int64_t readFifo(FifoBuffer *fifo, char *dest, uint64_t size, bool blocking) {
	if (blocking)
		blockFifo(fifo, READ);
	uint64_t i = 0;
	while (i < size && *(fifo->readCursor) != EOF && !wouldBlock(fifo, READ)) {
		dest[i++] = *(fifo->readCursor++);
		if (fifo->readCursor >= fifo->buffer + PIPES_BUFFER_SIZE) { // BUFFER CIRCULAR
			fifo->readCursor = fifo->buffer;
		}
	}
	if (i)
		unblockFifo(fifo, WRITE);
	return i;
}

int64_t writeFifo(FifoBuffer *fifo, char *src, uint64_t size, bool blocking) {
	if (blocking)
		blockFifo(fifo, WRITE);
	uint64_t i = 0;
	while (i < size && !wouldBlock(fifo, WRITE)) {
		*(fifo->writeCursor++) = src[i++];
		if (fifo->writeCursor >= fifo->buffer + PIPES_BUFFER_SIZE) { // BUFFER CIRCULAR
			fifo->writeCursor = fifo->buffer;
		}
	}
	if (i)
		unblockFifo(fifo, READ);
	return i;
}

bool putFifo(FifoBuffer *fifo, char c, bool blocking) {
	return writeFifo(fifo, &c, 1, blocking);
}
char getFifo(FifoBuffer *fifo, bool blocking) {
	char c = EOF;
	readFifo(fifo, &c, 1, blocking);
	return c;
}

bool wouldBlock(FifoBuffer *fifo, FifoMode blockMode) {
	return ((blockMode == WRITE && ((fifo->writeCursor - fifo->buffer + 1) % PIPES_BUFFER_SIZE) == (fifo->readCursor - fifo->buffer)) ||
			(blockMode == READ && fifo->readCursor == fifo->writeCursor));
}

void blockFifo(FifoBuffer *fifo, FifoMode blockMode) {
	if (wouldBlock(fifo, blockMode)) {
		// CHANGE STATE A BLOCKED
		PCB *process = getCurrentProcess();
		process->blockedOn.fd = TRUE;
		process->status = BLOCKED;

		BlockedProcessesNode *blockProcess = allocMemory(sizeof(BlockedProcessesNode));
		if (blockProcess == NULL) {
			return;
		}
		blockProcess->blockedPid = getCurrentPID();
		blockProcess->next = NULL;

		BlockedProcessesNode *current = (blockMode == READ) ? fifo->blockedProcessesOnRead : fifo->blockedProcessesOnWrite;
		if (current == NULL) {
			if (blockMode == READ)
				fifo->blockedProcessesOnRead = blockProcess;
			else
				fifo->blockedProcessesOnWrite = blockProcess;
		}
		else {
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = blockProcess;
		}
		// YIELD
		process->stackPointer = forceyield();
	}
}

void unblockFifo(FifoBuffer *fifo, FifoMode blockMode) {
	if ((blockMode == WRITE && ((fifo->writeCursor - fifo->buffer + 1) % PIPES_BUFFER_SIZE) != (fifo->readCursor - fifo->buffer)) ||
		(blockMode == READ && (fifo->writeCursor != fifo->readCursor))) {
		BlockedProcessesNode *blocked = (blockMode == READ) ? fifo->blockedProcessesOnRead : fifo->blockedProcessesOnWrite;
		BlockedProcessesNode *aux;
		PCB *process;
		while (blocked) {
			aux = blocked->next;
			if ((process = getProcess(blocked->blockedPid))) {
				process->blockedOn.fd = FALSE;
			}
			freeMemory(blocked);
			blocked = aux;
		}
		if (blockMode == READ) {
			fifo->blockedProcessesOnRead = NULL;
		}
		else {
			fifo->blockedProcessesOnWrite = NULL;
		}
	}
}

int64_t getPipeIndex() {
	for (int index = 0; index < PIPES_QTY; index++) {
		if (!pipesList[index])
			return index;
	}

	return -1;
}

// Un pipe solo se puede abrir si es con nombre.
FifoBuffer *openPipe(char *name) {
	int i;
	for (i = 0; i < PIPES_QTY; i++) {
		if (pipesList[i] && pipesList[i]->name && strcmp(pipesList[i]->name, name) == 0)
			break;
	}
	if (i == PIPES_QTY) {
		return NULL;
	}
	return pipesList[i];
}

FifoBuffer *createFifo(char *name) {
	FifoBuffer *newPipe;
	if (name) {
		newPipe = openPipe(name);
		if (newPipe) {
			return newPipe;
		}
	}
	int32_t index = getPipeIndex();
	if (index == -1) {
		return NULL;
	}
	newPipe = allocMemory(sizeof(FifoBuffer));
	if (!newPipe) {
		return NULL;
	}
	if (name) {
		uint64_t size = strlen(name);
		newPipe->name = allocMemory(size);
		strcpy(newPipe->name, name); // si el nombre es la cadena vacia 0, es un pipe anonimo.
	}
	else {
		newPipe->name = "\0";
	}
	newPipe->readCursor = newPipe->buffer;
	newPipe->writeCursor = newPipe->buffer;
	newPipe->readEnds = 0;
	newPipe->writeEnds = 0;
	newPipe->blockedProcessesOnRead = NULL;
	newPipe->blockedProcessesOnWrite = NULL;

	pipesList[index] = newPipe;

	return newPipe;
}
