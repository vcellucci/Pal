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

namespace Pal {
    
template<class T>
class ConcurrentStack
{
protected:
    struct Node
    {
        T data;
        std::shared_ptr<Node> next;
        
        Node(const T& data)
        :data(data)
        ,next(nullptr)
        {}
        
        Node()
        :next(nullptr)
        {}
    };
    
public:
    ConcurrentStack()
    :head(nullptr)
    ,numElements(0)
    {
    }
    
    void push(const T& data)
    {
        auto newNode = std::make_shared<Node>(data);
        newNode->next = std::atomic_load(&head);
        while (!std::atomic_compare_exchange_weak(&head, &newNode->next, newNode));
        numElements.fetch_add(1, std::memory_order_acq_rel);
    }
    
    bool pop(T& data)
    {
        auto oldHead = std::atomic_load(&head);
        while(oldHead && !std::atomic_compare_exchange_weak(&head, &oldHead, oldHead->next));
        if( oldHead )
        {
            data = oldHead->data;
            numElements.fetch_sub(1, std::memory_order_acq_rel);
            return true;
        }
        return false;
    }

    bool empty() const
    {
        return  !head.get();
    }
    
    std::size_t size() const
    {
        return numElements.load(std::memory_order_acquire);
    }
    
protected:
   // std::atomic<Node*> head;
    std::shared_ptr<Node> head;
    std::atomic<std::size_t> numElements;
};
    
}

#endif
