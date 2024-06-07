#include "./../Include/semaphores.h"

static uint16_t semaphoreCheckId(uint16_t id) {

    return (id && semaphores[id] != NULL && semaphores[id].blockedProcessesAccess != NULL); // && semaphores[id].blockedProcessescounter != NULL)
}

static uint16_t semaphoreFindFirstFree() {

    for (uint16_t cont=1; cont < SEMAPHORES_MAX; cont++) {
        if (semaphores[cont] == NULL) {
            return cont;
        }
    }
    return 0;
}

static void semaphoreClearBlockProcesses(sem_blk_prc *blkPrc) {

    blkPrc->first = 0;
    blkPrc->last = 0;
    blkPrc->lastPid = 0;

    for (uint16_t cont=0; cont < SEM_BLK_PRC_ARR_SIZE; cont++) {    // [!] Se podria usar memcpy, quizá más eficiente..
        blkPrc->pids[cont] = 0;
    }
}

static void semaphoresInitialize() {   // semaphores[0] puede servir para tener mutex sobre la misma estructura. Puede servir si se quiere hacer de tamaño dinámico

    semaphore *candidate = allocMemory(sizeof(semaphores[0]));
    candidate->access = 0;
    candidate->counter = 0;
    candidate->blockedProcessesCounter = NULL;
    candidate->blockedProcessesAccess = allocMemory(sizeof(semaphores[0]->blockedProcessesAccess));

    semaphoreClearBlockProcesses(candidate->blockedProcessesAccess);

    if (atomicCompareExchange(semaphores[0], NULL, candidate) != candidate) {
        freeMemory(candidate);
    }
}

void semaphoreBinaryPost(uint16_t id) {

    if (semaphoreCheckId(id)) {

        

    }
}

void semaphoreBinaryWait(uint16_t id) {

}

uint16_t semaphoreCreate(uint32_t initialValue) {

    if (semaphores[0] == NULL) {
        semaphoresInitialize();
    }

    semaphore *newSem = allocMemory(sizeof(semaphores[0]));
    newSem->access = 0;
    newSem->counter = initialValue;
    newSem->blockedProcessesAccess = allocMemory(sizeof(newSem->blockedProcessesAccess));
    newSem->blockedProcessesCounter = allocMemory(sizeof(newSem->blockedProcessesCounter));

    semaphoreClearBlockProcesses(newSem->blockedProcessesAccess);
    semaphoreClearBlockProcesses(newSem->blockedProcessesCounter);
    
    uint16_t newSemId;

    do {
        newSemId = semaphoreFindFirstFree();
    } while (newSemId && (atomicCompareExchange(semaphores[newSemId], NULL, newSem) != newSem));

    if (newSemId) {
        semaphoreBinaryPost(newSemId);
    }
    else {
        freeMemory(newSem);
    }

    return newSemId;
}

uint16_t semaphoreOpen(uint16_t id) {
    return 0;
}

uint16_t semaphoreClose(uint16_t id) {
    return 0;
}

void semaphorePost(uint16_t id) {
    return; 
}

void semaphoreWait(uint16_t id) {
    return;
}