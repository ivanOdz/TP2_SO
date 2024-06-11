#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

#include "libc.h"
#include <scheduler.h>

#define MAX_SEMAPHORES	 256

typedef struct {
	int value;
	bool lock;
	bool inUse;
} sem;

int16_t my_sem_init(int initialValue);
int16_t my_sem_destroy(uint16_t id);
void my_sem_wait(uint16_t id);
void my_sem_post(uint16_t id);

#endif // SEMAPHORES_H_