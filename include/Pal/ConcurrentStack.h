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

namespace Pal {
    
template<class T>
class ConcurrentStack
{
protected:
    struct Node
    {
        T data;
        Node* next;
        
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
        Node* newNode = new Node(data);
        newNode->next = head.load();
        while(!head.compare_exchange_weak(newNode->next, newNode));
        numElements.fetch_add(1, std::memory_order_acq_rel);
    }
    
    bool pop(T& data)
    {
        Node* oldHead = head.load();
        while (head && !head.compare_exchange_weak(oldHead, oldHead->next));
        if( oldHead )
        {
            data = oldHead->data;
            delete oldHead;
            numElements.fetch_sub(1, std::memory_order_acq_rel);
            return true;
        }
        return false;
    }

    bool empty() const
    {
        return (head.load() == nullptr);
    }
    
    std::size_t size() const
    {
        return numElements.load(std::memory_order_acquire);
    }
    
protected:
    std::atomic<Node*> head;
    std::atomic<std::size_t> numElements;
};
    
}

#endif
