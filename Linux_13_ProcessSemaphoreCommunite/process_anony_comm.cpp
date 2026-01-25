#include "process_sem_comm.h"


using namespace std;

int main()
{
    void *data = mmap(nullptr, sizeof(ProcessSemComm), PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    ProcessSemComm* comm = static_cast<ProcessSemComm*>(data);

    sem_init(&comm->sem_write, 1, 1); // 1表示进程间共享，初始值1
    sem_init(&comm->sem_read, 1, 0); // 1表示进程间共享，初始值0
    comm->id = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed.");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        while (true) {
            sem_wait(&comm->sem_read);

            cout << "[读 (❤ ω ❤)📕 ]" << comm->message << endl;
            memset(comm->message, 0, 256);

            sem_post(&comm->sem_write);
        }
    }

    ostringstream msg;
    while (true) {
        sem_wait(&comm->sem_write);

        ++comm->id;
        cout << "[写 ✍😊]开始记第 " << comm->id << " 天的日志..." << endl;
        msg << "[" << setw(2) << left << comm->id << "] " << "开始地过了一天~" << endl;
        strcpy(comm->message, msg.str().c_str());
        msg.clear();
        msg.str("");
        
        sem_post(&comm->sem_read);
        usleep(1800000);
    }

    waitpid(pid, nullptr, 0);
    munmap(comm, sizeof(ProcessSemComm));
    exit(EXIT_SUCCESS);
}
