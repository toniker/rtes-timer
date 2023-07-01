#include <iostream>
#include <cmath>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

inline constexpr uint8_t QueueSize = 10;
inline constexpr uint8_t NumberOfJobs = 5;

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

struct workFunction {
    void (*work)(double *);

    void *arg;
};

struct queue {
    workFunction buf[QueueSize];
    uint16_t head, tail;
    pthread_mutex_t *mutex;
    pthread_cond_t *has_space, *has_items;
};

queue *initializeQueue() {
    queue *q;
    q = static_cast<queue *>(malloc(sizeof(queue)));
    q->head = 0;
    q->tail = 0;
    q->mutex = static_cast<pthread_mutex_t *>(malloc(sizeof(pthread_mutex_t)));
    q->has_space = static_cast<pthread_cond_t *>(malloc(sizeof(pthread_cond_t)));
    q->has_items = static_cast<pthread_cond_t *>(malloc(sizeof(pthread_cond_t)));
    return q;
}

void deleteQueue(queue *q) {
    pthread_mutex_destroy(q->mutex);
    free(q->mutex);
    pthread_cond_destroy(q->has_space);
    free(q->has_space);
    pthread_cond_destroy(q->has_items);
    free(q->has_items);
    free(q);
}

void addToQueue(queue *q, workFunction input) {
    q->buf[q->tail] = input;
    q->tail++;
    if (q->tail == QueueSize) {
        q->tail = 0;
    }
    pthread_cond_broadcast(q->has_items);
}

void removeFromQueue(queue *q, workFunction *output) {
    *output = q->buf[q->head];
    q->head++;
    if (q->head == QueueSize) {
        q->head = 0;
    }
    pthread_cond_broadcast(q->has_space);
}

void* production(void *arg) {
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

    auto q_ptr = static_cast<queue *>(arg);
    auto q = *q_ptr;
    free(q_ptr);

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
        usleep(sleepDuration);
    }

    return nullptr;
}

void* consumption(void *arg) {
    auto q_ptr = static_cast<queue *>(arg);
    queue q = *q_ptr;
    timeval time{};

    while (true) {
        pthread_cond_wait(q.has_items, q.mutex);
        pthread_mutex_lock(q.mutex);

        workFunction workFunction{};
        removeFromQueue(&q, &workFunction);
        pthread_cond_signal(q.has_space);
        gettimeofday(&time, nullptr);

        workFunction.work(static_cast<double *>(workFunction.arg));
        pthread_mutex_unlock(q.mutex);
    }

    return nullptr;
}

int main() {
    pthread_t producer, consumer;
    queue *queue_ptr = initializeQueue();
    if (queue_ptr == nullptr) {
        std::cout << "Error initializing queue" << std::endl;
        return (EXIT_FAILURE);
    }
    queue queue = *queue_ptr;
    free(queue_ptr);

    pthread_create(&producer, nullptr, &production, nullptr);
    pthread_create(&consumer, nullptr, &consumption, nullptr);

    deleteQueue(&queue);
    return 0;
}