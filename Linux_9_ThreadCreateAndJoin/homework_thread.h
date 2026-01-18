#ifndef __HOMEWORK_THREAD_H__
#define __HOMEWORK_THREAD_H__

#include <iomanip>
#include <ios>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

using namespace std;

class calculateRes {
    int max;
    int sum;
public:
    calculateRes(int max = 0, int sum = 0) : max(max), sum(sum) {}
    auto printRes() -> void {
        cout << std::setw(5) << right << "max:" << setw(3) << max
            << setw(5) << left << "sum" << setw(5) << sum << endl;

    }
    auto updateRes(int ele) -> void {
        if (ele > max) {
            max = ele;
        }
        sum += ele;
    }
};

#endif