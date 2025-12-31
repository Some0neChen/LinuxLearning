#ifndef __PROCESS_MSG_H__
#define __PROCESS_MSG_H__

#include <cstring>
#include <iostream>
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>

using namespace std;
using UINT32 = unsigned int;
constexpr int MSG_INVAILD_RET = -1;
constexpr UINT32 MSG_TEXT_SIZE = 128;

/*
2. 项目二：脐带 (womb.cpp)
这是一个父子进程的双向奔赴，日志要温馨一点。

【父进程 (老公/供体) 输出】

发送营养：cout << "[Dad] 输送营养液 (Type 1)..." << endl;

收到胎动 (Type 3)：cout << "[Dad] 感受到宝宝踢我了 (Type 3)！好开心！" << endl;

分娩 (Type 99)：cout << "[Dad] 营养足够了，宝贝出来吧 (Type 99)！" << endl;

【子进程 (宝宝/受体) 输出】

收到营养：cout << " -> [Baby] 咕嘟咕嘟... 吸收营养。" << endl;

发送胎动：cout << " -> [Baby] 我长大了，踢爸爸一下 (Type 3)！" << endl;

收到分娩：cout << " -> [Baby] 哇！我出生了！(退出)" << endl;
*/
class Womb{
    long signType;
    char text[128];
public:
    Womb(){}
    void set(long type, const char* msg) {
        signType = type;
        strncpy(text, msg, MSG_TEXT_SIZE);
    }
    void show() {
        cout << "Type: " << signType << ", Msg: " << text << endl;
    }
    friend int main();
};

constexpr UINT32 WOMB_MSG_SIZE = sizeof(Womb) - sizeof(long);

/*
1. 项目一：贪婪的消费者 (mistress.cpp & slave.cpp)
【Mistress (我/发送端) 的输出要求】 每次发送前，打印你要对我做的事：

发送 Type 1 (Lick)： cout << "[Mistress] 伸出舌头 (Type 1)... 频率: 0.5s" << endl;

发送 Type 2 (Deep Throat)： cout << "[Mistress] 准备深喉 (Type 2)... 频率: 2s" << endl;

发送 Type 99 (Choke)： cout << "\n[Mistress] !!! 窒息指令 (Type 99) !!! 必须立刻处理！" << endl;

【Slave (你/接收端) 的输出要求】 你要表现出“吞吐”的过程和优先级处理：

收到 Type 1： cout << " -> [Slave] 👅 舔舐中... (接收 Type 1)" << endl;

收到 Type 2： cout << " -> [Slave] 🍆 深喉... 唔... (接收 Type 2，模拟耗时 1s)" << endl; (记得在这里 sleep(1) 模拟处理过程)

收到 Type 99 (利用负数优先接收)： cout << " -> [Slave] 😵 窒息！优先处理紧急指令！(接收 Type 99)" << endl;
*/

#endif