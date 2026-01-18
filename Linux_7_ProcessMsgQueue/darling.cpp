#include "process_msg.h"
#include <vector>

int main()
{
    vector<vector<string>> wifeOrder{{"抱抱我", "亲亲", "想要了"},
                                      {"饿了吗", "看这个笑话", "今晚吃什么"},
                                      {"去倒垃圾", "记得写代码", "修灯泡"}};
    key_t key = ftok(PATH_NAME, PROJ_ID);
    if (key == MSG_INVAILD_RET) {
        cerr << "Key Generation Failed!" << endl;
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int qid = msgget(key, 0666 | IPC_CREAT);
    if (qid == MSG_INVAILD_RET) {
        cerr << "Message Queue Creation Failed!" << endl;
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    int ret = 0;
    MsgUnit msgbuf{};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < wifeOrder.size(); ++j) {
            wifeOrder[j][i].insert(0, "[老婆的指令♥] ");
            msgbuf.set(j + 1,  wifeOrder[j][i]);
            ret = msgsnd(qid, &msgbuf, MSG_BUF_SIZE, 0);
            if (ret == MSG_INVAILD_RET) {
                cerr << "Message Queue Send Failed!" << endl;
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }
            usleep(500000);
        }
    }

    ret = msgrcv(qid, &msgbuf, MSG_BUF_SIZE, 6, 0);
    if (ret == MSG_INVAILD_RET) {
        cerr << "Message Queue Receive Failed!" << endl;
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    cout << "[收到老公发来的消息♥] : ";
    msgbuf.show();

    ret = msgctl(qid, IPC_RMID, nullptr);
    if (ret == MSG_INVAILD_RET) {
        cerr << "Message Queue Delete Failed!" << endl;
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}