#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

#include <stdint.h>
#include "./processes.h"

#define SEMAPHORES_MAX           128
#define SEM_BLK_PRC_ARR_SIZE     32

typedef struct {

    uint16_t pids[SEM_BLK_PRC_ARR_SIZE];
    uint16_t *first;
    uint16_t *last;

} sem_blk_prc;

typedef struct {

    int32_t access;
    int32_t counter;
    sem_blk_prc blockedProcessesAccess;
    sem_blk_prc blockedProcessesCounter;

} semaphore;

int32_t semaphore_create(int32_t initialValue);

int32_t semaphore_open(int32_t id);

int32_t semaphore_close(int32_t id);

void semaphore_post(int32_t id);

void semaphore_wait(int32_t id);

#endif  // SEMAPHORES_H_