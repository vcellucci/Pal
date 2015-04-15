//
//  parition.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-15.
//
//

#ifndef Pal_parition_h
#define Pal_parition_h


namespace details {
    
template<class T>
struct range_partition
{
    T chunks;
    T chunk_size;
    T left_over;
};
    
template<class T>
range_partition<T> partition(T num_threads, T num_elements)
{
    range_partition<T> rp;
    rp.chunks = num_elements / num_threads;
    rp.chunk_size = num_elements / rp.chunks;
    rp.left_over    = num_elements - (rp.chunks*rp.chunk_size);

    return rp;
    
};
    
}
#endif
