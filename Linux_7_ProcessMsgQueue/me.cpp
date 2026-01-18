#include "process_msg.h"
#include <cstdio>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <vector>

int main()
{
    key_t key = ftok(PATH_NAME, PROJ_ID);
    if (key == MSG_INVAILD_RET) {
        cerr << "Key Generation Failed!" << endl;
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int qid = msgget(key, 0666);
    if (qid == MSG_INVAILD_RET) {
        cerr << "Message Queue Creation Failed!" << endl;
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    int ret = 0;
    vector<string> mood{"[❤️ 紧急]", "[😊 开心]", "[😩 苦逼]"};
    vector<string> react{"马上执行！！", "嘿嘿~~", "...不想动。"};
    ostringstream oss;
    MsgUnit msgbuf{};
    while (msgrcv(qid, &msgbuf, MSG_BUF_SIZE, -3, IPC_NOWAIT) > 0) {
        oss << mood[msgbuf.signType - 1] << " 收到老婆的消息 : ";
        oss << msgbuf.text;
        cout << oss.str() << endl;
        oss.clear();
        oss.str("");
        usleep(500000);
    }
    oss << "[❤️❤️]老公已收到老婆的全部消息! 老婆可以继续休息做自己的事情啦~";
    cout << oss.str() << endl;
    msgbuf.set(6, oss);

    ret = msgsnd(qid, &msgbuf, MSG_BUF_SIZE, 0);
    if (ret == MSG_INVAILD_RET) {
        cerr << "MSG send Error!";
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}