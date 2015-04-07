#include "BlockingQueueTests.h"
#include "Pal/BlockingQueue.h"

TEST_F(BlockingQueueTests, testPush)
{
    Pal::BlockingQueue<int> q;
    q.push(3);
    
    ASSERT_EQ(1, q.size());
    
    int val;
    ASSERT_TRUE(q.pop(val));
    ASSERT_EQ(3, val);
    ASSERT_TRUE(q.empty());
    
}

TEST_F(BlockingQueueTests, testOrder)
{
    Pal::BlockingQueue<int> q;
    for(int i = 1; i < 4; i++ )
    {
        q.push(i);
    }
    
    int test = 1;
    while(!q.empty())
    {
        int val;
        ASSERT_TRUE(q.pop(val));
        ASSERT_EQ(test, val);
        test++;
    }
}

TEST_F(BlockingQueueTests, testEmpty)
{
    Pal::BlockingQueue<int> q;
    q.stop();
    int val;
    ASSERT_FALSE(q.pop(val));
}

