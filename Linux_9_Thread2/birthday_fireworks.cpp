#include "birthday_fireworks.h"
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *bgm_task(void *args)
{
    while(true) {
        cout << "🎵 BGM: 祝你生日快乐..." << endl;
        usleep(1000000);
    }
}

void *fireToSpark(void *args)
{
    cout << "(嗖...) 烟花升空！" << endl;
    usleep(1000000);
    Firework* spark = static_cast<Firework*>(args);
    spark->showSpark();
    delete(spark);
    pthread_exit(nullptr);
}

int main()
{
    vector<string> shapes({"🎇", "✨", "❤️"});
    vector<string> colors({RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN});
    pthread_t bgmThreadNo;
    pthread_create(&bgmThreadNo, nullptr, bgm_task, nullptr);
    pthread_t sparkThreadNo;
    for (int i = 0; i < 32; ++i) {
        Firework* spark = new Firework(colors[i % colors.size()], shapes[i % shapes.size()]);
        pthread_create(&sparkThreadNo, nullptr, fireToSpark, spark);
        pthread_detach(sparkThreadNo);
    }
    cout << "Appreaciate Spark!~~" << endl;
    usleep(3000000);
    pthread_cancel(bgmThreadNo);
    pthread_join(bgmThreadNo, nullptr);
    std::cout << MAGENTA << "演出结束，生日快乐！" << RESET << std::endl;
    pthread_exit(nullptr);
}