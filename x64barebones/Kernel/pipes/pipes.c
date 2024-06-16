// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <videoDriver.h>

static FifoBuffer *pipesList[PIPES_QTY] = {0};

int64_t readFifo(FifoBuffer *fifo, char *dest, uint64_t size, bool isBlocking) {
	if (isBlocking) {
		blockFifo(fifo, READ);
	}
	uint64_t read = 0;
	while (read < size && !wouldBlock(fifo, READ)) {
		dest[read++] = *(fifo->readCursor++);
		if (fifo->readCursor >= fifo->buffer + PIPES_BUFFER_SIZE) { // BUFFER CIRCULAR
			fifo->readCursor = fifo->buffer;
		}
	}
	if (read < size && wouldBlock(fifo, READ) && !fifo->writeEnds) {
		dest[read++] = EOF;
	}
	if (read) {
		unblockFifo(fifo, WRITE);
	}
	if (!read && size && !isBlocking) {
		dest[read] = 0;
	}
	return read;
}

int64_t writeFifo(FifoBuffer *fifo, char *src, uint64_t size, bool isBlocking) {
	if (isBlocking) {
		blockFifo(fifo, WRITE);
	}
	uint64_t written = 0;
	while (written < size && !wouldBlock(fifo, WRITE)) {
		*(fifo->writeCursor++) = src[written++];
		if (fifo->writeCursor >= fifo->buffer + PIPES_BUFFER_SIZE) { // BUFFER CIRCULAR
			fifo->writeCursor = fifo->buffer;
		}
	}
	if (written) {
		unblockFifo(fifo, READ);
	}
	return written;
}

bool putFifo(FifoBuffer *fifo, char c, bool isBlocking) {
	return writeFifo(fifo, &c, 1, isBlocking);
}

char getFifo(FifoBuffer *fifo, bool isBlocking) {
	char c = EOF;
	readFifo(fifo, &c, 1, isBlocking);
	return c;
}

bool wouldBlock(FifoBuffer *fifo, FifoMode blockMode) {
	return ((blockMode == WRITE && ((fifo->writeCursor - fifo->buffer + 1) % PIPES_BUFFER_SIZE) == (fifo->readCursor - fifo->buffer)) ||
			(blockMode == READ && fifo->readCursor == fifo->writeCursor));
}

void blockFifo(FifoBuffer *fifo, FifoMode blockMode) {
	if (wouldBlock(fifo, blockMode) && ((blockMode == READ && fifo->writeEnds) || (blockMode == WRITE))) {
		// CAMBIAR A ESTADO BLOQUEADO
		PCB *process = getCurrentProcess();
		process->blockedOn.fd = TRUE;
		process->status = BLOCKED;

		BlockedProcessesNode *blockProcess = allocMemory(sizeof(BlockedProcessesNode));
		if (!blockProcess) {
			return;
		}
		blockProcess->blockedPid = getCurrentPID();
		blockProcess->next = NULL;

		BlockedProcessesNode *current = (blockMode == READ) ? fifo->blockedProcessesOnRead : fifo->blockedProcessesOnWrite;
		if (!current) {
			if (blockMode == READ) {
				fifo->blockedProcessesOnRead = blockProcess;
			}
			else {
				fifo->blockedProcessesOnWrite = blockProcess;
			}
		}
		else {
			while (current->next) {
				current = current->next;
			}
			current->next = blockProcess;
		}
		process->stackPointer = forceyield();
	}
}

void unblockFifo(FifoBuffer *fifo, FifoMode blockMode) {
	if (!wouldBlock(fifo, blockMode) || (blockMode == READ || !fifo->writeEnds)) {
		BlockedProcessesNode *blocked = (blockMode == READ) ? fifo->blockedProcessesOnRead : fifo->blockedProcessesOnWrite;
		BlockedProcessesNode *aux;
		PCB *process;
		while (blocked) {
			aux = blocked->next;
			process = getProcess(blocked->blockedPid);
			if (process) {
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
		if (!pipesList[index]) {
			return index;
		}
	}

	return -1;
}

// Un pipe solo se puede abrir si es con nombre
FifoBuffer *openFifo(char *name, FifoMode mode) {
	int i;
	for (i = 0; i < PIPES_QTY; i++) {
		if (pipesList[i] && pipesList[i]->name && strcmp(pipesList[i]->name, name) == 0) {
			if (mode == READ) {
				pipesList[i]->readEnds++;
			}
			else {
				pipesList[i]->writeEnds++;
			}
			return pipesList[i];
		}
	}
	return NULL;
}

FifoBuffer *createFifo(char *name) {
	FifoBuffer *newPipe;
	if (name) {
		newPipe = openFifo(name, READ);
		if (newPipe) {
			closeFifo(newPipe, READ);
			return NULL;
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
		strcpy(newPipe->name, name); // Si el nombre es la cadena vacia 0, es un pipe anonimo.
	}
	else {
		newPipe->name = "";
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

void closeFifo(FifoBuffer *fifo, FifoMode mode) {
	if (mode == READ) {
		fifo->readEnds--;
	}
	else {
		fifo->writeEnds--;
	}
	if (!fifo->writeEnds) {
		unblockFifo(fifo, READ);
	}
}

void setFdInfo(PCB *process, FdInfo *node, uint16_t fd) {
	node->pid = process->pid;
	node->processName = process->name;
	node->fd = fd;
	node->mode = process->fileDescriptors[fd].mode;
	node->pipeName = process->fileDescriptors[fd].pipe->name;
	node->nextFdInfo = NULL;
}

FdInfo *fdInfo(uint16_t pid) {
	PCB *process = getProcess(pid);
	int fdnum = 0;
	if (!process) {
		return NULL;
	}
	while (!process->fileDescriptors[fdnum].pipe) {
		fdnum++;
	}
	FdInfo *first = allocMemory(sizeof(FdInfo));
	setFdInfo(process, first, fdnum);
	FdInfo *newFdInfo = first;
	fdnum++;
	while (fdnum < MAX_FILE_DESCRIPTORS) {
		if (process->fileDescriptors[fdnum].pipe) {
			newFdInfo->nextFdInfo = allocMemory(sizeof(FdInfo));
			if (newFdInfo->nextFdInfo) {
				newFdInfo = newFdInfo->nextFdInfo;
				setFdInfo(process, newFdInfo, fdnum);
			}
		}
		fdnum++;
	}
	return first;
}
