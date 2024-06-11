#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

typedef struct Sem {
  int value;
} sem;

void my_sem_init(sem *s, int value);
void my_sem_wait(sem *s);
void my_sem_post(sem *s);
void acquire(int *lock);
void release(int *lock);
#endif // SEMAPHORES_H_