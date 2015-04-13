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
#include <memory>

namespace Pal{
    
template<template<class> class, class> class TaskPoolWorker;

template< template<class> class ContainerT, class R, class ...Args>
class TaskPoolWorker<ContainerT,R(Args...)>
{
public:
    
    using WorkQueue = ContainerT<std::packaged_task<R()>>;
    
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
    
    void addOtherWorkers(const std::vector<std::shared_ptr<TaskPoolWorker>>& workers)
    {
        otherWorkers = workers;
    }
    
    void operator()()
    {
        while(working.load(std::memory_order_acquire))
        {
            if( workQueue.empty() && working.load(std::memory_order_acquire))
            {
                std::this_thread::yield();
                stealWork();
            }
            else if( working.load(std::memory_order_acquire ) )
            {
                executeNextTask(workQueue);
            }
        }

        drain();
    }

protected:

    inline void drain()
    {
        while (!workQueue.empty())
        {
            executeNextTask(workQueue);
        }
    }
    
    void stealWork()
    {
        for( auto& worker : otherWorkers )
        {
            if(worker.get() != this)
            {
                WorkQueue* queue = worker->getQueue();
                if( queue )
                {
                    if(executeNextTask(*queue) )
                    {
                        return;
                    }
                }
            }
        }
    }

    inline bool executeNextTask(WorkQueue &queue)
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
    
    WorkQueue* getQueue()
    {
        if(working.load() && !workQueue.empty())
        {
            return &workQueue;
        }
        
        return nullptr;
    }
    
protected:
    WorkQueue workQueue;
    std::vector<std::shared_ptr<TaskPoolWorker>> otherWorkers;
    std::atomic_bool working;
    std::size_t idx;
};
    
}

#endif
