#include <iostream>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>

using UINT32 = unsigned int;

enum BurgerType {
    CyberCheese = 0,
    NeonBeef,
    CyberSpicy,
    ChineseFlavor,
    HealthStyle,
    BurgerTypeMax
};

struct Burger {
    UINT32 serial_id;      // 流水号
    std::string type;   // 类型："Cyber-Cheese" 或 "Neon-Beef"
};

class CyberKitchen {
private:
    // --- 核心资源区 ---
    std::queue<Burger> conveyor_belt_; // 传送带 (任务队列)
    
    // --- 同步工具区 ---
    pthread_mutex_t mutex_;             // 保护传送带的锁
    sem_t burger_sem_;                  // 信号量：代表"做好的汉堡数量"
    pthread_spinlock_t chefSpinLock_;   // 自旋锁：用来分配无人机与厨师的标号
    pthread_spinlock_t deliverSpinLock_;// 自旋锁：用来分配无人机与厨师的标号

    // 全局数据区
    static UINT32 global_id;
    static std::string burger_type_name[BurgerTypeMax];
    static UINT32 chef_id;
    static UINT32 drone_id;

public:
    CyberKitchen() {
        // 初始化锁和无名信号量 (pshared=0/PTHREAD_PROCESS_PRIVATE)
        pthread_mutex_init(&mutex_, nullptr);
        sem_init(&burger_sem_, PTHREAD_PROCESS_PRIVATE, 0);
        pthread_spin_init(&chefSpinLock_, PTHREAD_PROCESS_PRIVATE);
        pthread_spin_init(&deliverSpinLock_, PTHREAD_PROCESS_PRIVATE);
    }

    ~CyberKitchen() {
        // 销毁锁和信号量
        sem_destroy(&burger_sem_);
        pthread_mutex_unlock(&mutex_);
        pthread_mutex_destroy(&mutex_);
        pthread_spin_unlock(&chefSpinLock_);
        pthread_spin_destroy(&chefSpinLock_);
        pthread_spin_unlock(&deliverSpinLock_);
        pthread_spin_destroy(&deliverSpinLock_);
    }

    // 👨‍🍳 机械臂逻辑 (Producer)
    // 动作：加锁 -> 生成汉堡 -> 入队 -> 解锁 -> V操作(通知)
    void makeBurger(const int& id) {
        while (true) {
            usleep(500000); // 模拟赛博大厨做汉堡的时间
            pthread_mutex_lock(&mutex_);
            Burger burger{global_id++, burger_type_name[rand() % BurgerTypeMax]};
            conveyor_belt_.push(burger);
            std::cout << "[ 🍔 👨‍🍳 赛博大厨#" << id << " 👩‍🍳 🍔 ] 生产了第" << " # " << burger.serial_id
                << " 号赛博顶尖汉堡！ 品类为 " << burger.type << " !. 当前传送带上共有 "
                << conveyor_belt_.size() << " 个汉堡." << std::endl;
            pthread_mutex_unlock(&mutex_);
            sem_post(&burger_sem_);
        }
    }

    // 🛸 无人机逻辑 (Consumer)
    // 动作：P操作(等汉堡) -> 加锁 -> 取汉堡 -> 出队 -> 解锁 -> 模拟配送(sleep)
    void deliverBurger(const int& drone_id) {
        while (true) 
        {
            sem_wait(&burger_sem_);
            pthread_mutex_lock(&mutex_);
            Burger burger = conveyor_belt_.front();
            conveyor_belt_.pop();
            std::cout << "[ 🐱‍🏍 🍔 赛博快递员#" << drone_id << " 🍔 🐱‍🏍 ] 正在配送第 #" << burger.serial_id
                << " 号赛博顶尖汉堡！ 品类为 " << burger.type << " !. 当前传送带上共有 "
                << conveyor_belt_.size() << " 个汉堡." << std::endl;
            pthread_mutex_unlock(&mutex_);
            usleep(1000000); // 模拟赛博快递员做汉堡的时间
        }
    }

    // --- 线程入口函数 (Static wrappers) ---
    // 因为 pthread_create 只能传静态函数，所以需要这两个跳板
    static void* chef_thread(void* arg);
    static void* drone_thread(void* arg);
    
    // 启动厨房：创建 1 个厨师线程，3 个无人机线程
    void startBusiness();
};