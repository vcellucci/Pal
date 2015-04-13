#include "TaskPoolTests.h"
#include "gmock/gmock.h"
#include "Pal/TaskPool.h"
#include "Pal/BlockingQueue.h"
#include "Pal/TaskPoolWorker.h"
#include "Pal/details/spmc_queue.h"
#include <deque>
#include <future>
#include <functional>

struct TestableWork
{
    virtual int call(){return 0;}
};

class TestableMock : public TestableWork
{
public:
    MOCK_METHOD0(call, int());
};

TEST_F(TaskPoolTests, testTaskWorkerIdx)
{
    Pal::TaskPoolWorker<details::spmc_queue, int()> worker(123);
    ASSERT_TRUE(worker.isWorking());
    ASSERT_EQ(123, worker.getIdx());
}

TEST_F(TaskPoolTests, testTaskWorker)
{
    TestableMock m;
    EXPECT_CALL(m, call()).WillOnce(testing::Return(1));
    
    auto task =[&m]()
    {
        return m.call();
    };
    
    Pal::TaskPoolWorker<details::spmc_queue, int()> worker(0);
    ASSERT_TRUE(worker.isWorking());

    auto token = worker.push(std::move(task));
    worker.stop();
    worker();
    int testVal = token.get();
    ASSERT_EQ(1, testVal);
}

TEST_F(TaskPoolTests, testTaskWorkerWithArgs)
{
    auto task =[](int x, int y)
    {
        return x + y;
    };
    
    Pal::TaskPoolWorker<details::spmc_queue, int(int, int)> worker(0);
    ASSERT_TRUE(worker.isWorking());

    auto token = worker.push(task, 2, 3);
    worker.stop();
    worker();
    int testVal = token.get();
    ASSERT_EQ(5, testVal);
    
}

TEST_F(TaskPoolTests, testSingleTask)
{
    auto addTask = [](int x, int y)->int
    {
        return x + y;
    };
    
    Pal::TaskPool<details::spmc_queue, int(int, int)> taskPool;
    auto future = taskPool.submit(addTask, 2,3);
    int val = future.get();
    ASSERT_EQ(5, val);
}

TEST_F(TaskPoolTests, testTaskPoolMultiTask)
{
    auto addTask = [](int x, int y)->int
    {
        return x + y;
    };
    
    Pal::TaskPool<details::spmc_queue, int(int, int)> taskPool;
    
    std::future<int> futures[32];
    
    for(int i = 0; i < 32; i++ )
    {
        futures[i] = taskPool.submit(addTask, 1, i);
    }
    
    for(int i = 0; i < 32; i++ )
    {
        int val = futures[i].get();
        ASSERT_EQ((1+i), val);
    }
}