//
//  Algorithms.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-13.
//
//

#ifndef Pal_Algorithms_h
#define Pal_Algorithms_h

#include <algorithm>
#include "TaskPool.h"
#include "details/spmc_queue.h"

namespace Pal {
    
template<class Iterator, class Callable>
void parallel_for_each(Iterator begin, Iterator end, Callable callable)
{
    TaskPool<details::spmc_queue, void( Iterator, Iterator ) > taskPool;
    
    std::size_t numThreads = taskPool.getNumThreads();
    std::size_t numElements = std::distance(begin, end);
    std::size_t chunkSize   = numElements / numThreads;
    std::size_t numChunks   = numElements / chunkSize;
    std::size_t leftOver    = numElements - (numChunks*chunkSize);
    
    
    auto it = begin;
    for (std::size_t i = 0; i < numChunks; i++)
    {
        auto first = it;
        auto last  = it + chunkSize;
        taskPool.submit(callable, first, last);
        it += chunkSize;
    }
    
    if(leftOver)
    {
        auto last = it + leftOver;
        taskPool.submit(callable, it, last);
    }
}

    
}


#endif
