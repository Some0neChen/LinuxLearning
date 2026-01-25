#ifndef __PROCESS_SEM_COMM_H__
#define __PROCESS_SEM_COMM_H__

#include <iostream>
#include <semaphore.h>
#include <sys/unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>   // mmap, munmap
#include <fcntl.h>      // O_CREAT, O_RDWR (用于有名信号量)
#include <sys/wait.h>   // wait
#include <iomanip>
#include <sstream>
#include "cstring"

struct ProcessSemComm {
    int id;
    char message[256];
    sem_t sem_write;
    sem_t sem_read;

    ProcessSemComm() {
    }
    ~ProcessSemComm() {
        sem_destroy(&sem_write);
        sem_destroy(&sem_read);
        memset(message, 0, 256);
    }
};

struct Notebook {
    int id;
    char message[256];
    ~Notebook() {
        memset(message, 0, 256);
    }
};

#endif /* __PROCESS_SEM_COMM_H__ */