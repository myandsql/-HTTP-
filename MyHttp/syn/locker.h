#ifndef LOCKER_H
#define LOCKER_H
#include<pthread.h>
#include<exception>
#include"cond.h"
class locker
{
    public:
        locker();
        virtual ~locker();
        bool lock();
        bool unlock();
        pthread_mutex_t* get();
    protected:
        pthread_mutex_t mutex;
    private:
};

#endif // LOCK_H
