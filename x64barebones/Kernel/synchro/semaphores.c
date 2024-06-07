#include "./../Include/semaphores.h"


static uint16_t semaphore_check_id(uint16_t id) {

    return (semaphores[id] != NULL && semaphores[id].blockedProcessesAccess != NULL); // && semaphores[id].blockedProcessescounter != NULL)
}

static uint16_t semaphore_find_first_free() {

    for (uint16_t cont=1; cont < SEMAPHORES_MAX; cont++) {
        if (semaphores[cont] == NULL) {
            return cont;
        }
    }
    return 0;
}

static void semaphore_clear_block_processes(sem_blk_prc *blkPrc) {

    blkPrc->first = 0;
    blkPrc->last = 0;
    blkPrc->lastPid = 0;

    for (uint16_t cont=0; cont < SEM_BLK_PRC_ARR_SIZE; cont++) {    // [!] Se podria usar memcpy, quizá más eficiente..
        blkPrc->pids[cont] = 0;
    }
}

void semaphores_initialize() {

    semaphore *candidate = allocMemory(sizeof(semaphores[0]));
    candidate->access = 0;
    candidate->counter = 0;
    candidate->blockedProcessesCounter = NULL;
    candidate->blockedProcessesAccess = allocMemory(sizeof(semaphores[0]->blockedProcessesAccess));

    semaphore_clear_block_processes(candidate->blockedProcessesAccess);
/*
    if (atomicCompareExchange(NULL, semaphores[0], candidate) != canditate) {
        freeMemory(candidate);
    }*/
}

uint16_t semaphore_create(uint32_t initialValue) {

    return 0;
}

uint16_t semaphore_open(uint16_t id) {
    return 0;
}

uint16_t semaphore_close(uint16_t id) {
    return 0;
}

void semaphore_binary_post(uint16_t id) {


}

void semaphore_binary_wait(uint16_t id) {

}

void semaphore_post(uint16_t id) {
    return; 
}

void semaphore_wait(uint16_t id) {
    return;
}