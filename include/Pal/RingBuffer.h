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
#include <mutex>
#include "AlignedAllocator.h"
#include "Spinlock.h"


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
        std::lock_guard<Spinlock> lk(monitor);
        return  (size == 0);
    }
    
    bool push(const T& val)
    {
        std::lock_guard<Spinlock> lk(monitor);
        
        if( size >= maxSize )
        {
            return false;
        }
        arrayBuffer[writeIndex] = val;
        writeIndex = (writeIndex + 1) % maxSize;
        size++;
        return true;
    }
    
    bool pop(T& val)
    {
        std::lock_guard<Spinlock> lk(monitor);

        if( size == 0)
        {
            return false;
        }
        
        val = arrayBuffer[readIndex];
        readIndex = (readIndex + 1) % maxSize;
        size--;
        return true;
    }
    
protected:
    uint32_t maxSize;
    uint32_t size;
    uint32_t readIndex;
    uint32_t writeIndex;
    T* arrayBuffer;
    Spinlock monitor;
    
};
    
}
#endif
