#ifndef LISTENER_H
#define LISTENER_H

#include<sys/epoll.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"../log/log.h"
#include"../timer/timer.h"
#include<exception>
#include<unistd.h>
#include"../log/log.h"
#include<fcntl.h>
class EventLoop
{
public:
    EventLoop(int size=1024);
    virtual ~EventLoop();
    void delete_event(int fd,uint32_t event);
    void change_event(int fd,uint32_t event);
    void add_event(int fd,uint32_t event);
    int get_listen_fd();
    int wait(int timeout);
    epoll_event* get_event();
    bool accept_fd(TimerManger&);
    static bool init_socket(int);
protected:
    int m_epoll_fd;
    int m_listen_fd;
    int m_size;
    epoll_event* m_ev;
private:
};

#endif // LISTENER_H
