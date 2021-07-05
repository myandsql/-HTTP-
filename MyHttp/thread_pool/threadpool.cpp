#include "thread_pool.h"
#include "../log/Log.h"
#include<iostream>
ThreadPool::ThreadPool(int thread_size,int max_request)
{

    if(thread_size<0||max_request<0)
    {
        LOG_ERROR("threadpool init fail!");
        throw std::exception();
    }
    m_size=thread_size;
    run_size=thread_size;
    m_max_request=max_request;
    m_threads=new pthread_t[thread_size];
    int ret=0;
    for(int i=0; i<thread_size; i++)
    {
        ret=pthread_create(&m_threads[i],NULL,worker,this);
        if(ret)
        {
            delete []m_threads;
            LOG_ERROR("threadpool pthread_create fail!");
            throw std::exception();
        }
        ret=pthread_detach(m_threads[i]);
        if(ret)
        {
            delete []m_threads;
            LOG_ERROR("threadpool pthread_detach fail!");
            throw std::exception();
        }
    }
    LOG_DEBUG("threadpool init success");
}

ThreadPool::~ThreadPool()
{
    delete[] m_threads;
}

bool ThreadPool::push(task*value)
{
    bool ret=true;
    m_mutex.lock();
    if(m_task_queue.size()<m_max_request)
    {
        m_task_queue.push(value);
        if(run_size<m_size&&m_task_queue.size()>run_size)m_cond.signal();
    }
    else ret=false;
    m_mutex.unlock();
    return ret;
}
task* ThreadPool::get()
{
    m_mutex.lock();
    while(m_task_queue.empty())
    {
        run_size--;

        m_cond.wait(m_mutex.get());
        run_size++;

    }
    task*ret=m_task_queue.front();
    m_task_queue.pop();
    m_mutex.unlock();
    return ret;
}
void *ThreadPool::worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    pool->run(pool);
    return (void*)0;
}

void ThreadPool::run(ThreadPool *pool)
{

    while(true)
    {
        task*ret=pool->get();
        ret->fun(ret->args);
        LOG_DEBUG("正在运行的工作线程%d",run_size);
        delete ret;
    }
}
