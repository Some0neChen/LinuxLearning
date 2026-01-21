#include "thread_rwlock.h"
#include <chrono>
#include <pthread.h>

pthread_spinlock_t* g_spin(new pthread_spinlock_t);

void* worker(void *args)
{
    auto start = std::chrono::steady_clock::now();
    int *g_count = static_cast<int*>(args);
    std::cout << "我 [" << pthread_self() << "] 来开始处理数据" << std::endl;
    for (int i = 0; i < 100000000; ++i) {
        pthread_spin_lock(g_spin);
        (*g_count)++;
        pthread_spin_unlock(g_spin);
    }
    std::cout << "我 [" << pthread_self() << "] 处理数据完毕" << std::endl;
    auto end = std::chrono::steady_clock::now();
    std::cout << "Thread ID: " << pthread_self() << ", Time taken: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us" << std::endl;
    pthread_exit(nullptr);
    return nullptr;
}

int main()
{
    pthread_t th1, th2;
    int *g_count = new int(0);
    pthread_spin_init(g_spin, PTHREAD_PROCESS_PRIVATE); // 为什么这里属性不能用nullptr

    pthread_create(&th1, nullptr, worker, g_count);
    pthread_create(&th2, nullptr, worker, g_count);

    pthread_join(th1, nullptr);
    pthread_join(th2, nullptr);

    std::cout << "Counting result: " << *g_count << std::endl;
    delete g_count;
    pthread_spin_destroy(g_spin);
    delete g_spin;

    return 0;
}