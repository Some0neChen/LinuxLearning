#include "process_sem_comm.h"
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>

using namespace std;

sem_t *sem_write;
sem_t *sem_read;

void clean(int signum) // 🧹 扫地僧：负责清理战场
{
    cout << "\n[收到信号 " << signum << "] 正在清理资源并退出..." << endl;
    if (sem_write != SEM_FAILED) {
        sem_close(sem_write);
    }
    if (sem_read != SEM_FAILED) {
        sem_close(sem_read);
    }

    sem_unlink("/notebook_wr_sem");
    sem_unlink("/notebook_rd_sem");
    shm_unlink("/notebook");

    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGINT, clean); // 注册 Ctrl+C 信号处理

    int fd = shm_open("/notebook", O_CREAT | O_RDWR , 0664);
    if (fd < 0) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, sizeof(Notebook));
    void *data = mmap(nullptr, sizeof(Notebook), PROT_READ | PROT_WRITE,
        MAP_SHARED, fd, 0);
    Notebook* comm = static_cast<Notebook*>(data);
    comm->id = 0;
    memset(comm->message, 0, 256);
    sem_write = sem_open("/notebook_wr_sem", O_CREAT, 0666, 1);
    sem_read = sem_open("/notebook_rd_sem", O_CREAT, 0666, 0);
    ostringstream msgOs;

    while(true) {
        sem_wait(sem_write);

        ++comm->id;
        cout << "[写 ✍😊]开始记第 " << comm->id << " 天的日志..." << endl;
        msgOs << "[" << setw(2) << left << comm->id << "] " << "开始地过了一天~" << endl;
        strcpy(comm->message, msgOs.str().c_str());
        msgOs.clear();
        msgOs.str("");

        sem_post(sem_read);
        usleep(1000000);
    }

    exit(EXIT_SUCCESS);
}