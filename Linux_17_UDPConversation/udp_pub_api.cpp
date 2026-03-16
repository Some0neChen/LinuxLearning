#include "udp_pub_api_pub.h"
#include <arpa/inet.h>
#include <chrono>
#include <ctime>
#include <netinet/in.h>
#include <string>

using namespace std;

string getTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    struct tm result_tm;
    // auto parts = localtime(&now_c);
    auto parts = localtime_r(&now_c, &result_tm); // 相比于localtime，localtime_r是线程安全的
    // parts和result_tm指向同一块内存，localtime_r将结果写入result_tm并返回指向result_tm的指针
    string time_str(20, '\0');
    auto date_len = strftime(time_str.data(), time_str.size(), "%Y-%m-%d %H:%M:%S", parts);
    time_str.resize(date_len);
    return time_str;
}

string init_msg(sockaddr* client_addr)
{
    sockaddr_in *addr_in = static_cast<sockaddr_in*>(static_cast<void*>(client_addr));
    auto port = ntohs(addr_in->sin_port);
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr_in->sin_addr, ip_str, INET_ADDRSTRLEN);
    return string(getTime()).append(" [").append(ip_str).append(":").append(to_string(port)).append("]");
}