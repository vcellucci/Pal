//
//  Spinlock.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-09.
//
//

#ifndef Pal_Spinlock_h
#define Pal_Spinlock_h

#include <atomic>

namespace Pal {
    
class Spinlock
{
public:
    Spinlock()
    {
        flag.clear(std::memory_order_release);
    }
    
    void lock()
    {
        while (flag.test_and_set(std::memory_order_acquire)) {}
    }
    
    void unlock()
    {
        flag.clear(std::memory_order_release);
    }
    
protected:
    std::atomic_flag flag;
};
}

#endif
