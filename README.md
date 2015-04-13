# Pal
Parallel Algorithms Library(PAL) is a modern c++(1x) library. It will be designed to take advantage of CPU cores.
NOTE: it is not designed to take advantage of GPU

**Planned Features**
It will feature standard parallel algorithms such as reduce and parallel pipeline.  It will use a work-stealing queue pattern.

**Current Features**
Currently features a linear task pool.  THe work is scheduled linearly accross threads.  I.e.  The first task will be scheduled to thread 1, second task to thread 2 etc...

**Some code**
Below is how to use a TaskPool

    auto addTask = [](int x, int y)->int
    {
        return x + y;
    };
    
    //spmc_queue is a single-producer, multi-consumer lock-free queue.
    Pal::TaskPool<details::spmc_queue, int(int, int)> taskPool;
    auto future = taskPool.submit(addTask, 40, 2);
    int value = future.get();
    std::cout << value << std::endl; // will print 42
    
