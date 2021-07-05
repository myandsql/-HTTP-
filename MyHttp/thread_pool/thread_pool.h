#ifndef THREADPOOL_H
#define THREADPOOL_H

#include"safe_queue.h"
#include<exception>
#include<queue>

struct task{
    void*args;
    void*(*fun)(void*);
};

class ThreadPool
{
    public:
        ThreadPool(int thread_size,int max_request);
        virtual ~ThreadPool();
        bool push(task*);
        task* get();
       // MyServer*m_server;
        //std::queue<task*> m_task_queue;
    protected:
        std::queue<task*> m_task_queue;
        pthread_t* m_threads;
        int m_size;
        int m_max_request;
        int run_size;
        locker m_mutex;
        cond m_cond;
    private:
        static void *worker(void *arg);
        void run(ThreadPool*pool);
};

#endif // THREADPOOL_H
