#pragma once
#include "etl/queue_mpmc_mutex.h"

struct workFunction {
    void (*work)(double *);

    void *arg;
};

inline constexpr uint8_t QueueSize = 10;
inline constexpr uint8_t NumberOfJobs = 5;

inline void initialize()
{
    std::cout << "Starting" << std::endl;
}

inline void periodic() {
    std::cout << "Periodic" << std::endl;
}

inline void reportQueueFullError() {
    std::cout << "Queue Full!" << std::endl;
}

inline void stop()
{
    std::cout << "Stopping" << std::endl;
}

/**
 * The delay between function calls, in milliseconds.
*/
inline constexpr uint16_t Period = 20;
/**
 * How many times the TimerFcn will be executed.
*/
inline constexpr uint8_t TasksToExecute = 5;
/**
 * Delay before the first call to TimerFcn.
*/
inline constexpr uint16_t StartDelay = 4;
/**
 * The function that will be called during the Timer constructor for data initialization
*/
inline constexpr void (*StartFcn)() = {initialize};
/**
 * The function that will be called after the last TimerFcn call.
*/
inline constexpr void (*StopFcn)() = {stop};
/**
 * The function that will be called periodically.
*/
inline constexpr void (*TimerFcn)() = {periodic};
/**
 * The function that will be called if the task queue is full.
*/
inline constexpr void (*ErrorFcn)() = {reportQueueFullError};
/**
 * Pointer to user data
*/
inline double* UserData;


void calculate_sin(double *arg) {
    double angle = *arg;
    *arg = sin(angle);
}

void calculate_cos(double *arg) {
    double angle = *arg;
    *arg = cos(angle);
}

void hello(double *arg) {
    double value = *arg;
    std::cout << "Hello from the thread! value: " << value << std::endl;
}

void tan(double *arg) {
    double angle = *arg;
    *arg = tan(angle);
}

void atan(double *arg) {
    double angle = *arg;
    *arg = atan(angle);
}

class Queue {
public:
    etl::queue_mpmc_mutex<workFunction, QueueSize> queue;


};