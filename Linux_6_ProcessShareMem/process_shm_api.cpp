#include "process_share_mem.h"
#include <cstring>

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