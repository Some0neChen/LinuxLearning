#include "TavernaVisitor.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

void* receive_messages(void* arg) {
    int* socket_fd = static_cast<int*>(arg);
    char buf[1024];
    int rcv_len;
    while ((rcv_len = read(*socket_fd, buf, 1024)) > 0) {
        buf[rcv_len] = '\0';
        if (strcmp(buf, "exit\n") == 0) {
            break;
        }
        cout << buf << endl;
        memset(buf, 0, rcv_len);
    }
    return nullptr;
}

void* send_messages(void* arg) {
    int* socket_fd = static_cast<int*>(arg);
    char buf[1024];
    int send_len;
    while ((send_len = read(STDIN_FILENO, buf, 1024)) > 0) {
        buf[send_len] = '\0';
        write(*socket_fd, buf, send_len); // 忽略换行符
        if (strcmp(buf, "exit\n") == 0) {
            break;
        }
        memset(buf, 0, send_len);
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "欢迎来到赛博朋克酒馆！" << endl;
    auto socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        cerr << "Socket creation failed" << endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in server_addr;
    unsigned short connect_port = static_cast<unsigned short>(stoi(argv[2]));
    server_addr.sin_port = htons(connect_port);
    server_addr.sin_family = AF_INET;
    auto ret = inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
    if (ret <= 0) {
        cerr << "Invalid server IP address" << endl;
        exit(EXIT_FAILURE);
    }
    ret = connect(socket_fd, static_cast<sockaddr*>(static_cast<void*>(&server_addr)), sizeof(sockaddr_in));
    if (ret == -1) {
        cerr << "Socket connection failed" << endl;
        exit(EXIT_FAILURE);
    }

    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, nullptr, receive_messages, static_cast<void*>(&socket_fd));
    pthread_create(&send_thread, nullptr, send_messages, static_cast<void*>(&socket_fd));
    
    pthread_join(recv_thread, nullptr);
    pthread_join(send_thread, nullptr);
    
    close(socket_fd);
}