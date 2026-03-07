#pragma once

#include "TavernaClub.h"
#include <algorithm>
#include <memory>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <deque>
#include <unordered_map>
#include <vector>

using UINT32 = unsigned int;
using USHORT = unsigned short;

struct Visitor {
    int fd;
    std::shared_ptr<sockaddr_in> addr;
    UINT32 club_id;
};

UINT32 get_club_id(int fd);

class TavernaServer
{
    std::unordered_map<UINT32, std::shared_ptr<TavernaClub>> chat_room_map;
    pthread_mutex_t mutex_;
    sem_t sem_;
    bool running_state_;
    std::vector<pthread_t> waiter_threads_;
    std::deque<std::shared_ptr<Visitor>> visitor_queue_;

    void check_club_using(UINT32 club_id) {
        pthread_mutex_lock(&mutex_);
        if (chat_room_map[club_id]->get_member_count() == 0) {
            chat_room_map.erase(club_id);
        }
        pthread_mutex_unlock(&mutex_);
    }

    void waiter_routine() {
        while (running_state_) {
            sem_wait(&sem_);
            pthread_mutex_lock(&mutex_);
            if (visitor_queue_.empty()) {
                pthread_mutex_unlock(&mutex_);
                continue;
            }
            std::shared_ptr<Visitor> v = visitor_queue_.front();
            visitor_queue_.pop_front();
            pthread_mutex_unlock(&mutex_);
            v->club_id = get_club_id(v->fd);
            if (v->club_id == 0) {
                continue;
            }
            if (chat_room_map.find(v->club_id) == chat_room_map.end()) {
                chat_room_map.emplace(v->club_id, std::make_shared<TavernaClub>());
            }
            chat_room_map[v->club_id]->add_member(v->fd, v->addr);
            chat_room_map[v->club_id]->receive_msg(v->fd);
            check_club_using(v->club_id);
        }
    }
public:
    TavernaServer() : running_state_(true) {
        pthread_mutex_init(&mutex_, nullptr);
        sem_init(&sem_, PTHREAD_PROCESS_SHARED, 0);
        waiter_threads_.resize(16);
        for_each(waiter_threads_.begin(), waiter_threads_.end(), [this](pthread_t &t) {
            pthread_create(&t, nullptr, taverna_waiter_server_wrapper, this);
            std::cout << "服务员编号[" << (t % 1000) << "]正在等待顾客上门..." << std::endl;
        });
    }

    TavernaServer(const TavernaServer&) = delete;
    TavernaServer& operator=(const TavernaServer&) = delete;
    ~TavernaServer() {
        running_state_ = false;
        for (auto &it : waiter_threads_) {
            sem_post(&sem_);
        }
        std::for_each(waiter_threads_.begin(), waiter_threads_.end(), [](const pthread_t &t) {
            pthread_join(t, nullptr);
        });
        sem_destroy(&sem_);
        pthread_mutex_destroy(&mutex_);
    }

    void welcome_visitor(std::shared_ptr<Visitor> v) {
        pthread_mutex_lock(&mutex_);
        visitor_queue_.push_back(v);
        pthread_mutex_unlock(&mutex_);
        sem_post(&sem_);
    }

    static void* taverna_waiter_server_wrapper(void* arg) {
        TavernaServer *server = static_cast<TavernaServer*>(arg);
        server->waiter_routine();
        return nullptr;
    }
};