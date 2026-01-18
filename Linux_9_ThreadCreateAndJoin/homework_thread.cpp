#include "homework_thread.h"
#include <pthread.h>
#include <unistd.h>
#include <vector>

void* calculator(void* arg)
{
    const vector<int>* vec = static_cast<const vector<int>*>(arg);
    calculateRes *res = new calculateRes();
    for (auto it = vec->cbegin(); it != vec->cend(); ++it) {
        res->updateRes(*it);
    }
    usleep(500000);
    cout << " 计算完成，结果已返回主线程处理。" << endl;
    pthread_exit(static_cast<void*>(res));
}

int main()
{
    unsigned char thread_num = 5;
    vector<pthread_t> threads(thread_num, 0);
    vector<calculateRes*> results(thread_num, nullptr);
    vector<vector<int>> data = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10},
        {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},
        {21, 22, 23, 24, 25}
    };
    for (unsigned char i = 0; i < thread_num; ++i) {
        pthread_create(&threads.at(i), nullptr, calculator, static_cast<void*>(&data.at(i)));
    }
    cout << "任务已下发，等下班中..." << endl;
    for (unsigned char i = 0; i < thread_num; ++i) {
        void *temp = nullptr;
        pthread_join(threads[i], &temp);
        results[i] = static_cast<calculateRes*>(temp);
    }
    for (unsigned char i = 0; i < thread_num; ++i) {
        results[i]->printRes();
        delete results[i];
    }

    return 0;
}