//
//  AlignedAllocator.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-13.
//
//

#ifndef Pal_AlignedAllocator_h
#define Pal_AlignedAllocator_h

#include <memory>
#include <cstdlib>
#include <stdexcept>

namespace Pal {
    
template <typename T>
struct aligned_allocator : public std::allocator<T>
{
    typedef size_t size_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    
    pointer allocate(size_type pCount, const_pointer = 0)
    {
        pointer mem = 0;
        if (posix_memalign(reinterpret_cast<void**>(&mem), 16, sizeof(T) * pCount) != 0)
        {
            throw std::bad_alloc(); // or something
        }
        
        return mem;
    }
    
    void deallocate(pointer pPtr, size_type pCount = 0)
    {
        free(pPtr);
    }
    
};
    
}


#endif
