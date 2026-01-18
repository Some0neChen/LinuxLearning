#include "birthday_fireworks.h"
#include <pthread.h>

using namespace std;

void *bgm_task(void *args)
{
    while (true) {
        cout << "\033]0;Happy Birthday!\007";
        usleep(500000);
        cout << "\033]🎵 BGM: 祝你生日快乐...\007";
        usleep(500000);
        cout << "\033]0;Enjoy the Fireworks!\007";
        usleep(500000);
    }
    return nullptr;
}

void *fire(void *args) 
{
    Firework *spark = static_cast<Firework*>(args);
    cout << "(嗖...) 烟花升空！" << endl;
    usleep(1000000);
    spark->fireToSkyAndCelebrate();
    pthread_exit(nullptr);
}

int main() {

}