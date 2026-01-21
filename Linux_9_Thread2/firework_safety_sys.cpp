#include "birthday_fireworks.h"

using namespace std;

void* task_blackbox_logger(void* arg) {
    unsigned int times = 0;
    int oldstate{0};
    while (true) {
        usleep(500000);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
        ++times;
        cout << "[黑匣子] 开始写入第 " << times <<" 条关键数据..." << endl;
        usleep(2000000);
        cout << "[黑匣子] 第 " << times <<"条数据写入完毕！" << endl;
        pthread_setcancelstate(oldstate, nullptr);
    }
}

void* task_ballistic_engine(void* arg) {
    unsigned calculateTimes = 0;
    cout << "🧮 [计算员] 开始疯狂计算弹道中..." << endl;
    while (true) {
        ++calculateTimes;
        if (calculateTimes == 1e9) {
            calculateTimes = 0;
            pthread_testcancel();
            cout << "🧮 [计算员] 仍在计算中..." << endl;
        }
    }
}

int main() {
    pthread_t loggerPtid, enginePtid;
    cout << "[主线程] 黑匣子与计算员！启动！~~~" << endl;
    pthread_create(&loggerPtid, nullptr, task_blackbox_logger, nullptr);
    pthread_create(&enginePtid, nullptr, task_ballistic_engine, nullptr);

    usleep(6000000);
    cout << "[主线程] 下班了！收！！！~~~" << endl;
    pthread_cancel(loggerPtid);
    pthread_cancel(enginePtid);
    void *temp = nullptr;
    pthread_join(loggerPtid, &temp);
    if (temp == PTHREAD_CANCELED) {
        cout << "[主线程] 很好~记录员已下班~ 😊" << endl;
    } else {
        cout << "[主线程] 什么！记录员竟然想内卷继续加班！ 😰" << endl;
    }
    pthread_join(enginePtid, &temp);
    if (temp == PTHREAD_CANCELED) {
        cout << "[主线程] 很好~计算员已下班~ 😊" << endl;
    } else {
        cout << "[主线程] 什么！计算员竟然想内卷继续加班！ 😰" << endl;
    }
}