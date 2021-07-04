#ifndef MYSERVER_H
#define MYSERVER_H
#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../http/http_handler.h"
#include "../event_loop/event_loop.h"
#include "../thread_pool/thread_pool.h"
#include "../mysql_pool/mysql_pool.h"
#include "../timer/timer.h"





class MyServer
{
    public:
        MyServer(int size,ThreadPool*pool);
        virtual ~MyServer();
        void init();
        void start();
    protected:
        ThreadPool *m_pool;
        HttpRequest** m_request;
        HttpResponse** m_response;
        EventLoop m_event_pool;
        TimerManger m_timer_manger;
        int m_size;
    private:
};

#endif // MYSERVER_H


