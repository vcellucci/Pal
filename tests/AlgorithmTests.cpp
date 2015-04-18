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
#include "Pal/details/parition.h"



TEST_F(AlgorithmTests, testChunkPartitioner)
{
    std::size_t numThreads = 16;
    std::size_t numElements = 32;
    
    details::range_partition<std::size_t> p = details::partition(numThreads, numElements);
    
    ASSERT_EQ(2, p.chunks);
    ASSERT_EQ(16, p.chunk_size);
    ASSERT_EQ(0, p.left_over);
}

TEST_F(AlgorithmTests, testForEach)
{
    std::vector<int,Pal::aligned_allocator<int>> intVector(257, 1);
    
    // assign 2 to each element in the vector
    Pal::parallel_for_each(intVector.begin(), intVector.end(), [](int& val)
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
    
    Pal::parallel_for_each(array, array + 256, [](int& val)
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
    Pal::parallel_for_each_range(intVector.begin(), intVector.end(),
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
    
    Pal::parallel_for_each_range(0, 256,
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

TEST_F(AlgorithmTests, testParallelReduce)
{
    // expected value = n(n+1)/2
    int n = 257;
    int expected = (n*(n+1))/2;
    
    using IntVector = std::vector<int, Pal::aligned_allocator<int>>;
    IntVector v(n);
    int counter = 1;
    std::generate(v.begin(), v.end(), [&counter]()
    {
        return counter++;
    });
    
    int value = Pal::parallel_reduce(v.begin(), v.end(), std::plus<int>());
    ASSERT_EQ(expected, value);
}

TEST_F(AlgorithmTests, DISABLED_testParallelPipeline)
{
    auto inputStage = []()->int
    {
        return 1;
    };
    
    auto incrementStage = [](int number)->int
    {
        return ++number;
    };
    
    auto squareStage = [](int number)->int
    {
        return (number*number);
    };
    
    auto finaStage = [](int number)->int
    {
        return (number--);
    };
    
    Pal::parallel_pipeline<int> pipeline;
    
    pipeline.input(inputStage);
    
    pipeline.then(incrementStage);
    pipeline.then(squareStage);
    
    pipeline.output(finaStage);
    int result = pipeline.run();
    ASSERT_EQ(3, result);
    
}



