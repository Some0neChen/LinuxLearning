#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <stack>
#include <string>
#include <sys/socket.h>
#include <unordered_map>
#include <utility>
#include <vector>

using TASK_UNIT = std::pair<std::vector<char>*, sockaddr*>;

class BufferPool
{
    pthread_mutex_t mutex_;
    std::vector<std::unique_ptr<std::vector<char>>> msg_pool_; // 消息内存实际存放位置
    std::vector<std::unique_ptr<sockaddr>> addr_pool;           // 地址信息内存实际存放位置
    std::stack<TASK_UNIT, std::deque<TASK_UNIT>> free_stack_; // 可用内存地址栈
    std::queue<TASK_UNIT, std::deque<TASK_UNIT>> used_stack_; // 待处理buffer栈

    void enlarge_capacity() {
        std::cout << "Capacity enlarged!" << std::endl;
        size_t old_cap = msg_pool_.size();
        size_t new_cap = old_cap + (old_cap >> 1);
        msg_pool_.reserve(new_cap);
        addr_pool.reserve(new_cap);
        for (auto i = old_cap; i < new_cap; ++i) {
            msg_pool_.push_back(std::make_unique<std::vector<char>>(1024));
            addr_pool.push_back(std::make_unique<sockaddr>());
            free_stack_.push(std::make_pair(msg_pool_.back().get(), addr_pool.back().get()));
        }
    }

public:
    BufferPool() {
        msg_pool_.reserve(10);
        addr_pool.reserve(10);
        for (int i = 0; i < 10; ++i) {
            msg_pool_.push_back(std::make_unique<std::vector<char>>(1024));
            addr_pool.push_back(std::make_unique<sockaddr>());
            free_stack_.push(std::make_pair(msg_pool_.back().get(), addr_pool.back().get()));
        }
        pthread_mutex_init(&mutex_, nullptr);
    }

    BufferPool(const BufferPool&) = delete;
    BufferPool& operator=(const BufferPool&) = delete;
    BufferPool(BufferPool&&) = delete;
    BufferPool& operator=(BufferPool&&) = delete;
    ~BufferPool() {
        pthread_mutex_destroy(&mutex_);
    }

    TASK_UNIT get_empty_buffer() {
        pthread_mutex_lock(&mutex_);
        if (free_stack_.empty()) {
            enlarge_capacity();
        }
        auto avail_buffer = free_stack_.top();
        free_stack_.pop();
        pthread_mutex_unlock(&mutex_);
        return avail_buffer;
    }

    void recycle_buffer(TASK_UNIT& task) {
        pthread_mutex_lock(&mutex_);
        free_stack_.push(task);
        task.first = nullptr;
        task.second = nullptr;
        pthread_mutex_unlock(&mutex_);
        return;
    }

    void save_task_buffer(TASK_UNIT& task) {
        pthread_mutex_lock(&mutex_);
        used_stack_.push(task);
        pthread_mutex_unlock(&mutex_);
        return;
    }

    std::shared_ptr<TASK_UNIT> get_task_buffer() {
        pthread_mutex_lock(&mutex_);
        if (used_stack_.empty()) {
            pthread_mutex_unlock(&mutex_);
            return nullptr;
        }
        auto task = used_stack_.front();
        used_stack_.pop();
        pthread_mutex_unlock(&mutex_);
        return std::shared_ptr<TASK_UNIT>(new TASK_UNIT(task), [this](TASK_UNIT* task){
            this->recycle_buffer(*task);
            delete task;
        });
    }
};

class ChatServer
{
    int *fd;
    std::unordered_map<std::string, sockaddr> client_book;
    BufferPool task_buf_pool;
    std::vector<pthread_t> worker_threads;
    pthread_mutex_t book_mutex_;
    sem_t sem;
    
    void msg_server(std::shared_ptr<TASK_UNIT>);
    void reg_handler(std::shared_ptr<TASK_UNIT>);
    void msg_handler(std::shared_ptr<TASK_UNIT>);
    void exit_handler(std::shared_ptr<TASK_UNIT>);

    using handler_func_t = void (ChatServer::*)(std::shared_ptr<TASK_UNIT>);
    std::vector<handler_func_t> handler_vec = {
        &ChatServer::reg_handler,
        &ChatServer::msg_handler,
        &ChatServer::exit_handler
    };

public:
    static std::atomic<bool> running_state;
    ChatServer(int* fd, unsigned char thread_num = 10) : fd(fd), task_buf_pool(BufferPool()) {
        worker_threads.resize(thread_num);
        pthread_mutex_init(&book_mutex_, nullptr);
        sem_init(&sem, PTHREAD_PROCESS_SHARED, 0);
        for (auto& it : worker_threads) {
            pthread_create(&it, nullptr, worker_wraper, this);
        }
    }
    ~ChatServer() {
        std::for_each(worker_threads.begin(), worker_threads.end(), [this](const pthread_t &t){sem_post(&this->sem);});
        std::for_each(worker_threads.begin(), worker_threads.end(), [this](const pthread_t &t){
            pthread_join(t, nullptr);
        });
        sem_destroy(&this->sem);
        pthread_mutex_destroy(&this->book_mutex_);
    }
    ChatServer(const ChatServer&) = delete;
    ChatServer(ChatServer&&) = delete;
    ChatServer& operator=(const ChatServer&) = delete;
    ChatServer& operator=(ChatServer&) = delete;

    void add_task(TASK_UNIT& task);
    void worker_routine() {
        while (ChatServer::running_state.load()) {
            sem_wait(&this->sem);
            auto task = task_buf_pool.get_task_buffer();
            this->msg_server(task);
        }
    }

    TASK_UNIT get_empty_buffer() {
        return task_buf_pool.get_empty_buffer();
    }

    void recycle_unsing_buffer(TASK_UNIT& task) {
        task_buf_pool.recycle_buffer(task);
    }
 
    static void* worker_wraper(void* args) {
        auto server = static_cast<ChatServer*>(args);
        server->worker_routine();
        return nullptr;
    }
};