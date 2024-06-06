#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

#include <stdint.h>
#include "./processes.h"

#define SEMAPHORES_MAX          128


typedef struct {
    
    int32_t counter;
    int32_t access;
    // uint16_t totalBlockedPids;
    // uint16_t *blockedPids;
    // 
} semaphore;


int32_t semaphore_create(int32_t initialValue);

int32_t semaphore_open(int32_t id);

int32_t semaphore_close(int32_t id);

void semaphore_post(int32_t id);

void semaphore_wait(int32_t id);

#endif  // SEMAPHORES_H_