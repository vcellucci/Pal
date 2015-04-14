//
//  AlgorithmTests.cpp
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-13.
//
//

#include "AlgorithmTests.h"
#include <vector>
#include "Pal/Algorithms.h"
#include "Pal/AlignedAllocator.h"


TEST_F(AlgorithmTests, testParallelForEach)
{
    std::vector<int> intVector(257, 1);
    using Iterator = std::vector<int, Pal::aligned_allocator<int> >::iterator;
    
    Pal::parallel_for_each(intVector.begin(), intVector.end(), [](Iterator begin, Iterator end)
    {
        for(auto it = begin; it != end; ++it )
        {
            *it = 2;
        }
    });
    
    
    std::for_each(intVector.begin(), intVector.end(), [](int& val)
    {
        ASSERT_EQ(2, val);
    });
}

TEST_F(AlgorithmTests, testParallelForEachArray)
{
    
    Pal::aligned_allocator<int> allocator;
    int* array  = allocator.allocate(256);
    
    Pal::parallel_for_each(array, array + 256, [](int* begin, int* end)
    {
       for(auto it = begin; it != end; ++it )
       {
           *it = 2;
       }
    });
    
    std::for_each(array, array + 256, [](int& val)
    {
        ASSERT_EQ(2, val);
    });
    
    allocator.deallocate(array);
    
}

