#include "ChatServer.h"
#include "udp_pub_api_pub.h"
#include <arpa/inet.h>
#include <atomic>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace std;

atomic<bool> ChatServer::running_state = atomic<bool>(true);

void signal_handler(int sig)
{
    ChatServer::running_state.store(false);
    return;
}

void ChatServer::msg_server(std::shared_ptr<TASK_UNIT> task)
{
    if (task->first->empty()) {
        cerr << "消息为空" << strerror(errno) << endl;
        return;
    }
    (this->*handler_vec[static_cast<udp_msg_header*>(static_cast<void*>(task->first->data()))->type])(task);
    return;
}

void ChatServer::reg_handler(std::shared_ptr<TASK_UNIT> task)
{
    udp_msg_header* header = reinterpret_cast<udp_msg_header*>(task->first->data());
    pthread_mutex_lock(&this->book_mutex_);
    this->client_book[header->name] = *task->second;
    pthread_mutex_unlock(&this->book_mutex_);
    return;
}

void ChatServer::exit_handler(std::shared_ptr<TASK_UNIT> task)
{
    string msg = "exit";
    socklen_t len = sizeof(sockaddr);
    sendto(*this->fd, msg.data(), msg.size(), 0, task->second, len);
    pthread_mutex_lock(&this->book_mutex_);
    this->client_book.erase((reinterpret_cast<udp_msg_header*>(task->first->data())->name));
    pthread_mutex_unlock(&this->book_mutex_);
    return;
}

void ChatServer::msg_handler(std::shared_ptr<TASK_UNIT> task)
{
    udp_msg_header* header = reinterpret_cast<udp_msg_header*>(task->first->data());
    pthread_mutex_lock(&this->book_mutex_);
    if (this->client_book.find(header->name) == this->client_book.end()) {
        string msg = "消息接收者未在服务器中注册，无法发送~\n";
        socklen_t len = sizeof(sockaddr);
        sendto(*this->fd, msg.data(), msg.size(), 0, task->second, len);
        cerr << "用户不存在" << strerror(errno) << endl;
        pthread_mutex_unlock(&this->book_mutex_);
        return;
    }
    auto target_addr = this->client_book[header->name];
    pthread_mutex_unlock(&this->book_mutex_);
    string msg_buf = init_msg(task->second);
    unsigned short sender_len = msg_buf.size();
    msg_buf.resize(sender_len + header->msg_len);
    memcpy(msg_buf.data() + sender_len, task->first->data() + sizeof(udp_msg_header), header->msg_len);
    socklen_t len = sizeof(sockaddr);
    auto ret = sendto(*this->fd, msg_buf.data(), msg_buf.size(), 0, &target_addr, len);
    if (ret < 0) {
        cerr << "消息发送失败" << strerror(errno) << endl;
    }
    return;
}

void ChatServer::add_task(TASK_UNIT& task)
{
    this->task_buf_pool.save_task_buffer(task);
    sem_post(&this->sem);
    return;
}

int main(int argc, char* argv[])
{
    // 守护进程配置及信号处理挂接
    signal(SIGINT, signal_handler);     // ctrl + c
    signal(SIGTERM, signal_handler);    // kill process
    const char* logFilePath = "/home/some0nechen/文档/code/LinuxLearning/Linux_17_UDPConversation/log/server_running.log";
    auto log_fd = open(logFilePath, O_WRONLY | O_APPEND | O_CREAT, 0664);
    if (log_fd < 0) {
        cerr << "日志文件打开失败" << strerror(errno) << endl;
        return -1;
    }
    dup2(log_fd, STDOUT_FILENO);
    dup2(log_fd, STDERR_FILENO);
    if (daemon(0, 0) == -1) {
        cerr << "守护进程创建失败" << strerror(errno) << endl;
        return -1;
    }


    // 服务器端口简历
    auto socket_d = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_d < 0) {
        cerr << "socket创建失败" << strerror(errno) << endl;
        return -1;
    }
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    inet_pton(AF_INET, "192.168.182.128", &server_addr.sin_addr);
    auto ret = bind(socket_d, reinterpret_cast<sockaddr*>(&server_addr), sizeof(sockaddr_in));
    if (ret < 0) {
        cerr << "bind失败" << strerror(errno) << endl;
        return -1;
    }

    // 线程池初始化
    ChatServer server(&socket_d);
    socklen_t len;
    ssize_t recv_len;
    
    // 线程池处理链接
    while (ChatServer::running_state.load()) {
        TASK_UNIT task = server.get_empty_buffer();
        len = sizeof(sockaddr);
        if ((recv_len = recvfrom(socket_d, task.first->data(), 1024, 0, task.second, &len)) < 0) {
            server.recycle_unsing_buffer(task);
            cerr << "消息接收失败" << strerror(errno) << endl;
            continue;
        }
        server.add_task(task);
    }
    close(socket_d);
    close(log_fd);
}