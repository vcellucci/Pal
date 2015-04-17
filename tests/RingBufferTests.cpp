//
//  RingBufferTests.cpp
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-17.
//
//

#include "RingBufferTests.h"
#include "Pal/RingBuffer.h"


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