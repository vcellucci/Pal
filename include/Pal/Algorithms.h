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
    
template<class Iterator>
struct chunk_range
{
    Iterator begin;
    Iterator end;
    
    chunk_range(const Iterator& b, const Iterator& e)
    :begin(b)
    ,end(e)
    {
        
    }
};
    
template<>
struct chunk_range<int>
{
    int begin;
    int end;
    
    chunk_range(int b, int e)
    :begin(b)
    ,end(e)
    {
        
    }
};
    
template<class Iterator>
std::size_t distance_wrapper(Iterator begin, Iterator end)
{
    return std::distance(begin, end);
}
    
template<>
std::size_t distance_wrapper<int>(int begin, int end)
{
    return (end - begin);
}
    
template<class Iterator, class Callable>
void for_each_range(Iterator begin, Iterator end, Callable callable)
{
    TaskPool<details::spmc_queue, void(chunk_range<Iterator>)> taskPool;
    
    std::size_t numThreads = taskPool.getNumThreads();
    std::size_t numElements = distance_wrapper(begin, end);
    std::size_t chunkSize   = numElements / numThreads;
    std::size_t numChunks   = numElements / chunkSize;
    std::size_t leftOver    = numElements - (numChunks*chunkSize);
    
    auto it = begin;
    for (std::size_t i = 0; i < numChunks; i++)
    {
        auto first = it;
        auto last  = it + chunkSize;
        chunk_range<Iterator> chunk(first, last);
        taskPool.submit(callable, chunk);
        it += chunkSize;
    }
    
    if(leftOver)
    {
        auto last = it + leftOver;
        chunk_range<Iterator> chunk(it, last);
        taskPool.submit(callable, chunk);
    }
}
    
template<class Iterator, class Callable>
void for_each(Iterator begin, Iterator end, Callable callable)
{
    TaskPool<details::spmc_queue, void( Iterator, Iterator ) > taskPool;
    
    for_each_range(begin, end, [callable](chunk_range<Iterator> range)
    {
        for(auto it = range.begin; it != range.end; ++it)
        {
            callable(std::ref(*it));
        }
    });
}
    
}


#endif
