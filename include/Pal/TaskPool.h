//
//  Spinlock.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-09.
//
//
#ifndef __PAL__TASK_POOL_H___
#define __PAL__TASK_POOL_H___


#include <functional>
#include <future>
#include <thread>
#include <memory>
#include <vector>
#include "TaskPoolWorker.h"

namespace Pal{
    
template<template<class> class, class> class TaskPool;

template< template<class> class ContainerT, class R, class ...Args>
class TaskPool<ContainerT,R(Args...)>
{
public:
    
    TaskPool()
    : currentThread(0)
    {
        numThreads = std::thread::hardware_concurrency() * 2;
        startThreads();
    }
    
    template<class ...FuncArgs>
    std::future<R> submit(std::function<R(Args...)> task, FuncArgs&&... funcArgs)
    {
        currentThread = (currentThread + 1) % numThreads;
        return workers[currentThread]->worker.push(task, funcArgs...);
    }
    
protected:
    void startThreads()
    {
        for( std::size_t i = 0; i < numThreads; i++ )
        {
            WorkerPtr worker = std::make_shared<Worker>();
            workers.push_back(std::move(worker));
        }
    }
    
protected:

    struct Worker
    {
        using WorkQueue = typename TaskPoolWorker<ContainerT, R(Args...)>::WorkQueue;
        TaskPoolWorker<ContainerT, R(Args...)> worker;
        std::thread workerThread;
        
        Worker()
        {
            workerThread = std::thread(std::ref(worker));
        }
        
        ~Worker()
        {
            worker.stop();
            if(workerThread.joinable())
            {
                workerThread.join();
            }
        }
        
        void stop()
        {
            worker.stop();
        }
    };
    
protected:
    using WorkerPtr = std::shared_ptr<Worker>;
    
    std::vector<WorkerPtr> workers;
    std::size_t numThreads;
    std::size_t currentThread;
};

}

#endif