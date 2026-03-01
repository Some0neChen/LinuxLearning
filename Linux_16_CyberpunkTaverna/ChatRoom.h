#include <iostream>
#include <unordered_map>
#include <netinet/in.h>
#include <pthread.h>

class ChatRoom {
    ChatRoom() {
        pthread_mutex_init(&mutex_, NULL);
    };
    ChatRoom& operator=(const ChatRoom&) = delete;
    ChatRoom(const ChatRoom&) = delete;

    void addMember(int fd, sockaddr_in* addr) {
        pthread_mutex_lock(&mutex_);
        // 向聊天室里的所有成员发送消息
        for (auto &member : chat_members) {
            
        }
        chat_members.emplace(fd, addr);
    }


private:
    std::unordered_map<int, sockaddr_in*> chat_members;
    pthread_mutex_t mutex_;
};