#include "process_share_mem.h"

int main()
{
    /* 第一步  ftok */
    const char* fileName = "./temp";
    key_t key = ftok(fileName, 1);

    /* 第二步 shmget */
    // 内核是以“页（4KB）”为单位分配的，如果申请 1字节，内核也会占 4096字节，但只能用1字节。
    int shm_id = shmget(key, sizeof(MaidStatus),  0664);
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
    MaidStatus* maid = static_cast<MaidStatus*>(shm_addr);
    char msgInfo[512];
    while (true) {
        if (maid->readFlag() || maid->getPid() == pid) {
            // 对方还没读完，等会儿
            usleep(100000); // 0.1秒
            continue;
        }
        maid->switchFlag();
        if (maid->assertQuit()) {
            cout << "[" << pid << "] 收到 [" << maid->getPid() << "]退出信号, 结束通讯." << endl;
            maid->switchFlag();
            break;
        }
        cout << "----------------******----------------" << endl;
        maid->show();
        cout << "----------------******----------------" << endl;
        maid->set(cin);
        cout << "----------------******----------------" << endl;
        maid->switchFlag();
        if (maid->assertQuit()) {
            cout << "[" << pid << "] 发送了退出信号, 结束通讯." << endl;
            break;
        }
        sleep(1);
    }
    
    /* 第五步 断开与内存的链接，使其内容的引用计数减一 */
    shmdt(shm_addr);
}