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

/*
 Note: This is designed as a single-producer, multi-consumer queue(single-writer, multi-readers).  It has not been tested
 for multi-writer
 */

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
        tail.store(head.load());
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
    
    // designed for single producer only
    void push(const data_t& _data)
    {
        auto old_tail = tail.load();
        auto new_node = new node(_data);

        old_tail->next.store(new_node);
        tail.store(new_node);
        elements++;
    }
        
    void push( data_t&& _data)
    {
        auto old_tail = tail.load();
        auto new_node = new node(std::move(_data));
        
        old_tail->next.store(new_node);
        tail.store(new_node);
        elements++;
    }
    
    bool pop(data_t& _data)
    {
        thread_count_sentry sentry(threads_in_pop);
        
        auto old_head = update_head_for_current_thread();
        if(!old_head)
        {
            return false;
        }
        
        auto next_node = old_head->next.load();
        if(next_node)
        {
            _data = std::move(next_node->data);
            try_reclaim(old_head);
            elements--;
            return true;
        }
        
        try_reclaim(old_head);
        return false;
    }
    
    bool pop(data_t&& _data)
    {
        thread_count_sentry sentry(threads_in_pop);
        
        auto old_head = update_head_for_current_thread();
        if(!old_head)
        {
            return false;
        }
        
        auto next_node = old_head->next.load();
        if(next_node)
        {
            _data = std::move(next_node->data);
            try_reclaim(old_head);
            elements--;
            return true;
        }
        
        try_reclaim(old_head);
        return false;
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
        std::atomic<node*>  next;
        
        node()
        : next(nullptr)
        {}
        
        node(const data_t& _data)
        : data(_data)
        , next(nullptr)
        {}
        
        node(data_t&& _data)
        : next(nullptr)
        {
            data = std::move(_data);
        }
    };
    
    struct thread_count_sentry
    {
        std::atomic_int& thread_count;
        thread_count_sentry( std::atomic_int& c)
        :thread_count(c)
        {
            thread_count++;
        }
        
        ~thread_count_sentry()
        {
            thread_count--;
        }	
    };
    
protected:
    // updates the head pointer and returns the old value
    node* update_head_for_current_thread()
    {
        node* old_head = nullptr;
        bool set = false;
        while(!set)
        {
            old_head = head.load();
            if(!old_head || old_head == tail.load())
            {
                return nullptr;
            }
            
            set = head.compare_exchange_strong(old_head, old_head->next.load());
        }
        
        return old_head;
    }
    
    void try_reclaim(node* n)
    {
        if( !n )
            return;
        
        if(threads_in_pop.load() == 1)
        {
            if(pending_delete_nodes.free()) // was it really the only thread?
            {
                delete n;
            }
            else
            {
                pending_delete_nodes.push_front(n);
            }
        }
        else
        {
            pending_delete_nodes.push_front(n);
        }
    }
    
protected:
    std::atomic<node*> head;
    std::atomic<node*> tail;
    std::atomic<std::size_t> elements;
    std::atomic_int threads_in_pop;
    free_list<node> pending_delete_nodes;
};
    
    
}
#endif
