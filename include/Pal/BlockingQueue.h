#ifndef __PAL__BLOCKINGQUEUE_H__
#define __PAL__BLOCKINGQUEUE_H__

#include <deque>
#include <condition_variable>
#include <mutex>

namespace Pal{
 
template<class T>
class BlockingQueue
{
public:
    
    BlockingQueue()
    :stopped(false)
    {
    }
    
    void push(const T& val)
    {
        std::unique_lock<std::mutex> lk(monitor);
        container.push_back(val);
        notEmpty.notify_one();
    }
    
    std::size_t size() 
    {
        std::unique_lock<std::mutex> lk(monitor);
        return container.size();
    }
    
    bool pop(T& val)
    {
        std::unique_lock<std::mutex> lk(monitor);
        while(container.empty() && !stopped) notEmpty.wait(lk);
        
        if( container.empty() )
        {
            return false;
        }

        val = container.front();
        container.pop_front();
        return true;
    }
    
    bool empty()
    {
        std::unique_lock<std::mutex> lk(monitor);
        return container.empty();
    }
    
    void stop()
    {
        std::unique_lock<std::mutex> lk(monitor);
        stopped = true;
        notEmpty.notify_one();
    }
    
protected:
    std::deque<T> container;
    std::mutex monitor;
    std::condition_variable notEmpty;
    bool stopped;
};

}
#endif