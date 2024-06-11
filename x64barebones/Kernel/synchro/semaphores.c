// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <semaphores.h>

static sem semaphores[MAX_SEMAPHORES] = {0};

void acquire(int *lock) {
  while (_xchg(lock, 1) != 0);
}

void release(int *lock) {
  _xchg(lock, 0);
}

int16_t my_sem_init(int initialValue) {

  for (int cont=0; cont < MAX_SEMAPHORES; cont++) {
    if (semaphores[cont].inUse == 0) {
      semaphores[cont].value = initialValue;
      semaphores[cont].inUse = 1;
      return cont;
    }
  }
  return MAX_SEMAPHORES-1;
}

int16_t my_sem_destroy(uint16_t id) {

  if (id >= MAX_SEMAPHORES) {
    return 0;
  }
  semaphores[id].lock = 0;
  semaphores[id].value = 0;
  semaphores[id].inUse = 0;

  return 1;
}

void my_sem_wait(uint16_t id) {

  if (id >= MAX_SEMAPHORES || !semaphores[id].inUse) {
    return;
  }

  while (1) {

    acquire(&semaphores[id].lock);
    if (semaphores[id].value > 0) {
      semaphores[id].value--;
      release(&semaphores[id]);
      break;
    }
    release(&semaphores[id].lock);
    schedyield();
  }

}

void my_sem_post(uint16_t id) {

  if (id >= MAX_SEMAPHORES || !semaphores[id].inUse) {
    return;
  }

  acquire(&semaphores[id].lock);
  semaphores[id].value++;
  release(&semaphores[id].lock);
}
