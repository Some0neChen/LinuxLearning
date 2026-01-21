#include "thread_rwlock.h"
#include <pthread.h>
#include <unistd.h>

using namespace std;

pthread_rwlock_t* g_rwlock;

void* reader(void* args)
{
    pthread_rwlock_rdlock(g_rwlock);
    cout << "+++ 线程 [" << pthread_self() << "] 正在读取数据!!" << endl;
    usleep(1000000); // 模拟读操作耗时
    cout << "+++ 线程 [" << pthread_self() << "] 读取完毕" << endl;
    pthread_rwlock_unlock(g_rwlock);
    //usleep(300000); // 模拟读操作间隙，给予写操作插入的机会
    return nullptr;
}

void* writer(void* args)
{
    pthread_rwlock_wrlock(g_rwlock);
    cout << "+++ 线程 [" << pthread_self() << "] 正在写入数据!!" << endl;
    usleep(1000000); // 模拟写操作耗时
    cout << "+++ 线程 [" << pthread_self() << "] 写入完毕" << endl;
    pthread_rwlock_unlock(g_rwlock);
    usleep(1600000);
    return nullptr;
}

int main()
{
    g_rwlock = new pthread_rwlock_t;
    vector<pthread_t> pthread_vec(8, 0);
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP); // 设置写优先
    pthread_rwlock_init(g_rwlock, &attr);

    unsigned index = 0;
    for (int i = 1; i <= 5; ++i) {
        if (i % 2 == 1) {
            pthread_create(&pthread_vec.at(index++), nullptr, writer, nullptr);
        }
        pthread_create(&pthread_vec.at(index++), nullptr, reader, nullptr);
    }

    for (auto it: pthread_vec) {
        pthread_join(it, nullptr);
    }

    pthread_rwlock_destroy(g_rwlock);
    delete(g_rwlock);
    return 0;
}