//
//  RingBuffer.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-17.
//
//

#ifndef Pal_RingBuffer_h
#define Pal_RingBuffer_h

#include <cstdint>
#include <atomic>
#include "AlignedAllocator.h"

namespace Pal {
    
template<class T>
class RingBuffer
{
public:
    RingBuffer(int32_t max)
    : maxSize(max)
    , size(0)
    , readIndex(0)
    , writeIndex(0)
    {
        arrayBuffer = aligned_allocator<T>().allocate(maxSize);
    }
    
    ~RingBuffer()
    {
        aligned_allocator<T>().deallocate(arrayBuffer);
    }
    
    bool empty()
    {
        return (size.load() == 0);
    }
    
    bool push(const T& val)
    {
        if( size.load() >= maxSize )
        {
            return false;
        }
        
        auto oldWriteIdx = writeIndex.load();
        auto writeIdx = (oldWriteIdx + 1) % maxSize;
        while (!writeIndex.compare_exchange_weak(oldWriteIdx, writeIdx))
        {
            if(size.load() >= maxSize)
            {
                return false;
            }
            writeIdx = (oldWriteIdx + 1) % maxSize;
        }
        
        arrayBuffer[oldWriteIdx] = val;
        size++;
        return true;
    }
    
    bool pop(T& val)
    {
        if( size.load() == 0)
        {
            return false;
        }
        
        val = arrayBuffer[readIndex.load()];
        readIndex = (readIndex + 1) % maxSize;
        size--;
        return true;
    }
    
protected:
    int32_t maxSize;
    std::atomic_int size;
    std::atomic_int readIndex;
    std::atomic_int writeIndex;
    T* arrayBuffer;
    
};
    
}
#endif
