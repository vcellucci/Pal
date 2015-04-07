//
//  ConcurrentStackTests.cpp
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-06.
//
//

#include "ConcurrentStackTests.h"
#include "Pal/ConcurrentStack.h"
#include <thread>

TEST_F(ConcurrentStackTests, testPushPop)
{
    Pal::ConcurrentStack<int> stack;
    stack.push(42);
    stack.push(45);
    
    int val;
    stack.pop(val);
    ASSERT_EQ(45, val);
    
    stack.pop(val);
    ASSERT_EQ(42, val);
    
}

TEST_F(ConcurrentStackTests, testPopWhenEmpty)
{
    Pal::ConcurrentStack<int> stack;
    int val;
    ASSERT_FALSE(stack.pop(val));
}

TEST_F(ConcurrentStackTests, testEmpty)
{
    Pal::ConcurrentStack<int> stack;
    ASSERT_TRUE(stack.empty());
    stack.push(3);
    ASSERT_FALSE(stack.empty());
}

TEST_F(ConcurrentStackTests, testSize)
{
    Pal::ConcurrentStack<int> stack;
    ASSERT_EQ(0, stack.size());
    stack.push(1);
    ASSERT_EQ(1, stack.size());
}

TEST_F(ConcurrentStackTests, testMultiThread)
{
    Pal::ConcurrentStack<int> stack;
    auto thread1 = [&stack]()
    {
        stack.push(1);
        stack.push(2);
    };
    
    auto thread2 = [&stack]()
    {
        for( int i = 0; i< 2; i++ )
        {
            int val;
            if(stack.pop(val))
            {
                ASSERT_TRUE(val == 1 || val == 2);
            }
        }
    };
    
    auto thread3 = [&stack]()
    {
        for( int i = 0; i< 2; i++ )
        {
            int val;
            if(stack.pop(val))
            {
                ASSERT_TRUE(val == 1 || val == 2);
            }
        }
    };
    
    auto thread4 = [&stack]()
    {
        for( int i = 0; i< 2; i++ )
        {
            int val;
            if(stack.pop(val))
            {
                ASSERT_TRUE(val == 1 || val == 2);
            }
        }
    };
    
    
    std::thread t1(thread1);
    std::thread t2(thread2);
    std::thread t3(thread3);
    std::thread t4(thread4);
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}


