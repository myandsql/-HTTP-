#include "locker.h"

locker::locker()
{
    if(pthread_mutex_init(&mutex,NULL)!=0)
    {
        throw std::
        exception();
    }
}

locker::~locker()
{
    if(pthread_mutex_destroy(&mutex)!=0)
    {
        throw std::
        exception();
    }
}
bool locker::lock()
{
    return pthread_mutex_lock(&mutex)==0;
}
bool locker::unlock()
{
    return pthread_mutex_unlock(&mutex)==0;
}

pthread_mutex_t* locker::get()
{
    return  &mutex;
}


