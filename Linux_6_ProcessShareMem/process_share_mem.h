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
using UINT32 = unsigned int;
const UINT32 BUFF_LEN = 128;
const UINT32 MSG_LEN = 512;
const UINT32 INVALID_EXCITENUM = 0;

class MaidStatus {
    int pid;            // 写入者的 PID (是谁请求？)
    UINT32 excitement;     // 兴奋度 (数值数据)
    char command[BUFF_LEN];  // 主人的命令 (字符串数据)
    char gesture[BUFF_LEN];  // 当前体位
    bool useFlag;
    int token;          // 一个简易的标志位 (用于后面演示同步问题)
    MaidStatus(int pid = 0, UINT32 excitement = 0,
        const char *command = "", const char *gesture = "", bool useFlag = false, int token = 0)
        : pid(pid), excitement(excitement), useFlag(useFlag), token(token) {
        strncpy(this->command, command, BUFF_LEN - 1);
        strncpy(this->gesture, gesture, BUFF_LEN - 1);
    }
public:
    MaidStatus& set(istream &is);
    MaidStatus& set(UINT32 excitement, char* command, char* gesture);
    bool readFlag() const;
    void switchFlag();
    void show() const;
    bool assertQuit() const;
    int getPid() const;
};

#endif