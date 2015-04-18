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
#include "../AlignedAllocator.h"
#include "../Spinlock.h"


namespace details {
    
template<class T>
class ring_buffer
{
public:
    ring_buffer(int32_t max)
    : max_size(max)
    , size(0)
    , read_index(0)
    , write_index(0)
    {
        array_buffer = Pal::aligned_allocator<T>().allocate(max_size);
    }
    
    ~ring_buffer()
    {
        Pal::aligned_allocator<T>().deallocate(array_buffer);
    }
    
    bool empty()
    {
        std::lock_guard<Pal::Spinlock> lk(monitor);
        return  (size == 0);
    }
    
    bool push(const T& val)
    {
        std::lock_guard<Pal::Spinlock> lk(monitor);
        
        if( size >= max_size )
        {
            return false;
        }
        array_buffer[write_index] = val;
        write_index = (write_index + 1) % max_size;
        size++;
        return true;
    }
    
    bool pop(T& val)
    {
        std::lock_guard<Pal::Spinlock> lk(monitor);

        if( size == 0)
        {
            return false;
        }
        
        val = array_buffer[read_index];
        read_index = (read_index + 1) % max_size;
        size--;
        return true;
    }
    
protected:
    uint32_t max_size;
    uint32_t size;
    uint32_t read_index;
    uint32_t write_index;
    T* array_buffer;
    Pal::Spinlock monitor;
    
};
    
}
#endif
