//
//  concurrent_queue.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-07.
//
//

#ifndef Pal_concurrent_queue_h
#define Pal_concurrent_queue_h

#include <atomic>
#include <memory>

namespace Pal {
    namespace detail{
        
template<class data_t>
class concurrent_queue
{
public:
    concurrent_queue()
    :num_elements(0)
    ,_consumer_lock(false)
    ,head(nullptr)
    ,tail(nullptr)
    {
    }
    
    std::size_t size() const
    {
        return num_elements.load();
    }
    
    bool empty() const
    {
        return( !head.load() );
    }
    
    void push(const data_t& data)
    {

        auto new_node = new node(data);
        while(_consumer_lock.exchange(true)){}

        auto old_tail = tail.load();
        while(!tail.compare_exchange_weak(old_tail, new_node));
        if(old_tail)
        {
            old_tail->next.store(new_node);
        }
        

        auto old_head = head.load();
        if( !old_head)
        {
            head.compare_exchange_strong(old_head, new_node);
        }
        
        _consumer_lock = false;
        num_elements++;
        
    }
    
    bool pop(data_t& val)
    {
        while(_consumer_lock.exchange(true)){}
        bool success = false;
        
        
        auto old_head = head.load();
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next)) {}
        if( old_head )
        {
            val = old_head->data;
            num_elements--;
            success = true;
            delete old_head;
        }
        else
        {
            success = false;
            tail.store(nullptr);
        }
        
        if( !head.load())
        {
            tail.store(nullptr);
        }
        _consumer_lock = false;
        return success;
    }
    
protected:
    struct node
    {
        data_t data;
        std::atomic<node*> next;
        
        node():next(nullptr){}
        
        node(const data_t& d)
        :data(d)
        ,next(nullptr)
        {}
    };
    
protected:
    std::atomic<std::size_t> num_elements;
    std::atomic<node*> head;
    std::atomic<node*> tail;
    std::atomic_bool _consumer_lock;
    
   
};
        
}}

#endif
