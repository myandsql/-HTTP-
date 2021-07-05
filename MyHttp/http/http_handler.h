#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include"../log/log.h"
#include"../mysql_pool/mysql_pool.h"
#include"./http_request.h"
#include"./http_response.h"
#include"../event_loop/event_loop.h"
#include"../timer/timer.h"

class arg{
    public:
    int fd;
    HttpRequest** arg_request;
    HttpResponse**arg_response;
    EventLoop* arg_listener;
    TimerManger *arg_manger;
    arg(int _fd,HttpRequest** request,HttpResponse** response,EventLoop *_listener,TimerManger* manger):
    fd(_fd),arg_request(request),arg_response(response),arg_listener(_listener),arg_manger(manger){};
};

class HttpHandler
{
    public:
        static bool path_switch(HttpRequest&,HttpResponse&);
        static bool login(HttpRequest&,HttpResponse&);
        static bool registration(HttpRequest&,HttpResponse&);
        static bool http_handle(HttpRequest&,HttpResponse&);
        static void* run_read(void*args);
        static void* run_write(void*args);
    protected:

    private:
        HttpHandler();
        virtual ~HttpHandler();
};

#endif // HTTP_HANDLER_H
