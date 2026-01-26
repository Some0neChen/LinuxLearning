// HusbandService.h
#pragma once

#include "Chore.h"
#include <deque>
#include "pthread.h"
#include "semaphore.h"
#include <vector>
#include <iostream>
#include <unistd.h>

class HusbandService
{
    std::deque<Chore> wife_task_queue_;
    std::vector<pthread_t> husband_threads_;
    sem_t task_sem_;
    pthread_mutex_t task_mutex_;
    bool husband_wake_status_;
    unsigned char pthread_nums_;
public:
    HusbandService(unsigned char pthread_nums_) : pthread_nums_(pthread_nums_) {
        sem_init(&task_sem_, 0, 0);
        pthread_mutex_init(&task_mutex_, nullptr);
    }

    ~HusbandService() {
        orderHusbandSleep();
        sem_destroy(&task_sem_);
        pthread_mutex_destroy(&task_mutex_);
    }

    void orderHusbandSleep() {
        husband_wake_status_ = false;
        for (int i = 0; i < husband_threads_.size(); ++i) {
            sem_post(&task_sem_);
        }
        for (auto it : husband_threads_) {
            pthread_join(it, nullptr);
        }
    }

    void orderHusbandWakeUp() {
        husband_wake_status_ = true;
        husband_threads_.resize(pthread_nums_, 0);
        for (int i = 0; i < pthread_nums_; ++i) {
            pthread_create(&husband_threads_[i], nullptr,
                husband_love_wife_action_enable, static_cast<void*>(this));
        }
    }

    void addWifeOrder(const Chore &chore) {
        pthread_mutex_lock(&task_mutex_);
        wife_task_queue_.push_back(chore);
        pthread_mutex_unlock(&task_mutex_);
        sem_post(&task_sem_);
    }

    void taskHandler(const Chore &chore, const unsigned long long &pthid) {
        std::cout << "老公的[" << pthid<< "]小心思正在完成老婆的第 #" << chore.id << " 任务： " << chore.description << std::endl;
        usleep(chore.complexity * 100000);
    }

    void husbandRoutine() {
        unsigned long long pthid = pthread_self() & 0xFFFFF;
        while(husband_wake_status_) {
            sem_wait(&task_sem_);
            pthread_mutex_lock(&task_mutex_);
            if (wife_task_queue_.empty()) {
                pthread_mutex_lock(&task_mutex_);
                continue;
            }
            Chore wife_order = wife_task_queue_.front();
            wife_task_queue_.pop_front();
            pthread_mutex_unlock(&task_mutex_);
            taskHandler(wife_order, pthid);
        }
    }

    static void* husband_love_wife_action_enable(void* args) {
        HusbandService* husbandService = static_cast<HusbandService*>(args);
        husbandService->husbandRoutine();
    }

};