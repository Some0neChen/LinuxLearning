#include "process_msg.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <sys/msg.h>

void exec_exit(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main()
{
    int msgqId = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msgqId == MSG_INVAILD_RET) {
        cerr << "Message Queue Creation Failed!" << endl;
        perror("msgget");
        return -1;
    }
    Womb womb;
    char msg[MSG_TEXT_SIZE];
    UINT32 times = 1;

    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Fork Failed!" << endl;
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        while (times < 99) {
            if (msgrcv(msgqId, &womb, WOMB_MSG_SIZE, -99, 0) < 0) {
                if (errno == EINTR) {
                    continue;
                }
                exec_exit("msgrcv");
            }
            if (womb.signType == 99) {
                break;
            }
            womb.show();
            cout << " -> [Baby] 咕嘟咕嘟... 吸收营养。" << endl;
            snprintf(msg, MSG_TEXT_SIZE, " -> [Baby%u] 我长大了，踢爸爸第%d下。", getpid(), times);
            womb.set(128, msg);
            if (msgsnd(msgqId, &womb, WOMB_MSG_SIZE, 0) != 0) {
                exec_exit("msgsnd");
            }
            times++;
        }
        
        if (msgrcv(msgqId, &womb, WOMB_MSG_SIZE, -99, 0) < 0) {
            exec_exit("msgrcv");
        }

        womb.show();
        cout << " -> [Baby] 哇！我出生了！(退出)" << endl;
        exit(EXIT_SUCCESS);
    }

    while (times < 99) {
        snprintf(msg, MSG_TEXT_SIZE, "[Dad%u] 第%u次输送营养液 (Type 1)...", getpid(), times);
        womb.set(1, msg);
        if (msgsnd(msgqId, &womb, WOMB_MSG_SIZE, 0) != 0) {
            msgctl(msgqId, IPC_RMID, 0);
            exec_exit("msgsnd");
        }

        if (msgrcv(msgqId, &womb, WOMB_MSG_SIZE, -128, IPC_NOWAIT) == MSG_INVAILD_RET) {
            if (errno != EINTR && errno != ENOMSG) {
                msgctl(msgqId, IPC_RMID, 0);
                exec_exit("msgrcv");
            }
        } else {
            womb.show();
            cout << "感受到宝宝踢我了 (Type 3)！好开心！" << endl;
        }
        
        times++;
        usleep(100000); // 稍微歇一会，模拟产奶时间
    }

    snprintf(msg, MSG_TEXT_SIZE, "[Dad%u]  营养足够了，宝贝出来吧 (Type 99)！", getpid());
    womb.set(99, msg);
    if (msgsnd(msgqId, &womb, WOMB_MSG_SIZE, 0) != 0) {
        msgctl(msgqId, IPC_RMID, 0);
        exec_exit("msgsnd");
    }

    waitpid(pid, nullptr, 0);
    cout << "完美完成！" << endl;
    if (msgctl(msgqId, IPC_RMID, 0) != 0 ) {
        exec_exit("msgctl");
    }
    exit(EXIT_SUCCESS);
}