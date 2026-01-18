#include <csignal>
#include <iostream>
#include <atomic>
#include <sys/wait.h>
using namespace std;

atomic<bool> husbandSwitch(false); 
atomic<bool> climaxSwitch(false);
__sighandler_t originalHandler;


void husbandReactHandler(int sign) {
    signal(SIGQUIT, originalHandler);
    husbandSwitch.store(true);
}

void husbandClimateHandler(int sign) {
    signal(SIGQUIT, originalHandler);
    climaxSwitch.store(true);
}

int main(int argc, const char* argv[]) {
    cout << "收到老婆信号♥♥ 开始在床上跟老婆抱着亲热..." << endl;
    originalHandler = signal(SIGQUIT, husbandReactHandler);
    signal(SIGINT, husbandClimateHandler);
    while (!climaxSwitch.load()) {
        if (!husbandSwitch.load()) {
            cout << "唔...好舒服...正在和老婆抱着亲亲..." << endl;
            usleep(1000000);
            continue;
        }
        cout << "正在与老婆抱着亲热...好舒服...啊...不断抽插中...好爱老婆......~~~" << endl;
        usleep(1000000);
    }
    cout << "与老婆一起高潮了！结束..." << endl;
    return 0;
}