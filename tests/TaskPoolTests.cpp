#include "TaskPoolTests.h"

#include "Pal/TaskPool.h"
#include "Pal/BlockingQueue.h"

TEST_F(TaskPoolTests, testTasks)
{
    Pal::TaskPool<Pal::BlockingQueue, void()> taskPool;
}