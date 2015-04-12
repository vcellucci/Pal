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
    
    using WorkQueue = ContainerT<std::packaged_task<R()>>;


    TaskPoolWorker()
    : working( true )
    {
    }
    
    explicit TaskPoolWorker(std::size_t _idx)
    : working(true)
    , idx(_idx)
    {
        
    }
    
    bool isWorking() const
    {
        return working.load(std::memory_order_acquire);
    }
    
    std::size_t getIdx() const
    {
        return idx;
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
    
    void add(TaskPoolWorker* worker)
    {
        if( worker != this)
        {
            otherWorkers.push_back(worker);
        }
    }
    
    void operator()()
    {
        while(working.load(std::memory_order_acquire))
        {
            if(!excuteNextTask(workQueue) && working.load(std::memory_order_acquire))
            {
                stealWork();
                std::this_thread::yield(); // always yield after a steal attempt
            }
        }

        drain();
    }

protected:

    inline void drain()
    {
        while (!workQueue.empty())
        {
            excuteNextTask(workQueue);
        }
    }

    bool stealWork()
    {
        for(TaskPoolWorker* worker : otherWorkers)
        {
            if( worker->isWorking() && !worker->workQueue.empty())
            {
                if(excuteNextTask(worker->workQueue))
                {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    inline bool excuteNextTask(ContainerT<std::packaged_task<R()>>& queue)
    {
        std::packaged_task < R() > t;
        if (queue.pop(std::move(t)))
        {
            t();
            return true;
        }
        else
        {
            std::this_thread::yield();
        }
        
        return false;
    }
    
    

protected:
    WorkQueue workQueue;
    std::atomic_bool working;
    std::size_t idx;
    std::vector<TaskPoolWorker*> otherWorkers;
};
    
}

#endif
