#include "CyberKitchen.h"

using namespace std;

int main()
{
    CyberKitchen cyber_kitchen;
    cyber_kitchen.startBusiness();
    return 0;
}

UINT32 CyberKitchen::global_id = 0;
UINT32 CyberKitchen::chef_id = 0;
UINT32 CyberKitchen::drone_id = 0;
std::string CyberKitchen::burger_type_name[BurgerTypeMax] = {
    "赛博芝士风味",
    "卤牛肉风味",
    "赛博炫辣风味",
    "中华风味",
    "健康汉堡风味"
};

void* CyberKitchen::chef_thread(void* arg)
{
    CyberKitchen* NCBestKichen = static_cast<CyberKitchen*>(arg);
    UINT32 id = 0;

    pthread_spin_lock(&NCBestKichen->chefSpinLock_);
    id = NCBestKichen->chef_id++;
    pthread_spin_unlock(&NCBestKichen->chefSpinLock_);

    while(true) {
        NCBestKichen->makeBurger(id);
    }
}


void* CyberKitchen::drone_thread(void* arg)
{
    CyberKitchen* NCBestKichen = static_cast<CyberKitchen*>(arg);
    UINT32 id = 0;

    pthread_spin_lock(&NCBestKichen->deliverSpinLock_);
    id = NCBestKichen->drone_id++;
    pthread_spin_unlock(&NCBestKichen->deliverSpinLock_);

    while(true) {
        NCBestKichen->deliverBurger(id);
    }
}

// 启动厨房：创建 1 个厨师线程，3 个无人机线程
void CyberKitchen::startBusiness()
{
    const int chefNums = 2;
    const int droneNums = 3;
    vector<pthread_t> pths(chefNums + droneNums, 0);
    for (int i = 0; i < chefNums; ++i) {
        pthread_create(&pths[i], nullptr, CyberKitchen::chef_thread, static_cast<void*>(this));
    }
    for (int i = chefNums; i < pths.size(); ++i) {
        pthread_create(&pths[i], nullptr, CyberKitchen::drone_thread, static_cast<void*>(this));
    }

    for (auto &it : pths) {
        pthread_join(it, nullptr);
    }
}