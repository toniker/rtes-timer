#include <queue>
#include <functional>
#include <sys/time.h>
#include <pthread.h>
#include <thread>
#include <utility>
#include <unistd.h>

#include "main.hpp"

struct workFunction {
    TimerFcn timerFcn;
    void *userData{};
};

class Timer {
public:
    Timer(int _period, int _tasksToExecute, int _startDelay, StartFcn _startFcn, StopFcn _stopFcn, TimerFcn _timerFcn,
          ErrorFcn _errorFcn, void *_userData) : period(_period), tasksToExecute(_tasksToExecute),
                                                 startDelay(_startDelay), startFcn(std::move(_startFcn)),
                                                 stopFcn(std::move(_stopFcn)), timerFcn(std::move(_timerFcn)),
                                                 errorFcn(std::move(_errorFcn)), userData(_userData), running(false),
                                                 taskCount(0), nextExecutionTime(0) {
        if (startDelay > 0)
            std::this_thread::sleep_for(std::chrono::seconds(startDelay));
    }

    ~Timer() {
        Stop();
    }

    void Start() {
        if (running)
            return;

        running = true;
        if (startFcn)
            startFcn();

        /*
         * Set the lowest macOS Quality of Service class.
         */
        pthread_attr_t qosAttribute{};
        pthread_attr_init(&qosAttribute);
        pthread_attr_set_qos_class_np(&qosAttribute, QOS_CLASS_BACKGROUND, 0);

        pthread_create(&producerThread, &qosAttribute, &Timer::ProducerThreadFuncHelper, this);
        pthread_create(&consumerThread, &qosAttribute, &Timer::ConsumerThreadFuncHelper, this);
    }

    void StartAt(int year, int month, int day, int hour, int minute, int second) {
        timeval currentTime{};
        gettimeofday(&currentTime, nullptr);
        long long currentTimestamp = currentTime.tv_sec * 1000LL + currentTime.tv_usec / 1000LL;

        tm targetTime{};
        targetTime.tm_year = year - 1900;
        targetTime.tm_mon = month - 1;
        targetTime.tm_mday = day;
        targetTime.tm_hour = hour;
        targetTime.tm_min = minute;
        targetTime.tm_sec = second;
        targetTime.tm_isdst = 1;

        time_t targetTimestamp = mktime(&targetTime);
        long long targetMillis = targetTimestamp * 1000LL;

        long long startDelayMillis = targetMillis - currentTimestamp;
        if (startDelayMillis > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(startDelayMillis));

        Start();
    }

    void Stop() {
        if (!running)
            return;

        running = false;
        pthread_join(producerThread, nullptr);
        pthread_join(consumerThread, nullptr);

        if (stopFcn)
            stopFcn();
    }

private:
    static void *ProducerThreadFuncHelper(void *arg) {
        auto timer = static_cast<Timer *>(arg);
        timer->produce();
        return nullptr;
    }

    static void *ConsumerThreadFuncHelper(void *arg) {
        auto timer = static_cast<Timer *>(arg);
        timer->consume();
        return nullptr;
    }

    void produce() {
        while (running) {
            timeval currentTime{};

            gettimeofday(&currentTime, nullptr);
            long long currentTimestamp = currentTime.tv_sec * 1000LL + currentTime.tv_usec / 1000LL;

            if (currentTimestamp >= nextExecutionTime) {
                if (taskCount < tasksToExecute) {
                    workFunction task;
                    task.timerFcn = timerFcn;
                    task.userData = &currentTimestamp;

                    pthread_mutex_lock(&queueMutex);
                    taskQueue.push(task);
                    taskCount++;

                    nextExecutionTime = currentTimestamp + period;

                    pthread_cond_signal(&queueCondition);
                    pthread_mutex_unlock(&queueMutex);
                } else {
                    running = false;
                }
            } else {
                long long timeToSleep = nextExecutionTime - currentTimestamp;
                usleep(timeToSleep * 1000);
            }
        }
    }

    void consume() {
        while (running) {
            workFunction task;

            pthread_mutex_lock(&queueMutex);
            while (taskQueue.empty() && running)
                pthread_cond_wait(&queueCondition, &queueMutex);

            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
            }
            pthread_mutex_unlock(&queueMutex);

            if (!task.timerFcn) {
                std::this_thread::yield();
                continue;
            }

            auto data = static_cast<long long *>(task.userData);
            task.timerFcn(data);
        }
    }

    int period;
    int tasksToExecute;
    int startDelay;
    StartFcn startFcn;
    StopFcn stopFcn;
    TimerFcn timerFcn;
    ErrorFcn errorFcn;
    void *userData;

    pthread_t producerThread;
    pthread_t consumerThread;
    std::queue<workFunction> taskQueue;
    pthread_mutex_t queueMutex;
    pthread_cond_t queueCondition;

    bool running;
    int taskCount;
    long long nextExecutionTime;
};

int main() {
    constexpr int period = 1000; // milliseconds
    constexpr int tasksToExecute = 3600 * 1000 / period;
    int startDelay = 1; // seconds
    StartFcn startFcn = MyStartFcn;
    StopFcn stopFcn = MyStopFcn;
    TimerFcn timerFcn = MyTimerFcn;
    auto userData = new(double);
    *userData = 3.14;
    ErrorFcn errorFcn = MyErrorFcn;

    Timer timer(period, tasksToExecute, startDelay, startFcn, stopFcn, timerFcn, errorFcn, userData);
//    timer.Start();

    // Start the timer at a specific time
    timer.StartAt(2023, 7, 11, 20, 23, 30);

    // Wait for the timer to finish
    std::this_thread::sleep_for(std::chrono::hours (1));

    return 0;
}
