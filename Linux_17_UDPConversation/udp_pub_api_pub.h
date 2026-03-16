#pragma once

#include <iostream>
#include <sys/socket.h>

typedef enum {
    REG = 0,
    MSG,
    EXIT
} udp_msg_type_t;

struct udp_msg_header
{
    udp_msg_type_t type;
    char name[32];
    unsigned int msg_len;
};

std::string getTime();
std::string init_msg(sockaddr* client_addr);