#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

#include <stdint.h>
#include "./processes.h"
#include "./../Asm/libasm.asm"
#include "./../memoryManagers/memoryManager.h"

#define SEMAPHORES_MAX           128
#define SEM_BLK_PRC_ARR_SIZE     32

static semaphore semaphores[SEMAPHORES_MAX] = {0};

typedef struct {

    uint16_t pids[SEM_BLK_PRC_ARR_SIZE];
    uint16_t first;
    uint16_t last;
    uint16_t lastPid;

} sem_blk_prc;

typedef struct {

    int32_t access;
    int32_t counter;
    sem_blk_prc *blockedProcessesAccess;
    sem_blk_prc *blockedProcessesCounter;

} semaphore;

void semaphore_initialize();

uint16_t semaphore_create(uint32_t initialValue);

uint16_t semaphore_open(uint16_t id);

uint16_t semaphore_close(uint16_t id);

void semaphore_binary_post(uint16_t id);

void semaphore_binary_wait(uint16_t id);

void semaphore_post(uint16_t id);

void semaphore_wait(uint16_t id);

#endif  // SEMAPHORES_H_