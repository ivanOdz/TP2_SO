// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <semaphores.h>

void my_sem_init(sem *s, int value) {
  s->value = value;
}

extern int _xchg(int *lock, int value);

void acquire(int *lock) {
  while (_xchg(lock, 1) != 0);
}

void release(int *lock) {
  _xchg(lock, 0);
}

int lock = 0;

void my_sem_wait(sem *s) {
  while (1) {
    acquire(&lock);
    if (s->value > 0) {
      s->value--;
      release(&lock);
      break;
    }
    release(&lock);
  }
}

void my_sem_post(sem *s) {
  acquire(&lock);
  s->value++;
  release(&lock);
}
