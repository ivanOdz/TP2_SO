#include "./../Include/tests.h"
#include <libc.h>
// #define USE_BINARY_SEMAPHORE

void testRace(uint64_t *shared) {

    int64_t copy = *shared;
    yield();
    copy += 1;
    // yield();
    *shared = copy;
}

int testProcessOfIncrementalLawOfEntropy(int argc, char **argv) {

    uint64_t *shared;
    uint64_t cycles;
    uint16_t idSemaphore;

    if (argc < 3) {
        return -1;
    }
    shared = (uint64_t*)argv[1];
    cycles = stringToInt(argv[2], strlen(argv[2]));
    idSemaphore = stringToInt(argv[3], strlen(argv[3]));

    if (!semOpen(idSemaphore)) {
        return -1;
    }
    for (uint64_t cont=0; cont < cycles; cont++) {

        if (idSemaphore) {
            #ifdef USE_BINARY_SEMAPHORE
            semBinaryWait(idSemaphore);
            testRace(shared);
            semBinaryPost(idSemaphore);
            #else
            semWait(idSemaphore);
            testRace(shared);
            semPost(idSemaphore);
            #endif
        }
        else {
            testRace(shared);
        }
    }
}

int testSemaphores(int argc, char **argv) { // Cant incrementos, cant procesos, usa semaforos o no

    static int64_t shared;
    uint64_t cycles;
    PID_t nProcesses = 2;
    PID_t *processes;
    char *argvForProcesses[4];
    uint16_t idSemaphore;
    uint8_t useSemaphore = 0;
    char cyclesString[24];
    char idSemaphoreString[24];

    if (argc < 4) {
        return -1;
    }
    cycles = stringToInt(argv[1], strlen(argv[1]));
    nProcesses = stringToInt(argv[2], strlen(argv[2]));
    useSemaphore = stringToInt(argv[3], strlen(argv[3]));

    if (cycles == 0) {
        cycles = 65535;
    }
    if (nProcesses == 0) {
        nProcesses = 2;
    }
    if (useSemaphore) {
        idSemaphore = semCreate(1);
        if (!idSemaphore) {
            return -1;
        }
    }

    processes = malloc(nProcesses * sizeof(PID_t));
    argvForProcesses[0] = "test_sem_process";
    argvForProcesses[1] = (char*)&shared;
    argvForProcesses[2] = cyclesString;
    argvForProcesses[3] = idSemaphoreString;

    for (PID_t cont=0; cont < nProcesses; cont++) {
        // processes[cont] = execute(testProcessOfIncrementalLawOfEntropy, argvForProcesses, NULL);
    }
    ReturnStatus status;
    for (PID_t cont=0; cont < nProcesses; cont++) {
        waitpid(processes[cont], &status);
    }

    if (useSemaphore) {
        semClose(idSemaphore);
    }
    printf("VALUE: ");
    // printint +\n
}