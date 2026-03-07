#include <arpa/inet.h>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "TavernaClub.h"
#include "TavernaSever.h"

using namespace std;

string getTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    auto parts = localtime(&now_c);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", parts);
    return std::move(time_str);
}

UINT32 get_club_id(int fd) {
    std::string msg = "欢迎光临赛博酒馆~请输入您要前往的俱乐部房间号，请输入大于0的全数字哦~\n房间号：";
    write(fd, msg.c_str(), msg.size());
    int rcv_len;
    char buf[32];
    while ((rcv_len = read(fd, buf, 32)) > 0) {
        buf[rcv_len - 1] = '\0'; // 忽略换行符
        if (strcmp(buf, "exit") == 0) {
            std::string exit_msg = "感谢光临赛博酒馆~欢迎下次再来哦~\n";
            write(fd, exit_msg.c_str(), exit_msg.size());
            return 0;
        }
        if (all_of(buf, buf + rcv_len - 1, [](const char &c){ return isdigit(c);})) {
            return stoul(buf);
        }
        string err_msg = "输入有误，请输入大于0的全数字哦~或者输入exit退出呢~\n房间号：";
        write(fd, err_msg.c_str(), err_msg.size());
        memset(buf, 0, 32);
    }
    return 0;
}

int get_socket_fd(unsigned short* port, const char* ip) {
    auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        cerr << "Socket creation failed" << endl;
        return -1;
    }
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(*port);
    auto ret = inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
    if (ret <= 0) {
        cerr << "Invalid server IP address" << endl;
        return -1;
    }
    ret = bind(sockfd, static_cast<sockaddr*>(static_cast<void*>(&server_addr)), sizeof(sockaddr_in));
    if (ret == -1) {
        cerr << "Socket binding failed" << endl;
        return -1;
    }
    return sockfd;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << endl;
        exit(EXIT_FAILURE);
    }
    unsigned short port_num = static_cast<unsigned short>(std::stoi(argv[2]));
    auto sockfd = get_socket_fd(&port_num, argv[1]);
    
    if (sockfd == -1) {
        exit(EXIT_FAILURE);
    }
    auto ret = listen(sockfd, 32);
    if (ret == -1) {
        cerr << "Socket listening failed" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "酒馆已开门~开始营业~[住址][" << argv[1] << ":" << port_num << "]" << endl;
    TavernaServer server;
    while (true) {
        shared_ptr<sockaddr_in> client_addr = make_shared<sockaddr_in>();
        socklen_t addr_len = sizeof(sockaddr_in);
        auto accept_fd = accept(sockfd,  static_cast<sockaddr*>(static_cast<void*>(client_addr.get())), &addr_len);
        if (accept_fd == -1) {
            cerr << "Socket accept failed" << endl;
            usleep(1000000);
        }
        std::shared_ptr<Visitor> v = make_shared<Visitor>();
        v->fd = accept_fd;
        v->addr = client_addr;
        v->club_id = 0;
        server.welcome_visitor(v);
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}