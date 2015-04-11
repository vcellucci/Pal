//
//  free_list.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-10.
//
//

#ifndef Pal_free_list_h
#define Pal_free_list_h


#include <atomic>

namespace details {
    
template<class node_t>
class free_list
{
public:
    
    free_list()
    :head(nullptr)
    {
    }
    
    void push_front(node_t* _node)
    {
        auto old_head = head.load();
        while (!head.compare_exchange_weak(old_head, _node)) {}
        _node->next = old_head;
    }
    
    void free()
    {
        auto old_head = head.exchange(nullptr);
        while (old_head)
        {
            auto temp = old_head;
            old_head = old_head->next;
            delete temp;
        }
    }
    
protected:
    std::atomic<node_t*> head;
};
}


#endif
