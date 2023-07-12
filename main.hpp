#pragma once

#include <iostream>

typedef std::function<void(long long* arg)> TimerFcn;
typedef std::function<void()> StartFcn;
typedef std::function<void()> StopFcn;
typedef std::function<void()> ErrorFcn;

void MyTimerFcn(const long long* arg) {
    auto data = *arg;
    std::cout << "Caclulated Drift = " << data << std::endl;
}

void MyStartFcn() {
    std::cout << "Timer started." << std::endl;
}

void MyStopFcn() {
    std::cout << "Timer stopped." << std::endl;
}

void MyErrorFcn() {
    std::cout << "Queue is full." << std::endl;
}