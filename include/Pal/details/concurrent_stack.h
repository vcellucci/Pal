//
//  concurrent_stack.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-09.
//
//

#ifndef Pal_concurrent_stack_h
#define Pal_concurrent_stack_h

#include <memory>
#include <atomic>

namespace details {
    
template<class T>
class concurrent_stack
{
protected:
    struct node
    {
        T data;
        std::shared_ptr<node> next;
        
        node(const T& _data)
        :data(_data)
        ,next(nullptr)
        {}
        
        node()
        :next(nullptr)
        {}
    };
    
public:
    concurrent_stack()
    :head(nullptr)
    ,num_elements(0)
    {
    }
    
    void push(const T& _data)
    {
        auto new_node = std::make_shared<node>(_data);
        new_node->next = std::atomic_load(&head);
        while (!std::atomic_compare_exchange_weak(&head, &new_node->next, new_node));
        num_elements.fetch_add(1);
    }
    
    bool pop(T& _data)
    {
        auto old_head = std::atomic_load(&head);
        while(old_head && !std::atomic_compare_exchange_weak(&head, &old_head, old_head->next));
        if( old_head )
        {
            _data = old_head->data;
            num_elements.fetch_sub(1);
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
        return num_elements.load();
    }
    
protected:
    std::shared_ptr<node> head;
    std::atomic<std::size_t> num_elements;
};
}
#endif
