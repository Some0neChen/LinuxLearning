#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>  // 这个头文件包含了 read, write, close 函数
#include <arpa/inet.h>  // inet_ntoa 和 ntohs 都在这里面，负责端口转换

using namespace std;

int main() {
    /*
        domain（协议族）：我们要用 IPv4 寻址，填 AF_INET。
        type（套接字类型）：我们要用可靠的、面向连接的 TCP 协议，填 SOCK_STREAM（流式套接字）。如果你想用 UDP，就填 SOCK_DGRAM。
        protocol（具体协议）：默认填 0，内核会自动根据前面的 SOCK_STREAM 选定 TCP。
        返回值：
            成功：返回一个大于 0 的整数。这叫 文件描述符 (File Descriptor, 简称 fd)。你可以把它理解为这部手机的“入网许可证号”。
            失败：返回 -1。
    */
    auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        cerr << "Socket creation failed" << endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET; // IPv4
    /*
        htons(8888)：Host TO Network Short。把你的主机端口号，翻译成网络能看懂的格式。
        htonl(INADDR_ANY)：Host TO Network Long。把 IP 地址翻译成网络格式。  
    */
    server_addr.sin_port = htons(8888); // 端口号，使用 htons 转换为网络字节序
    server_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有可用的接口/网卡 IP (0.0.0.0)

    /*
        sockfd：刚才买的手机，也就是 socket() 返回的那个整数。
        addr：刚才填好的那张手机卡结构体的内存地址。
        addrlen：手机卡结构体的大小（直接 sizeof(server_addr)）。
        返回值：成功返回 0，失败返回 -1。
    */
    auto ret = bind(sockfd, static_cast<struct sockaddr*>(static_cast<void*>(&server_addr)), sizeof(sockaddr_in));
    if (ret == -1) {
        cerr << "Socket binding failed" << endl;
        exit(EXIT_FAILURE);
    }

    /*
        int listen(int sockfd, int backlog);告知内核进入被动接听状态，如果有 TCP 的 SYN 握手包找我，你帮我在后台处理一下三次握手。
        sockfd：那部手机的 fd。
        backlog：未接来电的队列长度（核心！）。比如填 128。当并发量极大时，内核帮你做完了三次握手，但你的代码还没来得及处理，这些连接就会排队。队伍最长就是 128，超过了内核就会直接拒绝新的连接。
        返回值：成功返回 0，失败返回 -1。
    */
    ret = listen(sockfd, 128);
    if (ret == -1) {
        cerr << "Socket listening failed" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "开始监听 8888 端口..." << endl;
    struct sockaddr client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);
    /*
        sockfd：你用来监听的总机 fd。
        addr：一个空的结构体指针。内核会把连进来的客户的 IP 和端口填到这里面，当作名片交给你！
        addrlen：传入空结构体的大小。
        返回值（极其极其重要）：
            成功：返回一个新的、专门为这个客户服务的文件描述符 (client_fd)！
            失败：返回 -1。
    */
    auto client_fd = accept(sockfd, &client_addr, &client_addr_len);
    if (client_fd == -1) {
        cerr << "Socket accept failed" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "有客户连接进来了！" << endl;

    char buffer[1024] = {};
    char reply[1024] = "Hello from 师姐！你的消息我收到啦！\n";
    ssize_t bytes_received = 0;
    sockaddr_in* client_info = static_cast<struct sockaddr_in*>(static_cast<void*>(&client_addr));
    auto client_ip = inet_ntoa(client_info->sin_addr); // (Network TO ASCII)
    auto client_port = ntohs(client_info->sin_port);
    while ((bytes_received = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        if (strcmp(buffer, "exit\n") == 0) {
            cout << "客户请求断开连接。" << endl;
            break;
        }
        cout << "收到[" << client_ip << ":" << client_port << "[客户消息：" << buffer << endl;
        write(client_fd, reply, sizeof(reply));
        memset(buffer, 0, sizeof(buffer));
    }
    
    close(client_fd);
    close(sockfd);

    exit(EXIT_SUCCESS);
}