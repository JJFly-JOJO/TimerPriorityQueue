#ifndef TIMERPRIORITYQUEUE_TIMERPRIORITYQUEUE_H
#define TIMERPRIORITYQUEUE_TIMERPRIORITYQUEUE_H

#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

class Timer {
   public:
    Timer(unsigned long long expire, std::function<void(void)> f, void* args)
        : expireTime(expire), f_callback(f), args(args)
    {}

    inline void action() { f_callback(); }

    inline unsigned long long getExpireTime() { return expireTime; }

   private:
    unsigned long long expireTime;
    std::function<void(void)> f_callback;
    void* args;
};

class TimerManager {
   public:
    static void handler(TimerManager* tm);

    TimerManager()= default;

    Timer* addTimer(int timeout, std::function<void(void)> f,
                    void* args = NULL);

    void delTimer(Timer* timer);

    unsigned long long getRecentTimeout();

    void takeAllTimeout();

    unsigned long long getCurrentMillisecs();

    std::thread* run();

   private:
    struct cmp
    {
        bool operator()(Timer* a, Timer* b) const
        {
            return a->getExpireTime() > b->getExpireTime();
        }
    };

    std::priority_queue<Timer*, std::vector<Timer*>, cmp> queue_;
};

#endif  // TIMERPRIORITYQUEUE_TIMERPRIORITYQUEUE_H
