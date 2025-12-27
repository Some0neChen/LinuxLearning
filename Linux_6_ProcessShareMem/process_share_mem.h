#ifndef __PROCESS_SHARE_MEM_H__
#define __PROCESS_SHARE_MEM_H__

#include <iostream>
#include <cstring>      // strcpy, sprintf
#include <istream>
#include <unistd.h>     // Linux系统调用库
#include <sys/ipc.h>    // 系统调用库
#include <sys/shm.h>    // 共享内存库
#include <string>
#include <cstdlib>
#include <sys/types.h>

using namespace std;

class MaidStatus {
    int pid;            // 写入者的 PID (是谁请求？)
    int excitement;     // 兴奋度 (数值数据)
    char command[128];  // 主人的命令 (字符串数据)
    char gesture[128];  // 当前体位
    int token;          // 一个简易的标志位 (用于后面演示同步问题)
public:
    MaidStatus(int pid = 0, int excitement = 0,
        const char *command = "", const char *gesture = "", int token = 0)
        : pid(pid), excitement(excitement), token(token) {
        strncpy(this->command, command, sizeof(this->command) - 1);
        strncpy(this->gesture, gesture, sizeof(this->gesture) - 1);
    }
    MaidStatus(istream &is);
    void show() const;
    void setReplyMsg(int excitement, char* command, char* gesture);
    void setReplyMsg(istream &is);
};

#endif