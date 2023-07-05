#pragma once

#include <iostream>
#include <sys/time.h>

class Timer {
private:
    timeval start{};
    timeval end{};
    timeval difference{};
    int data{};

public:
    Timer(int (*StartFcn)()) {
        data = StartFcn();
        if (gettimeofday(&start, nullptr)) {
            std::cout << "Error during timer creation" << std::endl;
        }
    }

    timeval end_timer() {
        if (gettimeofday(&end, nullptr)) {
            std::cout << "Error during timer ending" << std::endl;
        }
        return end;
    }

    timeval get_difference() {
        timersub(&end, &start, &difference);
        return difference;
    }
};
