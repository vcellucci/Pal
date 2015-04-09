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

void consume(std::atomic_bool& done, int* array, Pal::ConcurrentStack<int>& stack)
{
    while (!done.load())
    {
        int val;
        if(stack.pop(val))
        {
            array[val] = val;
            
        }
    }
    
    while (!stack.empty())
    {
        
        int val;
        if(stack.pop(val))
        {
            array[val] = val;
            
        }
    }
}

TEST_F(ConcurrentStackTests, testMultiThread)
{
    Pal::ConcurrentStack<int> stack;
    constexpr int size = 200000;
    std::atomic_bool done1(false);
    int array[size];
    memset(array, -1, sizeof(array));
    
    auto push1 = [&stack, &done1]()
    {
        for (int i = 0; i < size/2;i++)
        {
            stack.push(i);
        }
        
        done1.store(true);
    };
    
    auto cons1 = [&]()
    {
        consume(done1, array, stack);
    };
    
    auto cons2 = [&]()
    {
        consume(done1, array, stack);
    };
    
    
    std::thread p1(push1);
    std::thread c1(cons1);
    std::thread c2(cons2);
    
    p1.join();
    c1.join();
    c2.join();
    
    for (int i = 0; i < size/2; i++) {
        ASSERT_EQ(i, array[i]);
    }
}


