//
//  ref_ptr.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-09.
//
//

#ifndef Pal_ref_ptr_h
#define Pal_ref_ptr_h

#include <atomic>

namespace details {
    
template<class ptr_t>
class ref_ptr
{
public:
    explicit ref_ptr(ptr_t* _ptr)
    : ptr(_ptr)
    , ref_count(1)
    {
    }
    
    ref_ptr()
    : ptr(nullptr)
    , ref_count(0)
    {
    }
    
    ref_ptr(const ref_ptr& rhs)
    {
        ref_count = rhs.increment();
        ptr.store(rhs.ptr.load());
    }
    
    ~ref_ptr()
    {
        if( decrement() == 1 )
        {
            ptr_t* temp = ptr.load();
            if(temp)
            {
                delete temp;
            }
        }
    }
    
    ptr_t* operator->()
    {
        return ptr.load();
    }
    
    operator bool() const
    {
        auto temp = ptr.load();
        return (temp != nullptr);
    }
    
    int count() const
    {
        return ref_count.load();
    }
    
    ref_ptr& operator=(const ref_ptr& rhs)
    {
        ref_count = rhs.increment();
        ptr.store(rhs.ptr.load());
        return *this;
    }
    
    bool operator==( const ref_ptr& rhs) const
    {
        auto p1 = rhs.ptr.load();
        return p1 == ptr.load();
    }
    
    bool operator != (const ref_ptr& rhs) const
    {
        auto p1 = rhs.ptr.load();
        return p1 != ptr.load();
    }
    
protected:
    int decrement() const
    {
        return ref_count.fetch_sub(1);
    }
    
    int increment() const
    {
        return ref_count.fetch_add(1);
    }
    
    
protected:
    std::atomic<ptr_t*> ptr;
    mutable std::atomic_int ref_count;
    
};
    
}

#endif
