#include "event_loop.h"

EventLoop::EventLoop(int size)
{
    //ctor
    m_listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(m_listen_fd==-1)
    {
        LOG_ERROR("epoll create fail");
        throw std::exception();
    }
    init_socket(m_listen_fd);
    sockaddr_in listen_addr;
    memset(&listen_addr,0,sizeof(listen_addr));
    listen_addr.sin_family=AF_INET;
    listen_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    listen_addr.sin_port=htons(6666);
    int ret=bind(m_listen_fd,(sockaddr*)&listen_addr,sizeof(sockaddr));
    if(ret==-1)
    {
        LOG_ERROR("bind falil");
    }
    ret=listen(m_listen_fd,1024);
    if(ret==-1)
    {
        LOG_ERROR("listen falil");
        throw std::exception();
    }
    m_size=size;
    m_epoll_fd=epoll_create(m_size);
    epoll_event ev;
    ev.data.fd=m_listen_fd;
    ev.events=EPOLLIN|EPOLLET;
    epoll_ctl(m_epoll_fd,EPOLL_CTL_ADD,m_listen_fd,&ev);
    m_ev=new epoll_event[m_size];
}

EventLoop::~EventLoop()
{
    //dtor
    close(m_epoll_fd);
    close(m_listen_fd);
    delete []m_ev;
}


void EventLoop::add_event(int fd,uint32_t event)
{
    epoll_event ev;
    ev.data.fd=fd;
    ev.events=event|EPOLLET|EPOLLONESHOT;
    epoll_ctl(m_epoll_fd,EPOLL_CTL_ADD,fd,&ev);
}

void EventLoop::delete_event(int fd,uint32_t event)
{
    epoll_event ev;
    ev.data.fd=fd;
    ev.events=event|EPOLLET|EPOLLONESHOT;
    epoll_ctl(m_epoll_fd,EPOLL_CTL_DEL,fd,&ev);
}

void EventLoop::change_event(int fd,uint32_t event)
{
    epoll_event ev;
    ev.data.fd=fd;
    ev.events=event|EPOLLET|EPOLLONESHOT;
    int  ret=epoll_ctl(m_epoll_fd,EPOLL_CTL_MOD,fd,&ev);
    //if(ret==-1)std::cout<<"----"<<strerror(errno)<<std::endl;
    //return ret!=-1;

}

int EventLoop::wait(int timeout)
{
    return epoll_wait(m_epoll_fd,m_ev,m_size,timeout);
}

epoll_event* EventLoop::get_event()
{
    return m_ev;
}
int EventLoop::get_listen_fd()
{
    return m_listen_fd;
}

bool EventLoop::init_socket(int client_fd)
{
    int flags = fcntl(client_fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(client_fd, F_SETFL, flags);
    return true;
}

bool EventLoop::accept_fd(TimerManger &manger)
{
    manger.update_time();
    sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    socklen_t len=sizeof(addr);
    int client_fd;
    while(1)
    {
        client_fd=accept(m_listen_fd,(sockaddr*)&addr,&len);
        if(client_fd==-1)
        {
            if(errno==EINTR)continue;
            else
            {
              //  LOG_INFO("accpet finsh");
                return true;
            }
        }
       // LOG_INFO("accpet connect ip:%u\t port:%d",htonl(addr.sin_addr.s_addr),htons(addr.sin_port));
        init_socket(client_fd);
        add_event(client_fd,EPOLLIN);
        manger.add_fd(client_fd);
    }

   // LOG_INFO("accpet finsh");

}


