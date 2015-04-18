//
//  ParallelPipeline.h
//  Pal
//
//  Created by Vittorio Cellucci on 2015-04-17.
//
//

#ifndef Pal_ParallelPipeline_h
#define Pal_ParallelPipeline_h

#include <functional>

namespace Pal {
    
template<class T>
class parallel_pipeline
{
public:
    void input(std::function<T()> input_stage)
    {
        
    }
    
    void then(std::function<T(T)> process_stage)
    {
        //std::function<R()> f = std::bind(process_stage);
        //std::packaged_task<R()> t(f);
       // auto fut = t.get_future();
       // workQueue.push(std::move(t));
        //return fut;
    }
    
    void output(std::function<T(T)> process_stage)
    {
        
    }
    
    T run()
    {
        return T();
    }
};
}

#endif
