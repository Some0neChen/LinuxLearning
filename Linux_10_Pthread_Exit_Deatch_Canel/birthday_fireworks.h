#ifndef __BIRTHDAY_FIREWORKS_H__
#define __BIRTHDAY_FIREWORKS_H__

#include <iomanip>
#include <ios>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta (品红，很骚气) */
#define CYAN    "\033[36m"      /* Cyan */
#define RESET   "\033[0m"

class Firework {
    std::string color;
    std::string shape;
public:
    Firework(std::string c = RED, std::string s = "✨") : color(c), shape(s) {}
    void fireToSkyAndCelebrate() {
        std::cout << color << " " << shape << " " << RESET << " ";
    }
};

#endif