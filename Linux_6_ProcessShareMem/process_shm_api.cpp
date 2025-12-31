#include "process_share_mem.h"
#include <cstring>

/*
本章节最终作业目标：【心智同调：结构化数据的直接映射】
在之前的管道中，我们传递的是“流（字节流）”，您得把数据切碎了塞进去，那边再拼起来，非常麻烦。 本次作业目标：我们不再传字符串，而是直接共享一个复杂的 C++ 对象（结构体）。

场景：

魔王（Writer）：在内存中直接修改一个 struct MaidStatus（包含心跳值、兴奋度、当前命令）。

女仆（Reader）：瞬间感知到结构体数值的变化，并根据“兴奋度”做出反应。

核心要求：

使用 ftok 生成唯一键值。

Writer 负责创建 (IPC_CREAT) 和销毁 (IPC_RMID)。

Reader 负责连接 (shmat)。

难点：体验没有“读写函数”的操作方式——直接操作指针。
*/

MaidStatus& MaidStatus::set(istream &is)
{
    pid_t pid = getpid();
    string tempStr{};
    this->pid = pid;
    cout << "[" << pid << "] 申请动作: ";
    getline(cin, tempStr);
    strncpy(this->command, tempStr.c_str(), BUFF_LEN - 1);
    cout << "[" << pid << "] 执行内容: ";
    getline(cin, tempStr);
    strncpy(this->gesture, tempStr.c_str(), BUFF_LEN - 1);
    cout << "[" << pid << "] 快感程度: ";
    is >> this->excitement;
    cin.get();

    return *this;
}

void MaidStatus::show() const
{
    char msgInfo[MSG_LEN];
    sprintf(msgInfo, "[%d] 发来%s信息, 请求%s. 此时对方兴奋度♥%d♥.",
            this->pid, this->command, this->gesture, this->excitement);
    cout << msgInfo << endl;
}

MaidStatus& MaidStatus::set(UINT32 excitement, char* command, char* gesture)
{
    if (excitement != INVALID_EXCITENUM) {
        this->excitement = excitement;
    }
    if (command != nullptr) {
        strncpy(this->command, command, BUFF_LEN - 1);
    }
    if (gesture != nullptr) {
        strncpy(this->gesture, gesture, BUFF_LEN - 1);
    }
    return *this;
}

bool MaidStatus::readFlag() const 
{
    return this->useFlag;
}

void MaidStatus::switchFlag()
{
    this->useFlag = !this->useFlag;
}

bool MaidStatus::assertQuit() const
{
    return strcmp(command, "quit") == 0;
}

int MaidStatus::getPid() const
{
    return this->pid;
}