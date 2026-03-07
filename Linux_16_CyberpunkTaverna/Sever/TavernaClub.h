#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <semaphore.h>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <netinet/in.h>
#include <pthread.h>

std::string getTime();

class TavernaClub {
    std::unordered_map<int, std::shared_ptr<sockaddr_in>> chat_members;
    pthread_mutex_t mutex_;
    std::string get_ip_str(const std::shared_ptr<sockaddr_in> addr) {
        std::string ip_str;
        ip_str.append(inet_ntoa(addr->sin_addr)).append(":").append(std::to_string(ntohs(addr->sin_port)));
        return std::move(ip_str);
    }

    // 用户离开了聊天室
    void so_long_member(int fd) {
        pthread_mutex_lock(&mutex_);
        auto ip_str = get_ip_str(chat_members[fd]);
        std::cout << "服务员编号[" << (pthread_self() % 1000) << "]正在与嘉宾告别：" << ip_str << std::endl;
        std::string msg = "\n[" + getTime() + "]一起欢送嘉宾离开吧~珍重！再见了~" + ip_str + "\n";
        for (auto &member : chat_members) {
            if (member.first == fd) {
                continue;
            }
            write(member.first, msg.c_str(), msg.size());
        }
        chat_members.erase(fd);
        pthread_mutex_unlock(&mutex_);
    }

public:
    TavernaClub() {
        pthread_mutex_init(&mutex_, nullptr);
    }
    TavernaClub& operator=(const TavernaClub&) = delete;
    TavernaClub(const TavernaClub&) = delete;
    ~TavernaClub() {
        pthread_mutex_destroy(&mutex_);
    }

    // 有新的用户加入了聊天室
    void add_member(int fd, std::shared_ptr<sockaddr_in> addr) {
        pthread_mutex_lock(&mutex_);
        // 向聊天室里的所有成员发送消息
        std::string ip_str = get_ip_str(addr);
        std::cout << "服务员编号[" << (pthread_self() % 1000) << "]正在接待新嘉宾：" << ip_str << std::endl;
        std::string msg = "\n[" + getTime() + "]欢迎新成员加入酒馆！来自" + ip_str + "\n";

        for (auto &member : chat_members) {
            write(member.first, msg.c_str(), msg.size());
        }
        chat_members.emplace(fd, addr);
        pthread_mutex_unlock(&mutex_);
    }

    // 用户发送了消息，当前需要将消息发送给聊天室里的每一位成员
    void send_msg(const std::string& msg, const int &fd) {
        pthread_mutex_lock(&mutex_);
        for (auto &member : chat_members) {
            if (member.first == fd) {
                continue;
            }
            write(member.first, msg.c_str(), msg.size());
        }
        pthread_mutex_unlock(&mutex_);
    }

    // 等待用户消息，并发送给每一位成员
    void receive_msg(const int &fd) {
        char buf[1024];
        std::string msg;
        unsigned int recv_len;
        while ((recv_len = read(fd, buf, 1024)) > 0) {
            buf[recv_len - 1] = '\0'; // 忽略换行符
            if (strcmp(buf, "exit") == 0) {
                so_long_member(fd);
                break;
            }
            msg.append("\r[").append(getTime()).append("] [").append(get_ip_str(chat_members[fd])).append("] : ").append(buf).append("\n");
            send_msg(msg, fd);
            memset(buf, 0, 1024);
            msg.clear();
        }
        close(fd);
    }

    unsigned short get_member_count() {
        pthread_mutex_lock(&mutex_);
        auto count = chat_members.size();
        pthread_mutex_unlock(&mutex_);
        return count;
    }
};