#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "udp_pub_api_pub.h"

using namespace std;

void *recv_msg(void *args)
{
    int* sock_fd = static_cast<int*>(args);
    ssize_t rcv_len;
    char buf[1024];
    sockaddr sever_addr;
    socklen_t len = sizeof(sever_addr);
    while ((rcv_len = recvfrom(*sock_fd, buf, 1024, 0, &sever_addr, &len)) > 0) {
        rcv_len = rcv_len >= 1024 ? 1023 : rcv_len;
        buf[rcv_len] = '\0';
        if (strcmp(buf, "exit") == 0) {
            cout << "Server has closed the connection. Exiting..." << endl;
            break;
        }
        cout << buf << endl;
        memset(buf, 0, rcv_len);
    }
    return nullptr;
}

void send_exit(char* buf, int *fd, ssize_t *send_len, sockaddr_in *sever_addr, ssize_t* sockaddr_len)
{
    udp_msg_header* header = reinterpret_cast<udp_msg_header*>(buf);
    header->type = EXIT;
    header->msg_len = sizeof(udp_msg_header);
    
    if ((*send_len = sendto(*fd, buf, sizeof(udp_msg_header), 0,
        static_cast<sockaddr*>(static_cast<void*>(sever_addr)), *sockaddr_len)) == -1) {
        cerr << "消息发送失败" << strerror(errno) << endl;
    }
    return;
}

void reg_users(int* fd, sockaddr_in* sever_addr, ssize_t* sockaddr_len)
{
    char buf[1024];
    udp_msg_header* reg_header = reinterpret_cast<udp_msg_header*>(buf);
    reg_header->type = REG;
    string name;
    cout << "请输入用户名进行注册: ";
    cin >> reg_header->name;
    ssize_t send_len;
    if ((send_len = sendto(*fd, buf, sizeof(udp_msg_header), 0, static_cast<sockaddr*>(static_cast<void*>(sever_addr)), *sockaddr_len)) == -1) {
        cerr << "消息发送失败" << endl;
    }
    return;
}

void *send_msg(void *args)
{
    int* fd = static_cast<int*>(args);
    int read_len;
    string str;
    str.resize(1024);
    size_t sub_idx;
    char buf[1024];
    ssize_t send_len, sockaddr_len = sizeof(sockaddr);
    sockaddr_in sever_addr;
    sever_addr.sin_family = AF_INET;
    sever_addr.sin_port = htons(8888);
    inet_pton(AF_INET, "192.168.182.128", static_cast<void*>(&sever_addr.sin_addr));
    reg_users(fd, &sever_addr, &sockaddr_len);

    cout << "请输入消息内容，输入exit退出聊天~" << endl;
    cout << "发送消息的格式为: <用户名> <消息内容>" << endl;
    cout << "例如: Alice 你好呀~" << endl;

    while ((read_len = read(0, str.data(), 1024))) {
        str.resize(read_len);
        str.pop_back();
        if (str == "exit") {
            send_exit(buf, fd, &send_len, &sever_addr, &sockaddr_len);
            break;
        }
        udp_msg_header* header = reinterpret_cast<udp_msg_header*>(buf);
        sub_idx = str.find_first_of(" ");
        if (sub_idx == string::npos) {
            cerr << "消息格式错误，请按照<用户名> <消息内容>的格式输入消息~" << endl;
            continue;
        }

        header->type = MSG;
        memcpy(header->name, str.data(), sub_idx);
        header->msg_len = str.size() - sub_idx - 1;
        memcpy(buf + sizeof(udp_msg_header), str.data() + sub_idx + 1, header->msg_len);

        if ((send_len = sendto(*fd, buf, sizeof(udp_msg_header) + header->msg_len, 0,
            static_cast<sockaddr*>(static_cast<void*>(&sever_addr)), sockaddr_len)) == -1) {
            cerr << "消息发送失败" << endl;
        }
        str.clear();
        str.resize(1024);
        memset(buf, 0, sizeof(buf));
    }

    return nullptr;
}


int main()
{
    auto sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        cerr << "Socket creation failed" << endl;
        exit(EXIT_FAILURE);
    }

    pthread_t recv_t, send_t;
    pthread_create(&send_t, nullptr, send_msg, static_cast<void*>(&sock_fd));
    pthread_create(&recv_t, nullptr, recv_msg, static_cast<void*>(&sock_fd));
    
    pthread_join(send_t, nullptr);
    pthread_join(recv_t, nullptr);

    close(sock_fd);
    return 0;
}