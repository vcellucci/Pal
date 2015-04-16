# Pal
Parallel Algorithms Library(PAL) is a modern c++(1x) library. It will be designed to take advantage of CPU cores.
NOTE: it is not designed to take advantage of GPU.
The main goal is to create a modern C++ parallel algorithms lib with no dependencies.


## Planned Features
It will feature standard parallel algorithms such as reduce and parallel pipeline.  It uses a work-stealing queue pattern.
Visit the Wiki for more info.

## Current Features
* Currently features work stealing parallel_for_each, paralle_for_each_range and parallel_reduce algorithms
* Tested on Mac OS X and Linux

### How to build
Building is optional, it simply builds the unit tests. Pal is currently headers only.
This project uses cmake.  It also uses [googlemock](https://code.google.com/p/googlemock/).

1. Download [googlemock](https://code.google.com/p/googlemock/) and copy it into the Root of Pal.  
2. Run cmake
    cmake .. -G Xcode // for xcode
    cmake .. -G "Unix Makefiles" // for Linux

### Some code
Here is a quick demo on how to use **parallel_for_each** on a vector

    std::vector<int,Pal::aligned_allocator<int>> intVector(257, 1);
    using Iterator = std::vector<int,Pal::aligned_allocator<int> >::iterator;
    
    // assign 2 to each element in the vector
    Pal::parallel_for_each(intVector.begin(), intVector.end(), [](int& val)
    {
        val = 2;
    });
    
Working with arrays;

    Pal::aligned_allocator<int> allocator;
    int* array  = allocator.allocate(256);
    
    Pal::parallel_for_each_range(0, 256, [&array](Pal::chunk_range<int> range)
    {
        for(auto it = range.begin; it != range.end; ++it)
        {
            array[it] = 2;
        }
    });
    
    allocator.deallocate(array);

**parallel_reduce** will take a list and reduce it to one value using a binary function.  Example below features parallel sum implementation using stl::plus.  The binary functor can be any binary function.

    // expected value = n(n+1)/2
    int n = 257;
    int expected = (n*(n+1))/2;

    using IntVector = std::vector<int, Pal::aligned_allocator<int>>;
    IntVector v;
    int counter = 1;
    std::generate(v.begin(), v.end(), [&counter]()
    {
        return counter++;
    });
    int value = Pal::parallel_reduce(v.begin(), v.end(), std::plus<int>());
    std::cout << value << std::endl; // prints out 33153

