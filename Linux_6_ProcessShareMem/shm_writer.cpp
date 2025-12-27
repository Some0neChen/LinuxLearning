#include "process_share_mem.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main()
{
    /* 第一步  ftok */
    const char* fileName = "./temp";
    key_t key = ftok(fileName, 1);

    /* 第二步 shmget */
    // 内核是以“页（4KB）”为单位分配的，如果申请 1字节，内核也会占 4096字节，但只能用1字节。
    int shm_id = shmget(key, sizeof(MaidStatusE), IPC_CREAT | 0664);
    if (shm_id <= 0) {
        perror("获取内存ID错误.");
        exit(EXIT_FAILURE);
    }

    /* 第三步 shmat */
    void* shm_addr = shmat(shm_id, nullptr, 0); // 第二个参数通常写空让系统分配，第三个0是读写，SHM_RDONLY是只读
    if (!shm_addr) {
        perror("分配内存失败.");
        exit(EXIT_FAILURE);
    }

    /* 第四步 转成要写的结构体，开始通讯 */
    pid_t pid = getpid();
    string info{};
    MaidStatusE* maid = static_cast<MaidStatusE*>(shm_addr);
    char msgInfo[512];
    while (true) {
        cout << "----------------******----------------" << endl;
        sprintf(msgInfo, "[%d] 发来%s信息, 请求%s. 此时对方兴奋度♥%d♥.",
            maid->pid, maid->command, maid->gesture, maid->excitement);
            cout << msgInfo << endl;
        cout << "----------------******----------------" << endl;
        maid->pid = pid;
        cout << "[" << pid << "] 申请动作: ";
        getline(cin, info);
        strcpy(maid->command, info.c_str());
        cout << "[" << pid << "] 执行内容: ";
        getline(cin, info);
        strcpy(maid->gesture, info.c_str());
        cout << "[" << pid << "] 快感程度: ";
        cin >> maid->excitement;
        cin.get();
        cout << "----------------******----------------" << endl;
        sleep(1);
    }
    
    /* 第五步 断开与内存的链接，使其内容的引用计数减一 */
    shmdt(shm_addr);

    /* 销毁内存 */
    shmctl(shm_id, IPC_RMID, nullptr);
}