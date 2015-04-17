//
//  ThreadSafeQueueTests.cpp
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-17.
//
//

#include "ThreadSafeQueueTests.h"
#include "Pal/ThreadSafeQueue.h"
#include <thread>
#include <vector>

TEST_F(ThreadSafeQueueTests, testQueue)
{
    Pal::ThreadSafeQueue<int> q;
    ASSERT_TRUE(q.empty());
}

TEST_F(ThreadSafeQueueTests, testPush)
{
    Pal::ThreadSafeQueue<int> q;
    q.push(33);
    ASSERT_FALSE(q.empty());
}

TEST_F(ThreadSafeQueueTests, testPop)
{
    int val(0);
    
    Pal::ThreadSafeQueue<int> q;
    ASSERT_FALSE(q.pop(std::move(val)));
    
    q.push(33);
    ASSERT_TRUE(q.pop(std::move(val)));
    ASSERT_EQ(33, val);
    ASSERT_TRUE(q.empty());
}

TEST_F(ThreadSafeQueueTests, testOrder)
{
    int val(-1);
    
    Pal::ThreadSafeQueue<int> q;
    
    q.push(0);
    q.push(1);
    q.push(2);
    
    int test  = 0;
    while (!q.empty())
    {
        ASSERT_TRUE(q.pop(std::move(val)));
        ASSERT_EQ(test, val);
        test++;
    }
   
}

using int_queue = Pal::ThreadSafeQueue<int>;
void consume(std::atomic_bool& done, int* array, int_queue& q)
{
    while (!done.load())
    {
        int val;
        if(q.pop(std::move(val)))
        {
            array[val] = val;
        }
        else
        {
            std::this_thread::yield();
        }
    }
    
    // drain
    while (!q.empty())
    {
        int val;
        if(q.pop(std::move(val)))
        {
            array[val] = val;
        }
        else
        {
            std::this_thread::yield();
        }
        
    }
}

TEST_F(ThreadSafeQueueTests, testParallel)
{
    constexpr int size = 2000;
    int array[size];
    memset(array, -1, sizeof(array));
    int_queue q;
    std::atomic_bool done(false);
    
    auto push = [&]()
    {
        for (int i = 0; i < size; i++)
        {
            int x = i;
            q.push(std::move(x));
        }
        
        done.store(true);
    };
    
    auto cons1 = [&](){ consume(done, array, q); };
    auto cons2 = [&](){ consume(done, array, q); };
    auto cons3 = [&](){ consume(done, array, q); };
    auto cons4 = [&](){ consume(done, array, q); };
    auto cons5 = [&](){ consume(done, array, q); };
    auto cons6 = [&](){ consume(done, array, q); };
    auto c7 = [&](){ consume(done, array, q); };
    auto c8 = [&](){ consume(done, array, q); };
    auto c9 = [&](){ consume(done, array, q); };
    auto c10 = [&](){ consume(done, array, q); };
    auto c11 = [&](){ consume(done, array, q); };
    auto c12 = [&](){ consume(done, array, q); };
    auto c13 = [&](){ consume(done, array, q); };
    auto c14 = [&](){ consume(done, array, q); };
    auto c15 = [&](){ consume(done, array, q); };
    auto c16 = [&](){ consume(done, array, q); };
    
    
    std::vector<std::thread> threads;
    threads.push_back(std::thread(push));
    threads.push_back(std::thread(cons1));
    threads.push_back(std::thread(cons2));
    threads.push_back(std::thread(cons3));
    threads.push_back(std::thread(cons4));
    threads.push_back(std::thread(cons5));
    threads.push_back(std::thread(cons6));
    threads.push_back(std::thread( c7 ));
    threads.push_back(std::thread( c8 ));
    threads.push_back(std::thread( c9 ));
    threads.push_back(std::thread( c10 ));
    threads.push_back(std::thread( c11 ));
    threads.push_back(std::thread( c12 ));
    threads.push_back(std::thread( c13));
    threads.push_back(std::thread( c14));
    threads.push_back(std::thread( c15 ));
    threads.push_back(std::thread( c16 ));
    
    for( auto& t : threads)
    {
        t.join();
    }
    
    for (int i = 0; i < size; i++)
    {
        ASSERT_EQ(i, array[i]);
    }
    
}
