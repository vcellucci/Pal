//
//  ThreadSafeQueue.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-17.
//
//

#ifndef Pal_ThreadSafeQueue_h
#define Pal_ThreadSafeQueue_h

#include "Spinlock.h"
#include <mutex>
#include <deque>

namespace Pal {
    
template<class T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue()
    {
        
    }
    
    
    bool empty()
    {
        std::lock_guard<Spinlock> lk(lock);
        return queue.empty();
    }
    
    void push(T&& t)
    {
        std::lock_guard<Spinlock> lk(lock);
        queue.push_back(t);
    }
    
    bool pop(T&& t)
    {
        std::lock_guard<Spinlock> lk(lock);
        if( queue.empty() )
        {
            return false;
        }
        
        t = std::move(queue.front());
        queue.pop_front();
        return true;
    }
    
protected:
    std::deque<T> queue;
    Spinlock lock;
};
    
}

#endif
