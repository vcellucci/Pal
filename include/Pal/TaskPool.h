#ifndef __PAL__TASK_POOL_H___
#define __PAL__TASK_POOL_H___


#include <functional>
#include <future>

namespace Pal{
    
template<template<class> class, class> class TaskPool;

template< template<class> class ContainerT, class R, class ...Args>
class TaskPool<ContainerT,R(Args...)>
{
};

}

#endif