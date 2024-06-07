#include "./../Include/semaphores.h"

static semaphore *semaphoreGetById(uint16_t id) {

    semaphore *sem = NULL;

    if (id) {
        sem = semaphores[id];
    }
    if (sem->blockedProcessesAccess == NULL) {
        sem = NULL;    
    }

    return sem;
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

    semaphore *sem = semaphoreGetById(id);

    if (sem != NULL && sem->access == 0) {

        PID_t luckyPid;
        uint16_t actualPosition = sem->blockedProcessesAccess->first;
        
        while (actualPosition != sem->blockedProcessesAccess->last) {
            
            actualPosition = atomicCompareExchange(&sem->blockedProcessesAccess->first, actualPosition, actualPosition+1);
            luckyPid = atomicExchange(&sem->blockedProcessesAccess->pids[actualPosition], 0);

            if (luckyPid) {
                // change status to RUN if everything is OK
            }
        }

    }
}

void semaphoreBinaryWait(uint16_t id) {

    semaphore *sem = semaphoreGetById(id);

    if (sem != NULL) {

        while (atomicExchange(&sem->access, 1)) {

            PID_t myPid = getCurrentPID();
            atomicHighValueCheck(&sem->blockedProcessesAccess->last, SEM_BLK_PRC_ARR_SIZE, 0);

            if (sem->blockedProcessesAccess->last +1 != sem->blockedProcessesAccess->first) {

                uint16_t myPosition = (uint16_t)atomicAdd(&sem->blockedProcessesAccess->last, 1);
                atomicHighValueCheck(&sem->blockedProcessesAccess->last, SEM_BLK_PRC_ARR_SIZE, 0);
// Si no estaba en 0, es posible que se haya llegado a llenar el arreglo! No hace falta decrementar last, porque funciona como un fin de carrera
                if (sem->blockedProcessesAccess->pids[myPosition] == 0) { 
                    sem->blockedProcessesAccess->pids[myPosition] = myPid;
                    blockProcess(myPid);
                }
            }
            else {
                yield();
            }
        }
    }
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