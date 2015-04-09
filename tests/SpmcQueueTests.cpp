//
//  SpmcQueueTests.cpp
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-09.
//
//

#include "SpmcQueueTests.h"
#include "Pal/details/spmc_queue.h"
#include <thread>
#include <vector>

using int_queue = details::spmc_queue<int>;

TEST_F(SpmcQueueTests, testSpmcQueue)
{
    int_queue q;
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(0, q.size());
}

TEST_F(SpmcQueueTests, testPush)
{
    int_queue q;
    q.push(1);
    ASSERT_FALSE(q.empty());
    ASSERT_EQ(1, q.size());
    
}

TEST_F(SpmcQueueTests, testPop)
{
    int_queue q;
    int test;
    ASSERT_FALSE(q.pop(test));
    
    q.push(1);
    ASSERT_TRUE(q.pop(test));
    ASSERT_EQ(1, test);
    ASSERT_TRUE(q.empty());
    
    for(int i = 0; i < 3; i++)
    {
        q.push(i);
    }
    
    for (int i = 0; i < 3; i++)
    {
        ASSERT_TRUE(q.pop(test));
        ASSERT_EQ(i, test);
    }
    
    ASSERT_TRUE(q.empty());
}

void consume(std::atomic_bool& done, int* array, int_queue& q)
{
    while (!done.load())
    {
        int val;
        if(q.pop(val))
        {
            array[val] = val;
        }
    }
    
    // drain
    while (!q.empty())
    {
        int val;
        if(q.pop(val))
        {
            array[val] = val;
        }
    }
}

TEST_F(SpmcQueueTests, testParallel)
{
    constexpr int size = 200000;
    int array[size];
    memset(array, -1, sizeof(array));
    int_queue q;
    std::atomic_bool done(false);
    
    auto push = [&]()
    {
        for (int i = 0; i < size; i++)
        {
            q.push(i);
        }
        
        done.store(true);
    };
    
    auto cons1 = [&](){ consume(done, array, q); };
    auto cons2 = [&](){ consume(done, array, q); };
    auto cons3 = [&](){ consume(done, array, q); };
    
    std::vector<std::thread> threads;
    threads.push_back(std::thread(push));
    threads.push_back(std::thread(cons1));
    threads.push_back(std::thread(cons2));
    threads.push_back(std::thread(cons3));
    
    for( auto& t : threads)
    {
        t.join();
    }
    
    for (int i = 0; i < size; i++)
    {
        ASSERT_EQ(i, array[i]);
    }
    
    ASSERT_TRUE(q.is_lock_free());
}