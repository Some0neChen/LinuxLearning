#include "ChatSever.h"
#include "udp_pub_api_pub.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <vector>

using namespace std;

void ChatSever::msg_sever(TASK_UINIT task)
{
    if (task.first.get() == nullptr) {
        cerr << "消息为空" << endl;
        return;
    }
    (this->*handler_vec[static_cast<udp_msg_header*>(static_cast<void*>(task.first->data()))->type])(task.first, &task.second);
    return;
}

void ChatSever::reg_handler(std::shared_ptr<std::vector<char>> data, sockaddr* client_addr)
{
    udp_msg_header* header = reinterpret_cast<udp_msg_header*>(data->data());
    pthread_mutex_lock(&this->book_mutex_);
    this->client_book[header->name] = *client_addr;
    pthread_mutex_unlock(&this->book_mutex_);
    return;
}

void ChatSever::exit_handler(std::shared_ptr<std::vector<char>> data, sockaddr* client_addr)
{
    string msg = "exit";
    socklen_t len = sizeof(sockaddr);
    sendto(*this->fd, msg.data(), msg.size(), 0, client_addr, len);
    pthread_mutex_lock(&this->book_mutex_);
    this->client_book.erase((reinterpret_cast<udp_msg_header*>(data->data())->name));
    pthread_mutex_unlock(&this->book_mutex_);
    return;
}

void ChatSever::msg_handler(std::shared_ptr<std::vector<char>> data, sockaddr* client_addr)
{
    udp_msg_header* header = reinterpret_cast<udp_msg_header*>(data->data());
    pthread_mutex_lock(&this->book_mutex_);
    if (this->client_book.find(header->name) == this->client_book.end()) {
        string msg = "消息接收者未在服务器中注册，无法发送~\n";
        socklen_t len = sizeof(sockaddr);
        sendto(*this->fd, msg.data(), msg.size(), 0, client_addr, len);
        cerr << "用户不存在" << endl;
        return;
    }
    auto target_addr = this->client_book[header->name];
    pthread_mutex_unlock(&this->book_mutex_);
    string msg_buf = init_msg(client_addr);
    unsigned short sender_len = msg_buf.size();
    msg_buf.resize(sender_len + header->msg_len);
    memcpy(msg_buf.data() + sender_len, data->data() + sizeof(udp_msg_header), header->msg_len);
    socklen_t len = sizeof(sockaddr);
    auto ret = sendto(*this->fd, msg_buf.data(), msg_buf.size(), 0, &target_addr, len);
    if (ret < 0) {
        cerr << "消息发送失败" << endl;
    }
    return;
}

void ChatSever::add_task(TASK_UINIT task)
{
    pthread_mutex_lock(&this->mutex_);
    this->msg_task_q.push_back(task);
    pthread_mutex_unlock(&this->mutex_);
    sem_post(&this->sem);
    return;
}

int main(int argc, char* argv[])
{
    auto fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        cerr << "socket创建失败" << endl;
        return -1;
    }
    sockaddr_in sever_addr;
    sever_addr.sin_family = AF_INET;
    sever_addr.sin_port = htons(8888);
    inet_pton(AF_INET, "192.168.182.128", &sever_addr.sin_addr);
    auto ret = bind(fd, reinterpret_cast<sockaddr*>(&sever_addr), sizeof(sockaddr_in));
    if (ret < 0) {
        cerr << "bind失败" << endl;
        return -1;
    }
    ChatSever sever(&fd);
    char buf[1024];
    sockaddr client_addr;
    socklen_t len = sizeof(sockaddr);
    ssize_t recv_len;
    while ((recv_len = recvfrom(fd, buf, 1024, 0, &client_addr, &len)) > 0) {
        sever.add_task(make_pair(make_shared<vector<char>>(buf, buf + recv_len), client_addr));
    }
    close(fd);
}