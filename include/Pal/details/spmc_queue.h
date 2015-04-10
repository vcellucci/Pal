//
//  spmc_queue.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-09.
//
//

#ifndef Pal_spmc_queue_h
#define Pal_spmc_queue_h

#include <atomic>
#include "free_list.h"

namespace details {
    
template<class data_t>
class spmc_queue
{
public:
    spmc_queue()
    :elements(0)
    ,threads_in_pop(0)
    {
        head.store(new node());
        tail.store(head);
    }
    
    bool empty() const
    {
        auto old_head = head.load();
        return old_head == tail.load();
    }
    
    std::size_t size() const
    {
        return elements.load();
    }
    
    void push(const data_t& _data)
    {
        auto new_node = new node(_data);
        auto old_tail = tail.load();
        old_tail->next = new_node;
        tail.store(new_node);
        elements.fetch_add(1);
    }
    
    bool pop(data_t& _data)
    {
        thread_count_sentry sentry(threads_in_pop);
        
        node* old_head;
        do
        {
            old_head = head.load();
            if(old_head == tail.load())
            {
                return false;
            }
        }
        while (!head.compare_exchange_weak(old_head, old_head->next)) ;
        
        _data = old_head->next->data;
        
        try_reclaim(old_head);
        
        return true;
    }
    
    bool is_lock_free()
    {
        
        return (head.is_lock_free() &&
                tail.is_lock_free() &&
                elements.is_lock_free());
    }

    
protected:
    struct node
    {
        data_t data;
        node*  next;
        
        node():next(nullptr){}
        
        node(const data_t& _data):data(_data), next(nullptr){}
    };
    
    struct thread_count_sentry
    {
        thread_count_sentry(std::atomic_int& _count)
        :count(_count)
        {
            count ++;
        }
        
        ~thread_count_sentry()
        {
            count --;
        }
        
        std::atomic_int& count;
    };
    
protected:
    void try_reclaim(node* n)
    {
        if(threads_in_pop.load() == 1)
        {
            nodes_to_be_deleted.free();
            delete n;
        }
        else
        {
            nodes_to_be_deleted.push(n);
        }
    }
    
protected:
    std::atomic<node*> head;
    std::atomic<node*> tail;
    std::atomic<std::size_t> elements;
    std::atomic_int threads_in_pop;
    free_stack<node> nodes_to_be_deleted;
};
    
}
#endif