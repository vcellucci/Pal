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
    
    using TaskPoolWorkerPtr = std::shared_ptr<TaskPoolWorker<ContainerT, R(Args...)>>;
    
    TaskPool()
    : currentThread(0)
    {
        numThreads = std::thread::hardware_concurrency()*4;
        startThreads();
    }
    
    
    template<class ...FuncArgs>
    std::future<R> submit(std::function<R(Args...)> task, FuncArgs&&... funcArgs)
    {
        auto future = workers[currentThread]->worker->push(task, funcArgs...);
        currentThread = (currentThread + 1) % numThreads;
        return future;
    }
    
    std::size_t getNumThreads() const
    {
        return numThreads;
    }
    
    void stop()
    {
        workers.clear();
    }
    
protected:
    void startThreads()
    {
        std::vector<TaskPoolWorkerPtr> taskWorkers;
        for( std::size_t i = 0; i < numThreads; i++ )
        {
            WorkerPtr worker = std::make_shared<Worker>(i);
            taskWorkers.push_back(worker->worker);
            workers.push_back(std::move(worker));
        }
        
        for(WorkerPtr& worker : workers)
        {
            worker->worker->addOtherWorkers(taskWorkers);
            worker->start();
        }
    }
    
protected:

    struct Worker
    {
        TaskPoolWorkerPtr worker;
        std::thread workerThread;
        
        Worker(std::size_t idx)
        {
            worker = std::make_shared<TaskPoolWorker<ContainerT, R(Args...)>>(idx);
        }
        
        ~Worker()
        {
            worker->stop();
            if(workerThread.joinable())
            {
                workerThread.join();
            }
        }
        
        void start()
        {
            workerThread = std::thread(std::ref(*worker));
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