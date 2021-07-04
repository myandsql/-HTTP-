#ifndef COND_H
#define COND_H
#include<pthread.h>
#include<exception>
#include"locker.h"
class cond
{
    public:
        cond();
        virtual ~cond();
        bool wait(pthread_mutex_t* mutex);
        bool timewait(pthread_mutex_t* mutex,timespec t);
        bool signal();
        bool broadcast();
    protected:
         pthread_cond_t _cond;
    private:
};

#endif // COND_H
