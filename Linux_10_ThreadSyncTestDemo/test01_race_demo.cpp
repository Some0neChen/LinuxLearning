#include "threadSyncTestDemo.h"
#include <chrono>
#include <pthread.h>

pthread_mutex_t* g_mutex(new pthread_mutex_t);

void* workder(void *args)
{
    auto start = std::chrono::steady_clock::now();
    int *g_count = static_cast<int*>(args);
    std::cout << "我现在来取把锁 " << std::endl;
    for (int i = 0; i < 100000000; ++i) {
        pthread_mutex_lock(g_mutex);
        (*g_count)++;
        pthread_mutex_unlock(g_mutex);
    }
    std::cout << "我解锁 " << std::endl;
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
    pthread_mutex_init(g_mutex, nullptr);

    pthread_create(&th1, nullptr, workder, g_count);
    pthread_create(&th2, nullptr, workder, g_count);

    pthread_join(th1, nullptr);
    pthread_join(th2, nullptr);

    std::cout << "Counting result: " << *g_count << std::endl;
    delete g_count;
    pthread_mutex_destroy(g_mutex);
    delete g_mutex;

    return 0;
}