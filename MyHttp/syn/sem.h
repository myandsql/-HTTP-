#ifndef SEM_H
#define SEM_H

#include<semaphore.h>
#include <exception>

class sem
{
    public:
        sem();
        sem(int num);
        bool wait();
        bool post();
        virtual ~sem();

    protected:
       sem_t _sem;
    private:
};

#endif // SEM_H
