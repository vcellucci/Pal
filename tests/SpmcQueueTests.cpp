//
//  SpmcQueueTests.cpp
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-09.
//
//

#include "SpmcQueueTests.h"
#include "Pal/details/spmc_queue.h"
#include "Pal/details/ref_ptr.h"
#include "Pal/details/free_list.h"
#include "gmock/gmock.h"
#include <thread>
#include <vector>

using int_queue = details::spmc_queue<int>;

class testable_interface
{
public:
    virtual ~testable_interface() = default;
    virtual void call() = 0;
};

class mock : public testable_interface
{
public:
    MOCK_METHOD0(call, void());
};

struct testable_node
{
    std::atomic<testable_node*> next;
    testable_interface* data;
    testable_node(testable_interface* _data)
    : next(nullptr)
    , data(_data)
    {
    }
    
    ~testable_node()
    {
        data->call();
    }
};

TEST_F(SpmcQueueTests, testRefPtr)
{
    mock* m = new mock();
    EXPECT_CALL(*m, call());
    
    details::ref_ptr<testable_interface> p(m);
    ASSERT_TRUE(static_cast<bool>(p));
    p->call();
}

TEST_F(SpmcQueueTests, testRefPtrEmpty)
{
    details::ref_ptr<testable_interface> p;
    ASSERT_FALSE(static_cast<bool>(p));
}

TEST_F(SpmcQueueTests, testAssignment)
{
    mock* m = new mock();
    EXPECT_CALL(*m, call());
    details::ref_ptr<testable_interface> p(m);
    details::ref_ptr<testable_interface> p2;
    p2 = p;
    p2->call();
    
    ASSERT_EQ(1, p2.count());
    ASSERT_EQ(2, p.count());
    
    details::ref_ptr<testable_interface> p3(p);
    ASSERT_EQ(3, p.count());
}


TEST_F(SpmcQueueTests, testRefCopy)
{
    mock* m = new mock();
    EXPECT_CALL(*m, call());
    details::ref_ptr<testable_interface> p1(m);
    details::ref_ptr<testable_interface> p2(p1);
    p2->call();
    ASSERT_EQ(2, p1.count());
    ASSERT_EQ(1, p2.count());
}

TEST_F(SpmcQueueTests, testEquality)
{
    mock* m = new mock();
    EXPECT_CALL(*m, call());
    details::ref_ptr<testable_interface> p1(m);
    details::ref_ptr<testable_interface> p2(p1);
    p2->call();
    
    ASSERT_EQ(p1, p2);
    
    mock* m2 = new mock();
    details::ref_ptr<testable_interface> p3(m2);
    ASSERT_NE(p1, p3);

}

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
        std::this_thread::yield();
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
        else
        {
            std::this_thread::yield();
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
        else
        {
            std::this_thread::yield();
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
    
    ASSERT_TRUE(q.is_lock_free());
}

TEST_F(SpmcQueueTests, testFreeList)
{
    mock m;
    EXPECT_CALL(m, call());
    testable_node* node = new testable_node(&m);
    
    details::free_list<testable_node> list;
    list.push_front(node);
    
    list.free();
}