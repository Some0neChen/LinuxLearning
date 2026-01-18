#ifndef __BIRTHDAY_FIREWORK_H__
#define __BIRTHDAY_FIREWORK_H__

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <iomanip>
#include <ios>

// 放在代码最前面
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta (品红，很骚气) */
#define CYAN    "\033[36m"      /* Cyan */
#define RESET   "\033[0m"

// 用法：
// printf(MAGENTA "❤️ 生日快乐！" RESET "\n");

class Firework {
    std::string color;
    std::string shape;
public:
    Firework(std::string color = RED, std::string shape = "✨") :
        color(color), shape(shape) {}
    void showSpark() {
        std::cout << this->color << std::setw(3) << std::left
            << this->shape << "生日快乐！" << RESET << std::endl;
    }
};

#endif