#include "cond.h"

cond::cond()
{
    if(pthread_cond_init(&_cond,NULL)!=0)
    {
        throw std::exception();
    }
}

cond::~cond()
{
    if(pthread_cond_destroy(&_cond)!=0)
    {
        throw std::exception();
    }
}
bool cond::wait(pthread_mutex_t* mutex)
{
    return pthread_cond_wait(&_cond,mutex)==0;
}
bool cond::signal()
{
    return pthread_cond_signal(&_cond)==0;
}
bool cond::timewait(pthread_mutex_t*mutex,timespec t)
{
    return pthread_cond_timedwait(&_cond,mutex,&t)==0;
}
bool cond::broadcast()
{
    return  pthread_cond_broadcast(&_cond)==0;
}

