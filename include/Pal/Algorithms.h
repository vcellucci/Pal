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
#include <iterator>
#include "TaskPool.h"
#include "AlignedAllocator.h"
#include "details/spmc_queue.h"
#include "details/parition.h"

namespace Pal {
    
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
    
template<class Iterator>
struct chunk_range
{
    Iterator begin;
    Iterator end;
    
    chunk_range(const Iterator& b, const Iterator& e)
    :begin(b)
    ,end(e)
    {}
};
    
template<>
struct chunk_range<int>
{
    int begin;
    int end;
    
    chunk_range(int b, int e)
    :begin(b)
    ,end(e)
    {}
};
    
template<class Iterator, class Callable>
void parallel_for_each_range(Iterator begin, Iterator end, Callable callable)
{
    TaskPool<details::spmc_queue, void(chunk_range<Iterator>)> taskPool;
    
    std::size_t numElements = distance_wrapper(begin, end);
    details::range_partition<std::size_t> parts = details::partition(taskPool.getNumThreads(), numElements);
    
    auto it = begin;
    for (std::size_t i = 0; i < parts.chunks; i++)
    {
        auto first = it;
        auto last  = it + parts.chunk_size;
        chunk_range<Iterator> chunk(first, last);
        taskPool.submit(callable, chunk);
        it += parts.chunk_size;
    }
    
    if(parts.left_over)
    {
        auto last = it + parts.left_over;
        chunk_range<Iterator> chunk(it, last);
        taskPool.submit(callable, chunk);
    }
}
    
template<class Iterator, class Callable>
void parallel_for_each(Iterator begin, Iterator end, Callable callable)
{
    TaskPool<details::spmc_queue, void( Iterator, Iterator ) > taskPool;
    
    parallel_for_each_range(begin, end, [callable](chunk_range<Iterator> range)
    {
        for(auto it = range.begin; it != range.end; ++it)
        {
            callable(std::ref(*it));
        }
    });
}
    
template<class Iterator, class BinaryFunctor>
typename std::iterator_traits<Iterator>::value_type parallel_reduce(Iterator begin, Iterator end, BinaryFunctor functor)
{
    using val_type = typename std::iterator_traits<Iterator>::value_type;
    
    TaskPool<details::spmc_queue, val_type( Iterator, Iterator ) > taskPool;
    
    std::size_t numElements = (distance_wrapper(begin, end));
    details::range_partition<std::size_t> parts = details::partition(taskPool.getNumThreads(), numElements);
    
    std::vector<std::future<val_type>, aligned_allocator<std::future<val_type>>> results(parts.chunks + parts.left_over);
    
    auto combine = [functor](Iterator first, Iterator last)
    {
        val_type result = (*first);
        first++;
        for(auto it = first; it != last; ++it)
        {
            result = functor(result, *it);
        }
        return result;
    };
    
    auto it = begin;
    for(std::size_t i = 0; i < parts.chunks; i++ )
    {
        auto last = it + parts.chunk_size;
        results[i] = taskPool.submit(combine, it, last);
        it += parts.chunk_size;
    }
    
    if(parts.left_over)
    {
        auto last = it + parts.left_over;
        std::size_t index = parts.chunks -1 + parts.left_over;
        results[index] = taskPool.submit(combine, it, last);
    }
    
    val_type result = results[0].get();
    for (std::size_t i = 1; i < results.size(); i++)
    {
        result = functor(result, results[i].get());
    }
    
    return result;

}


}


#endif
