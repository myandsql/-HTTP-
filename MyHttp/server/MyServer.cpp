#include "MyServer.h"

string user="root";
string password="123";
string db="MyHttp";
string server="localhost";

MyServer::MyServer(int size,ThreadPool*pool):m_event_pool(size),m_timer_manger(size)
{
    //ctor
    m_size=size;
    m_pool=pool;
    m_request=new HttpRequest*[size];
    m_response=new HttpResponse*[size];
    for(int i=0; i<size; i++)
    {
        m_request[i]=NULL;
        m_response[i]=NULL;
    }
    //  memset(m_request,NULL,sizeof(HttpRequest*)*size);
    // memset(m_response,NULL,sizeof(HttpResponse*)*size);

}

MyServer::~MyServer()
{
    //dtor
    for(int i=0; i<m_size; i++)
    {
        if(m_request[i]!=NULL)delete m_request[i];
    }
    delete []m_request;

    for(int i=0; i<m_size; i++)
    {
        if(m_response[i]!=NULL)delete m_response[i];
    }
    delete []m_response;

}
void MyServer::init()
{
    Log *t=Log::getLog();
    t->init();
    MysqlPool::GetInstance()->init(server,user,password,db,10);
    //m_pool.m_server=this;
}

void MyServer::start()
{
    epoll_event*ev=m_event_pool.get_event();
    int listen_fd=m_event_pool.get_listen_fd();
    int len;
    int time_out_fd=0;
    while(1)
    {
        len=m_event_pool.wait(m_timer_manger.get_time());
        std::cout<<"len:"<<len<<std::endl;
        for(int i=0; i<len; i++)
        {
            if(ev[i].data.fd==listen_fd)
            {
                m_event_pool.accept_fd(m_timer_manger);
            }
            else
            {
                int fd=ev[i].data.fd;

                if(ev[i].events==EPOLLIN)
                {
                    task* ptr=new task;
                    arg* args=new arg(fd,&m_request[fd],&m_response[fd],&m_event_pool,&m_timer_manger);
                    ptr->args=(void*)args;
                    ptr->fun=HttpHandler::run_read;
                    m_timer_manger.delete_fd(fd);
                    m_pool->push(ptr);
                }
                else
                {
                    task* ptr=new task;
                    arg* args=new arg(fd,&m_request[fd],&m_response[fd],&m_event_pool,&m_timer_manger);
                    ptr->args=(void*)args;
                    ptr->fun=HttpHandler::run_write;
                    m_timer_manger.delete_fd(fd);
                    m_pool->push(ptr);
                };
            }
        }
        while((time_out_fd=m_timer_manger.get_time_out_fd())>0)
        {
            if(m_response[time_out_fd]!=NULL)
                m_event_pool.delete_event(time_out_fd,EPOLLOUT);
            else  m_event_pool.delete_event(time_out_fd,EPOLLIN);
            if(m_request[time_out_fd])
            {
                delete m_request[time_out_fd];
                m_request[time_out_fd]=NULL;
            }
            if(m_response[time_out_fd])
            {
                delete  m_response[time_out_fd];
                m_response[time_out_fd]=NULL;
            }
            std::cout<<"time_out_fd:"<<time_out_fd<<std::endl;
            close(time_out_fd);
        }

    }
}
