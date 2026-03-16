#pragma once

#include <algorithm>
#include <deque>
#include <map>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <sys/socket.h>
#include <unordered_map>
#include <vector>

using TASK_UINIT = std::pair<std::shared_ptr<std::vector<char>>, sockaddr>;

class ChatSever
{
    int *fd;
    std::unordered_map<std::string, sockaddr> client_book;
    std::deque<TASK_UINIT> msg_task_q;
    std::vector<pthread_t> worker_threads;
    pthread_mutex_t mutex_;
    pthread_mutex_t book_mutex_;
    sem_t sem;
    bool running_state;

    void msg_sever(TASK_UINIT);
    void reg_handler(std::shared_ptr<std::vector<char>>, sockaddr*);
    void msg_handler(std::shared_ptr<std::vector<char>>, sockaddr*);
    void exit_handler(std::shared_ptr<std::vector<char>>, sockaddr*);

    using handler_func_t = void (ChatSever::*)(std::shared_ptr<std::vector<char>>, sockaddr*);
    std::vector<handler_func_t> handler_vec = {
        &ChatSever::reg_handler,
        &ChatSever::msg_handler,
        &ChatSever::exit_handler
    };

public:
    ChatSever(int* fd, unsigned char thread_num = 10) : fd(fd) {
        worker_threads.resize(thread_num);
        pthread_mutex_init(&mutex_, nullptr);
        pthread_mutex_init(&book_mutex_, nullptr);
        sem_init(&sem, PTHREAD_PROCESS_SHARED, 0);
        running_state = true;
        for (auto& it : worker_threads) {
            pthread_create(&it, nullptr, worker_wraper, this);
        }
    }
    ~ChatSever() {
        running_state = false;
        std::for_each(worker_threads.begin(), worker_threads.end(), [this](const pthread_t &t){sem_post(&this->sem);});
        std::for_each(worker_threads.begin(), worker_threads.end(), [this](const pthread_t &t){
            pthread_join(t, nullptr);
        });
        sem_destroy(&this->sem);
        pthread_mutex_destroy(&this->mutex_);
    }
    ChatSever(const ChatSever&) = delete;
    ChatSever(ChatSever&&) = delete;
    ChatSever& operator=(const ChatSever&) = delete;
    ChatSever& operator=(ChatSever&) = delete;

    void add_task(TASK_UINIT);
    void worker_routine() {
        while (running_state) {
            sem_wait(&this->sem);
            pthread_mutex_lock(&this->mutex_);
            if (msg_task_q.empty()) {
                pthread_mutex_unlock(&this->mutex_);
                continue;
            }
            auto task = msg_task_q.front();
            msg_task_q.pop_front();
            pthread_mutex_unlock(&this->mutex_);
            this->msg_sever(task);
        }
    }

    static void* worker_wraper(void* args) {
        auto server = static_cast<ChatSever*>(args);
        server->worker_routine();
        return nullptr;
    }
};