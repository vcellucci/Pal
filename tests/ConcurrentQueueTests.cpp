//
//  ConcurrentQueueTests.cpp
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-07.
//
//

#include "ConcurrentQueueTests.h"
#include "Pal/detail/concurrent_queue.h"
#include <array>
#include <thread>

using concurrent_queue = Pal::detail::concurrent_queue<int>;

TEST_F(ConcurrentQueueTests, testDetailQueue)
{
    Pal::detail::concurrent_queue<int> queue;
    ASSERT_EQ(0, queue.size());
    ASSERT_TRUE(queue.empty());
}

TEST_F(ConcurrentQueueTests, testDetailPush)
{
    concurrent_queue queue;
    queue.push(1);
    ASSERT_EQ(1, queue.size());
    ASSERT_FALSE(queue.empty());
}

TEST_F(ConcurrentQueueTests, testDetailPop)
{
    concurrent_queue queue;
    int test;

    ASSERT_FALSE(queue.pop(test));
    queue.push(1);
    queue.push(2);
    queue.push(3);
    
    ASSERT_TRUE(queue.pop(test));
    ASSERT_EQ(1, test);
    ASSERT_EQ(2, queue.size());
    
    ASSERT_TRUE(queue.pop(test));
    ASSERT_EQ(2, test);
    ASSERT_EQ(1, queue.size());

    
    ASSERT_TRUE(queue.pop(test));
    ASSERT_EQ(3, test);
    ASSERT_EQ(0, queue.size());
    ASSERT_TRUE(queue.empty());

}



TEST_F(ConcurrentQueueTests, testDetailPopUntilEmptyThenPush)
{
    concurrent_queue queue;
    int test;
    
    ASSERT_FALSE(queue.pop(test));
    queue.push(1);
    queue.push(2);
    queue.push(3);
    
    ASSERT_TRUE(queue.pop(test));
    ASSERT_EQ(1, test);
    ASSERT_EQ(2, queue.size());
    
    ASSERT_TRUE(queue.pop(test));
    ASSERT_EQ(2, test);
    ASSERT_EQ(1, queue.size());
    
    
    ASSERT_TRUE(queue.pop(test));
    ASSERT_EQ(3, test);
    ASSERT_EQ(0, queue.size());
    ASSERT_TRUE(queue.empty());
    
    queue.push(4);
    ASSERT_TRUE(queue.pop(test));
    ASSERT_EQ(4, test);
    ASSERT_EQ(0, queue.size());
    ASSERT_TRUE(queue.empty());
}

TEST_F(ConcurrentQueueTests, testMultiThread)
{
    std::array<int, 200000> testArray;
    concurrent_queue queue;
    auto push1 = [&queue]()
    {
        for (int i = 0; i < 100000; i++) {
            queue.push(i);
        }
    };
    
    auto push2 = [&queue]()
    {
        for (int i = 100000; i < 200000; i++) {
            queue.push(i);
        }
    };
    
    auto pop1 = [&queue, &testArray]()
    {
        for (int i = 0; i < 50000; i++) {
            int temp = -1;
            if(queue.pop(temp))
            {
                testArray[temp] = temp;
            }
        }
    };
    
    auto pop1x = [&queue, &testArray]()
    {
        for (int i = 0; i < 50000; i++) {
            int temp = -1;
            if(queue.pop(temp))
            {
                testArray[temp] = temp;
            }
        }
    };
    
    auto pop2 = [&queue, &testArray]()
    {
        while(!queue.empty())
        {
            int temp = -1;
            if(queue.pop(temp))
            {
                testArray[temp] = temp;
            }
        }
    };
    
    auto pop3 = [&queue, &testArray]()
    {
        while(!queue.empty())
        {
            int temp = -1;
            if(queue.pop(temp))
            {
                testArray[temp] = temp;
            }
        }
    };

    std::thread t1(push1);
    std::thread t2(push2);
    std::thread t3(pop1);
    std::thread tx(pop1x);
    
    
    t1.join();
    t2.join();
    t3.join();
    tx.join();
    
    std::thread t4(pop2);
    std::thread t5(pop3);
    
    t4.join();
    t5.join();
    
    
    
    for( int i = 0; i < testArray.size(); i++ )
    {
        ASSERT_EQ(i, testArray[i]);
    }
    
}