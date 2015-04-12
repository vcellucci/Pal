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
        std::size_t index = (currentThread + 1) % numThreads;
        return workers[index]->worker.push(task, funcArgs...);
    }
    
protected:
    void startThreads()
    {
        for( std:: size_t i = 0; i < numThreads; i++ )
        {
            WorkerPtr worker(new Worker());
            workers.push_back(std::move(worker));
        }
    }
    
protected:
    struct Worker
    {
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
    using WorkerPtr = std::unique_ptr<Worker>;
    std::vector<WorkerPtr> workers;
    std::size_t numThreads;
    std::size_t currentThread;
};

}

#endif