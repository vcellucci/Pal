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


TEST_F(AlgorithmTests, testForEach)
{
    std::vector<int,Pal::aligned_allocator<int>> intVector(257, 1);
    
    // assign 2 to each element in the vector
    Pal::for_each(intVector.begin(), intVector.end(), [](int& val)
    {
        val = 2;
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
    
    Pal::for_each(array, array + 256, [](int& val)
    {
        val = 2;
    });
    
    std::for_each(array, array + 256, [](int& val)
    {
        ASSERT_EQ(2, val);
    });
    
    allocator.deallocate(array);
}

TEST_F(AlgorithmTests, testBlockedRange)
{
    using IntVector =std::vector<int,Pal::aligned_allocator<int>>;
    IntVector intVector(257, 1);
    Pal::for_each_range(intVector.begin(), intVector.end(),
    [](Pal::chunk_range<IntVector::iterator> range)
    {
        for(auto it = range.begin; it != range.end; ++it)
        {
            *it = 2;
        }
    });
    
    std::for_each(intVector.begin(), intVector.end(), [](int& val)
    {
        ASSERT_EQ(2, val);
    });
}


TEST_F(AlgorithmTests, testBlockedRangeArray)
{
    Pal::aligned_allocator<int> allocator;
    int* array  = allocator.allocate(256);
    
    Pal::for_each_range(0, 256,
    [&array](Pal::chunk_range<int> range)
    {
        for(auto it = range.begin; it != range.end; ++it)
        {
            array[it] = 2;
        }
    });
    
    std::for_each(array, array + 256, [](int& val)
    {
        ASSERT_EQ(2, val);
    });
    
    allocator.deallocate(array);
}

