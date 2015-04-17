//
//  ThreadSafeQueue.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-17.
//
//

#ifndef Pal_ThreadSafeQueue_h
#define Pal_ThreadSafeQueue_h

#include "Spinlock.h"
#include <mutex>
#include <deque>

namespace Pal {
    
template<class T, class Lock=Spinlock>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue()
    {
        head = new node();
        tail = head;
    }
    
    
    bool empty()
    {
        std::lock_guard<Lock> lk(lock);
        return head == tail;
    }
    
    void push(T&& t)
    {
        std::lock_guard<Lock> lk(lock);
        node* newNode = new node(t);
        tail->next = newNode;
        tail = newNode;
    }
    
    bool pop(T&& t)
    {
        std::lock_guard<Lock> lk(lock);
        if( head == tail || head == nullptr)
        {
            return false;
        }
        
        auto temp = head;
        head = head->next;
        if( head == nullptr )
        {
            head = tail;
            return false;
        }
        t = std::move(head->val);
        if( temp )
        {
            delete temp;
        }
        return true;
    }
    
protected:
    struct node
    {
        node* next;
        T val;
        
        node()
        :next(nullptr){}
        
        node(T& v)
        :next(nullptr)
        ,val(v)
        {
            
        }
    };
    Lock lock;
    node* head;
    node* tail;
};
    
}

#endif
