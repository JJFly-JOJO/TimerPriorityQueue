#include "TimerPriorityQueue.h"
#include <printf.h>
#include <sys/epoll.h>
#include <sys/time.h>

Timer* TimerManager::addTimer(int timeout, std::function<void(void)> f,
                              void* args)
{
    if (timeout < 0)
    {
        return nullptr;
    }
    unsigned long long expireTime = getCurrentMillisecs() + timeout;
    auto* timer = new Timer(expireTime, f, args);
    queue_.push(timer);
    return timer;
}

unsigned long long TimerManager::getCurrentMillisecs()
{
    struct timespec ts
    {};
    clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / (1000 * 1000);
}

void TimerManager::delTimer(Timer* timer)
{
    std::priority_queue<Timer*, std::vector<Timer*>, cmp> tmpQueue;
    while (!queue_.empty())
    {
        Timer* t = queue_.top();
        if (t != timer)
        {
            tmpQueue.push(t);
        }
    }
    queue_ = tmpQueue;
}

void TimerManager::takeAllTimeout()
{
    unsigned long long now = getCurrentMillisecs();
    while (!queue_.empty())
    {
        Timer* t = queue_.top();
        if (t->getExpireTime() <= now)
        {
            queue_.pop();
            t->action();
            delete t;
            continue;
        }
        break;
    }
}
unsigned long long TimerManager::getRecentTimeout()
{
    long long timeout;
    if (queue_.empty())
    {
        printf("queue is empty,wait 1000ms\n\r");
        return 1000;
    }

    timeout = (long long)queue_.top()->getExpireTime() - getCurrentMillisecs();
    if (timeout < 0)
    {
        timeout = 0;
    }
    return timeout;
}

void TimerManager::handler(TimerManager* tm)
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        std::cerr << "epoll_create1 error" << std::endl;
        return;
    }
//    struct epoll_event ev
//    {
//        .events = EPOLLIN, .data = {.fd = 0 }
//    };
//    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &ev);
    struct epoll_event* events = new epoll_event[1];
    for (;;)
    {
        epoll_wait(epoll_fd, events, 1, tm->getRecentTimeout());
        tm->takeAllTimeout();
    }
}

std::thread* TimerManager::run()
{
    return new std::thread(TimerManager::handler, this);
}
