// Chore.h
#pragma once
#include <string>
#include <functional>

// 任务类型：不仅仅是 void()，我们要带点参数
enum ChoreType {
    WASH_DISHES,    // 洗碗
    MASSAGE_LEG,    // 按摩大腿
    WRITE_CODE,     // 写代码
    BUY_MILK,       // 买奶茶
    WARM_BED        // 暖床 (优先级最高!)
};

struct Chore {
    int id;                 // 任务ID
    ChoreType type;         // 类型
    std::string description;// 任务描述 (比如: "去楼下买布丁")
    int complexity;         // 复杂度 (决定你需要 sleep 多久来模拟耗时)
};