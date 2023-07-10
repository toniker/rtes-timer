#include <iostream>
#include <cmath>
#include <cstdint>
#include <pthread.h>
#include <unistd.h>
#include <chrono>

#include "main.hpp"

void* production(void *arg) {
    auto userData = *static_cast<double *>(arg);

    auto workArray_ptr = malloc(NumberOfJobs * sizeof(workFunction));
    if (workArray_ptr == nullptr) {
        std::cout << "Error initializing workArray" << std::endl;
        pthread_exit(nullptr);
    }
    auto workArray = static_cast<workFunction *>(workArray_ptr);
    double angle = 30;
    workArray[0].work = calculate_sin;
    workArray[1].work = calculate_cos;
    workArray[2].work = hello;
    workArray[3].work = tan;
    workArray[4].work = atan;

    for (uint8_t i = 0; i < NumberOfJobs; i++) {
        workArray[i].arg = &angle;
        pthread_mutex_lock(q.mutex);

        addToQueue(&q, workArray[i]);
        pthread_cond_signal(q.has_items);
        while (q.head == q.tail) {
            pthread_cond_wait(q.has_space, q.mutex);
        }

        pthread_mutex_unlock(q.mutex);
        useconds_t sleepDuration = 1'000'000;
        gettimeofday(&lastCallTime, nullptr);
        usleep(sleepDuration);
        gettimeofday(&currentTime, nullptr);
    }

    pthread_exit(nullptr);
}

void* consumption(void *arg) {
    timeval time{};

    while (true) {
        pthread_cond_wait(q.has_items, q.mutex);
        pthread_mutex_lock(q.mutex);

        workFunction workFunction{};
        pthread_cond_signal(q.has_space);

        workFunction.work(static_cast<double *>(workFunction.arg));
    }

    pthread_exit(nullptr);
}

void start() {

}

int main() {
    UserData = new(double);
    *UserData = 5.134;

    Queue queue;
    struct data {
        Queue* q;
        double* d;
    };
    data data = {&queue, UserData};

    StartFcn();
    pthread_t producer, consumer;

    pthread_create(&producer, nullptr, &production, &data);
    pthread_create(&consumer, nullptr, &consumption, &data);

    pthread_join(producer, nullptr);
    pthread_join(consumer, nullptr);

    StopFcn();
    return 0;
}
