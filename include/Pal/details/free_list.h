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
    
    ~free_list()
    {
        free();
    }
    
    void push_front(node_t* _node)
    {
        auto temp = _node;
        temp->next = nullptr;
        auto old_head = head.load();
        while (!head.compare_exchange_weak(old_head, _node))
        {
            _node = temp;
        }
        _node->next.store(old_head);
        
    }
    
    bool free()
    {
        auto old_head = head.load();
        if(head.compare_exchange_strong(old_head, nullptr) )
        {
            while (old_head)
            {
                auto temp = old_head;
                old_head = old_head->next.load();
                delete temp;
            }
            return true;
        }
        return false;
    }
    
protected:
    std::atomic<node_t*> head;
};
}


#endif
