// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>

FifoBuffer *pipesList[PIPES_QTY] = {0};

void defineDefaultFileDescriptors(PCB *process);

int32_t getPipeIndex() {
	int index;
	for (index = 0; pipesList[index] == NULL && index < PIPES_QTY; index++)
		;
	if (index == PIPES_QTY) {
		return -1;
	}
	return index;
}

// Un pipe solo se puede abrir si es con nombre.
FifoBuffer *openPipe(char *name) {
	int i;
	for (i = 0; strcmp(pipesList[i]->name, name) != 0 && i < PIPES_QTY; i++)
		;
	if (i == PIPES_QTY) {
		return NULL;
	}
	return pipesList[i];
}

FifoBuffer *createFifo(char *name) {
	FifoBuffer *newPipe;
	if (name != NULL) {
		newPipe = openPipe(name);
		if (newPipe != NULL) {
			return newPipe;
		}
	}
	uint16_t index = getPipeIndex();
	if (index == -1) {
		return NULL;
	}
	newPipe = allocMemory(sizeof(FifoBuffer));
	if (newPipe == NULL) {
		return NULL;
	}
	strcpy(newPipe->name, name); // si el nombre es la cadena vacia 0, es un pipe anonimo.
	newPipe->readCursor = newPipe->buffer;
	newPipe->writeCursor = newPipe->buffer;
	newPipe->readEnds = 0;
	newPipe->writeEnds = 0;

	pipesList[index] = newPipe;

	return newPipe;
}
