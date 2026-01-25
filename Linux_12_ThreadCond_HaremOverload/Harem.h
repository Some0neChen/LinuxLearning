#ifndef __HAREM_H__
#define __HAREM_H__

#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <deque>
#include <algorithm>
#include <deque>
#include <queue>

using UINT32 = unsigned int;
using UCHAR = unsigned char;

class concubine
{
    std::string name;
    UINT32 glamor;
    std::string concubineId;
public:
    concubine(const std::string& name_, UINT32 glamor_, const std::string& id_)
        : name(name_), glamor(glamor_), concubineId(id_) {}

    void come() const {
        std::cout << "宠姬 [" << name << "] [" << glamor
                  << "] [" << concubineId << "] " << "来到了后宫..." << std::endl;
    }

    void dress_up() const {
        std::cout << "宠姬 [" << name << "] [" << glamor
                  << "] 正在打扮自己..." << std::endl;
        usleep((200 + (rand() % 10) * 40) * 1000); // 模拟打扮时间
    }

    void coquetry() const {
        std::cout << "宠姬 [" << name << "] [" << glamor
                  << "] 💢 前面的人快点啊！主公是我的！我都流水了！~~ 💢" << std::endl;
    }

    void coy() const {
        std::cout << "宠姬 [" << name << "] [" << glamor
                  << "] 💋 主公...你看...下面全湿了...快进来..我已经没法忍受了... 💋" << std::endl;
    }

    void wait_in_lounge() const {
        std::cout << "宠姬 [" << name << "] [" << glamor
                  << "] " << "正在候宫中等待宠幸..." << std::endl;
    }

    void enjoy_blissed() const {
        std::cout << "❤️正在宠信宠姬 [" << name << "] [" << glamor
                  << "] ❤️ "  << std::endl;
        usleep(10000000); // 模拟宠信时间
    }

    void rest() const {
        std::cout << "宠姬 [" << name << "] [" << glamor
                  << "] " << "哈啊...满足了...我要休息会..." << std::endl;
        usleep((2000 + (rand() % 10) * 20) * 1000); // 模拟休息时间
    }

    void cut_in() const {
        std::cout << " ⚡ 绝世美人 [" << this->name << "] 魅力("
                    << this->glamor << ") 强行插队到了前面！" << std::endl;
    } 

    UCHAR get_glamor() const {
        return glamor;
    }
};

class concubine_raii
{
    concubine* p_concubine;
public:
    concubine_raii() {
        p_concubine = nullptr;
    }

    concubine_raii(concubine* p) {
        p_concubine = p;
    }

    ~concubine_raii() {
        if (p_concubine == nullptr) {
            return;
        }
        delete p_concubine;
    }
};

#endif // __HAREM_H