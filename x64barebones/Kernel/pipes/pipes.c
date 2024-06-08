#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>

FifoBuffer *pipesList[PIPES_QTY] = {0};

void defineDefaultFileDescriptors(PCB *process);

uint16_t getPipeIndex() {
	int index;
	for (index = 0; pipesList[index] == NULL; index++)
		;
	return index;
}

FifoBuffer *createPipe(char *name) {
	FifoBuffer *newPipe = allocMemory(sizeof(FifoBuffer));
	if (newPipe == NULL) {
		return NULL;
	}
	strcpy(newPipe->name, name); // si el nombre es la cadena vacia 0, es un pipe anonimo.
	newPipe->readCursor = newPipe->buffer;
	newPipe->writeCursor = newPipe->buffer;
	newPipe->readEnds = 1; // El proceso que crea el pipe tiene ambos extremos en principio.
	newPipe->writeEnds = 1;

	uint16_t index = getPipeIndex();
	pipesList[index] = newPipe;

	return newPipe;
}

// Un pipe solo se puede abrir si es con nombre.
// Pone en la tabla de FileDescriptors del proceso que esta corriendo actualmente 
void openPipe(char * name, int pipefd[2]){
    if(name == NULL){
        return;
    }

    // Si el pipe no esta creado, se crea y se abre
    // pipefd[0] refers to the read end of the pipe.  
    // pipefd[1] refers to the write end of the pipe.  

}