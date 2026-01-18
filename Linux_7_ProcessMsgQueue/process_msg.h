#ifndef __PROCESS_MSG_H__
#define __PROCESS_MSG_H__

#include <cstring>
#include <iostream>
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>
#include <sstream>

using namespace std;
using UINT32 = unsigned int;
constexpr int MSG_INVAILD_RET = -1;
constexpr UINT32 MSG_TEXT_SIZE = 128;
constexpr char PROJ_ID = 1;

class MsgUnit {
    long signType;
    char text[128];

    friend int main();
public:
    MsgUnit(){}

    void set(long type, const char* msg) {
        signType = type;
        // snprintf 比 strncpy 更安全，会自动在末尾补 \0
        snprintf(text, MSG_TEXT_SIZE, "%s", msg);
    }

    void set(long type, const string& msg) {
        this->set(type, msg.c_str());
    }

    void set(long type, ostringstream& oss) {
        signType = type;
        // 获取流中的字符串并写入 char 数组
        snprintf(text, MSG_TEXT_SIZE, "%s", oss.str().c_str());
        // 清空流内容并重置状态位，以便下次复用
        oss.str("");
        oss.clear();
    }

    void show() {
        cout << "Type: " << signType << ", Msg: " << text << endl;
    }
    
};

constexpr UINT32 MSG_BUF_SIZE = sizeof(MsgUnit) - sizeof(long);

constexpr const char* PATH_NAME = "./file/que";

#endif