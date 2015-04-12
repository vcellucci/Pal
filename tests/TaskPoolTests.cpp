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

TEST_F(TaskPoolTests, testTaskWorker)
{
    TestableMock m;
    EXPECT_CALL(m, call()).WillOnce(testing::Return(1));
    
    auto task =[&m]()
    {
        return m.call();
    };
    
    Pal::TaskPoolWorker<details::spmc_queue, int()> worker;
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
    
    Pal::TaskPoolWorker<details::spmc_queue, int(int, int)> worker;
    auto token = worker.push(task, 2, 3);
    worker.stop();
    worker();
    int testVal = token.get();
    ASSERT_EQ(5, testVal);
    
}