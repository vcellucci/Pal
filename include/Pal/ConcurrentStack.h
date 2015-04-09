//
//  ConcurrentStack.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-06.
//
//

#ifndef Pal_ConcurrentStack_h
#define Pal_ConcurrentStack_h

#include <atomic>
#include <memory>
#include "details/concurrent_stack.h"

namespace Pal {
    
template<class T>
class ConcurrentStack
{
public:
    ConcurrentStack() = default;
    
    void push(const T& data)
    {
        stack.push(data);
    }
    
    bool pop(T& data)
    {
        return stack.pop(data);
    }

    bool empty() const
    {
        return stack.empty();
    }
    
    std::size_t size() const
    {
        return stack.size();
    }
    
protected:
    details::concurrent_stack<T> stack;
};
    
}

#endif
