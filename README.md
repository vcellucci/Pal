# Pal
Parallel Algorithms Library(PAL) is a modern c++(1x) library. It will be designed to take advantage of CPU cores.
NOTE: it is not designed to take advantage of GPU.
The main goal is to create a modern C++ parallel algorithms lib with no dependencies.


## Planned Features
It will feature standard parallel algorithms such as reduce and parallel pipeline.  It uses a work-stealing queue pattern.
Visit the Wiki for more info.

## Current Features
* Currently features work stealing parallel_for_each algorithm
* Tested on Mac OS X and Linux

### How to build
This project uses cmake.  It also uses [googlemock](https://code.google.com/p/googlemock/).

1. Download [googlemock](https://code.google.com/p/googlemock/) and copy it into the Root of Pal.  
(Note: there will be an option later to skip this step)
2. Run cmake
    cmake .. -G Xcode // for xcode
    cmake .. -G "Unix Makefiles" // for Linux

### Some code
Here is a quick demo on how to use **parallel_for_each** on a vector

    std::vector<int,Pal::aligned_allocator<int>> intVector(257, 1);
    using Iterator = std::vector<int,Pal::aligned_allocator<int> >::iterator;
    
    // assign 2 to each element in the vector
    Pal::for_each(intVector.begin(), intVector.end(), [](int& val)
    {
        // each task gets a chunk of work
        for(auto it = begin; it != end; ++it )
        {
            val = 2;
        }
    });
    
    
