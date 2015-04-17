//
//  RingBufferTests.cpp
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-17.
//
//

#include "RingBufferTests.h"
#include "Pal/RingBuffer.h"
#include <thread>

TEST_F(RingBufferTests, testRingBuffer)
{
    Pal::RingBuffer<int> rb(16);
    ASSERT_TRUE(rb.empty());
}

TEST_F(RingBufferTests, testPush)
{
    Pal::RingBuffer<int> rb(16);
    ASSERT_TRUE(rb.empty());
    ASSERT_TRUE(rb.push(33));
    ASSERT_FALSE(rb.empty());
}

TEST_F(RingBufferTests, testPop)
{
    Pal::RingBuffer<int> rb(16);
    int val = 0;
    ASSERT_FALSE(rb.pop((val)));
    ASSERT_TRUE(rb.push(33));
    ASSERT_FALSE(rb.empty());
    ASSERT_TRUE(rb.pop((val)));
    ASSERT_EQ(33, val);
    ASSERT_TRUE(rb.empty());
}

TEST_F(RingBufferTests, testOverflow)
{
    Pal::RingBuffer<int> rb(16);
    for( int i = 0; i < 16; i++)
    {
        ASSERT_TRUE(rb.push(i));
    }
    
    ASSERT_FALSE(rb.push(44));
    
    int val;
    for(int i = 0; i < 16; i++ )
    {
        ASSERT_TRUE(rb.pop(val));
        ASSERT_EQ(i, val);
    }
    ASSERT_TRUE(rb.empty());
    ASSERT_FALSE(rb.pop(val));
    
    for( int i = 0; i < 16; i++)
    {
        ASSERT_TRUE(rb.push(i));
    }
    
    for(int i = 0; i < 16; i++ )
    {
        ASSERT_TRUE(rb.pop(val));
        ASSERT_EQ(i, val);
    }
    ASSERT_TRUE(rb.empty());
    ASSERT_FALSE(rb.pop(val));
}

TEST_F(RingBufferTests, testParallelPush)
{
    Pal::RingBuffer<int> rb(20);
    
    auto a = [&rb]()
    {
        for (int i = 0; i < 5; i++)
        {
            rb.push(i);
        }
    };
    
    auto b = [&rb]()
    {
        for (int i = 5; i < 10; i++)
        {
            rb.push(i);
        }
    };
    
    auto c = [&rb]()
    {
        for (int i = 10; i < 15; i++)
        {
            rb.push(i);
        }
    };
    
    auto d = [&rb]()
    {
        for (int i = 15; i < 20; i++)
        {
            rb.push(i);
        }
    };
    
    std::thread t1(a);
    std::thread t2(b);
    std::thread t3(c);
    std::thread t4(d);
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    
    ASSERT_FALSE(rb.push(44));
    
    int test[20];
    while(!rb.empty())
    {
        int val;
        rb.pop(val);
        test[val] = val;
    }
    
    for (int i = 0; i < 20; i++)
    {
        ASSERT_EQ(i, test[i]);
    }
    
    
}