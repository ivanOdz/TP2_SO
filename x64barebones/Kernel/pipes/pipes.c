#include <libc.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>

FifoBuffer * pipesList = NULL;
static uint64_t pipesInUse;


void defineDefaultFileDescriptors(PCB * process);
