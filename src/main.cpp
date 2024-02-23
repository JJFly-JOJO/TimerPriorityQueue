#include <iostream>
#include <unistd.h>
#include "TimerPriorityQueue.h"

int main()
{
    TimerManager tm;
    tm.addTimer(1000, []() { std::cout << "1000ms trigger" << std::endl; });
    tm.addTimer(2000, []() { std::cout << "2000ms trigger" << std::endl; });
    tm.addTimer(3000, []() { std::cout << "3000ms trigger" << std::endl; });
    tm.addTimer(4000, []() { std::cout << "4000ms trigger" << std::endl; });

    tm.run();
    while(1)
    {
        sleep(1);
    }
}