# Pal
Parallel Algorithms Library(PAL) is a modern c++(1x) library. It will be designed to take advantage of CPU cores.
NOTE: it is not designed to take advantage of GPU.
The main goal is to create a modern C++ parallel algorithms lib with no dependencies.


## Planned Features
It will feature standard parallel algorithms such as reduce and parallel pipeline.  It uses a work-stealing queue pattern.

## Current Features
* Currently features a work stealing task pool.
* Tested on Mac OS X and Linux

### How to build
This project uses cmake.  It also uses [googlemock](https://code.google.com/p/googlemock/).

1. Download [googlemock](https://code.google.com/p/googlemock/) and copy it into the Root of Pal.  
(Note: there will be an option later to skip this step)
2. Run cmake
    cmake .. -G Xcode // for xcode
    cmake .. -G "Unix Makefiles" // for Linux

### Some code
Below is how to use a TaskPool

    auto addTask = [](int x, int y)->int
    {
        return x + y;
    };
    
    //spmc_queue is a single-producer, multi-consumer lock-free queue.
    Pal::TaskPool<details::spmc_queue, int(int, int)> taskPool;
    
    auto future1 = taskPool.submit(addTask, 40, 2); // execute addTask async
    auto future2 = taskPool.submit(addTask, 20, 10); // execute another addTask async
    
    int value1 = future1.get();
    int value2 = future2.get();
    
    std::cout << value1 << std::endl; // will print 42
    std::cout << value2 << std::endl; // will print 30
    
    
