#include "sem.h"

sem::sem()
{
    if(sem_init(&_sem,0,0)!=0)
    {
        throw std::exception();
    }
}
sem::sem(int num)
{
    if(sem_init(&_sem,0,num)!=0)
    {
        throw std::exception();
    }
}
sem::~sem()
{
    if(sem_destroy(&_sem)!=0)
    {
        throw std::exception();
    }
}
bool sem::post()
{
    return sem_post(&_sem)==0;
}
bool sem::wait()
{
    return sem_wait(&_sem)==0;
}
