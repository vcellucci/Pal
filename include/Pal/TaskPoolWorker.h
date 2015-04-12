//
//  TaskPoolWorker.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-12.
//
//

#ifndef Pal_TaskPoolWorker_h
#define Pal_TaskPoolWorker_h


#include <functional>
#include <future>
#include <atomic>
#include <thread>

namespace Pal{
    
template<template<class> class, class> class TaskPoolWorker;

template< template<class> class ContainerT, class R, class ...Args>
class TaskPoolWorker<ContainerT,R(Args...)>
{
public:

    TaskPoolWorker()
    : working( true )
    {
    }

    template<class ...FuncArgs>
    std::future<R> push(std::function<R(Args...)> task, FuncArgs&&... funcArgs)
    {
        std::function<R()> f = std::bind(task, std::forward<FuncArgs>(funcArgs)...);
        std::packaged_task<R()> t(f);
        auto fut = t.get_future();
        workQueue.push(std::move(t));
        return fut;
    }

    void stop()
    {
        working.store(false, std::memory_order_release);
    }
    
    void operator()()
    {
        while(working.load(std::memory_order_acquire))
        {
            excuteNextTask();
        }

        drain();
    }

protected:

    inline void drain()
    {
        while (!workQueue.empty())
        {
            excuteNextTask();
        }
    }

    inline void excuteNextTask()
    {
        std::packaged_task < R() > t;
        if (workQueue.pop(std::move(t)))
        {
            t();
        }
        else
        {
            std::this_thread::yield();
        }
    }

protected:
    ContainerT<std::packaged_task<R()>> workQueue;
    std::atomic_bool working;
};
    
}

#endif
