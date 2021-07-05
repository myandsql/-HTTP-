#include "Log.h"
#include<iostream>
#include<error.h>
#include<unistd.h>
Log::Log(){

}

Log::~Log(){
 for(int i=0;i<3;i++){
    m_file[i].flush();
    m_file[i].close();
 }
}
void Log::init(){
 DIR*dir;
 dir=opendir("./Log");
    if(dir==NULL){
      mkdir("./Log",0777);
    }
    else closedir(dir);
    dir=opendir("./Log/ERROR");
    if(dir==NULL){
      mkdir("./Log/ERROR",0777);
    }
    else closedir(dir);
    dir=opendir("./Log/INFO");
    if(dir==NULL){
           mkdir("./Log/INFO",0777);

    }
    else closedir(dir);
    dir=opendir("./Log/DEBUG");
    if(dir==NULL){
         mkdir("./Log/DEBUG",0777);
    }
    else closedir(dir);
    time(&m_t);
    m_now=*localtime(&m_t);
    update_path();
    m_run=false;
    pthread_create(&m_fd,NULL,write_log,NULL);

}


bool Log::push(int level,const char*format,...){
    char *buff=new char[128];
    strcpy(buff,m_level[level]);
    time(&m_t);
    tm *now=localtime(&m_t);
    strftime(buff+m_len[level],120," [%Y-%m-%d %H:%M:%S] ",now);
    va_list ap;
    va_start(ap,format);
    vsprintf(buff+23+m_len[level],format,ap);
    m_lock.lock();
    m_queue.push(buff);
    if(m_run==false)
    {
        m_cond.signal();
    }
    m_lock.unlock();
    return true;
}

char* Log::get(){
    char*ret=NULL;
    m_lock.lock();
    if(m_queue.empty()){
        m_run=false;
        getLog()->m_file[0].flush();
        getLog()->m_file[1].flush();
        m_cond.wait(m_lock.get());
        m_run=true;
    }
    ret=m_queue.front();
    int temp=17;
    if(ret[1]=='I')temp--;
    temp=(ret[temp]-'0')*10+ret[temp+1]-'0';
    if(m_now.tm_mday!=temp)update_path();
    m_queue.pop();
    m_lock.unlock();

    return ret;
}

bool Log::open(int level,const char *path){
    if(m_file[level].is_open())m_file[level].close();
    m_file[level].open(path,std::ios::app|std::ios::in);
    if(!m_file[level].is_open())LOG_ERROR("FILE OPEN ERROR!");
    return m_file[level].is_open();
}
bool Log::update_path(){
    strcpy(m_buff,str);
    time(&m_t);
    tm *now=localtime(&m_t);
    tm  m_now=*now;
    for(int i=0;i<3;i++){
       strcpy(m_buff+5,m_level[i]);
       m_buff[5]='/';
       m_buff[5+m_len[i]-1]='/';
       strftime(m_buff+5+m_len[i],110,"%Y-%m-%d.txt",now);
       open(i,m_buff);
    }
}


