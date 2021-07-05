#include<iostream>
#include"./mysql_pool/mysql_pool.h"
#include<pthread.h>
#include"./log/Log.h"
#include<unistd.h>
#include"./thread_pool/thread_pool.h"
#include"./timer/timer.h"
#include<sys/socket.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include<cstring>
#include"./http/http_request.h"
#include<fcntl.h>
#include<unistd.h>
#include"./http/http_response.h"
#include"./event_loop/event_loop.h"
#include"./server/MyServer.h"

int main()
{
    ThreadPool p(5,1024);
    MyServer host(1024,&p);
    host.init();
    host.start();
    return 0;
}
