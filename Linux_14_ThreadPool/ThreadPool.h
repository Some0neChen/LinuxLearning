#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__


#include <iostream>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <functional>

using UINT32 = unsigned int;
using Task = std::function<void(void)>;

class ThreadPool {
private:
    // 定义任务类型：这是一个通用的函数包装器
    std::queue<Task> taskList;

    // --- 同步工具区 ---
    pthread_mutex_t mutex_;     // 保护任务列表
    sem_t task_sem_;            // 信号量，统计当前有多少待执行任务

    std::vector<pthread_t> threads_;
    bool running_;    // 控制线程池停止的开关

public:
    ThreadPool(int threadNums) : running_(true) {
        // 初始化锁和无名信号量 (pshared=0/PTHREAD_PROCESS_PRIVATE)
        pthread_mutex_init(&mutex_, nullptr);
        sem_init(&task_sem_, PTHREAD_PROCESS_PRIVATE, 0);

        threads_.resize(threadNums);
        for (auto& it : threads_) {
            pthread_create(&it, nullptr, worker_wrapper, this);
        }
    }

    ~ThreadPool() {
        running_ = false; // 关闭线程池
        for (auto& it : threads_) {
            sem_post(&task_sem_); // 全部唤醒，处理前面running_为true后进入到sem_wait的线程
        }
        for (auto& it : threads_) {    // 先退线程
            pthread_join(it, nullptr);
        }

        // 销毁锁和信号量
        sem_destroy(&task_sem_);
        pthread_mutex_unlock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }

    void addTask(Task task) {
        pthread_mutex_lock(&mutex_);
        taskList.push(task);
        pthread_mutex_unlock(&mutex_);
        sem_post(&task_sem_);
    }

private:
    void worker_routine() {
        while (running_) {
            sem_wait(&task_sem_);
            pthread_mutex_lock(&mutex_);
            if (taskList.empty()) {
                pthread_mutex_unlock(&mutex_);
                continue;
            }
            Task task = taskList.front();
            taskList.pop();
            pthread_mutex_unlock(&mutex_);
            task();
        }
    }

    static void* worker_wrapper(void* arg) {
        ThreadPool* pool = static_cast<ThreadPool*>(arg);
        pool->worker_routine();
        return nullptr;
    }
};

#endif /* __THREAD_POOL_H__ */