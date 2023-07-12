#pragma once

#include <iostream>

typedef std::function<void(long long* arg)> TimerFcn;
typedef std::function<void()> StartFcn;
typedef std::function<void()> StopFcn;
typedef std::function<void()> ErrorFcn;

void MyTimerFcn(const long long* arg) {
    auto data = *arg;
    timeval currentTime{};
    gettimeofday(&currentTime, nullptr);
    long long currentTimestamp = currentTime.tv_sec * 1000LL + currentTime.tv_usec / 1000LL;
    long long delay = currentTimestamp - data;
    std::cout << "Time since last queue operation:" << delay << std::endl;
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