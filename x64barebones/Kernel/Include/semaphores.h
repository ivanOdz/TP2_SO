#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

#include <libc.h>
#include <lib.h>
#include <memoryManager.h>
#include <scheduler.h>

#define SEMAPHORES_MAX		 128
#define SEM_BLK_PRC_ARR_SIZE 32

typedef struct str_sem_blk_prc{
	PID_t pids[SEM_BLK_PRC_ARR_SIZE];
	uint16_t first;
	uint16_t last;

} sem_blk_prc;

typedef struct str_semaphore{
	int32_t access;
	int32_t counter;
	sem_blk_prc *blockedProcessesAccess;
	sem_blk_prc *blockedProcessesCounter;
} semaphore;

uint16_t semaphoreCreate(uint32_t initialValue);

uint16_t semaphoreOpen(uint16_t id);

uint16_t semaphoreClose(uint16_t id);

void semaphoreBinaryPost(uint16_t id);

void semaphoreBinaryWait(uint16_t id);

void semaphorePost(uint16_t id);

void semaphoreWait(uint16_t id);

#endif // SEMAPHORES_H_