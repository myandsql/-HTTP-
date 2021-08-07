#include "timer.h"

#include<iostream>
Timer::Timer(int fd, int sec, int usec): m_fd(fd) {
    m_time.tv_sec = sec;
    m_time.tv_usec = usec;
}

Timer::~Timer() {
    //dtor
}

int TimerManger::get_time() {
    int ret = -1;
    if(m_tree.empty())return ret;
    ret = ((*m_tree.begin())->m_time.tv_sec - m_now_timer.m_time.tv_sec) * 1000;
    ret += ((*m_tree.begin())->m_time.tv_usec - m_now_timer.m_time.tv_usec) % 1000;
    //std::cout<<"time:"<<ret<<std::endl;
    return ret;
}

int TimerManger::get_time_out_fd() {
    update_time();
    Timer*ptr;
    int fd = 0;
    if(!m_tree.empty() && m_now_timer > (**m_tree.begin())) {
        m_mutex.lock();
        ptr = (*m_tree.begin());
        fd = ptr->m_fd;
        m_tree.erase(m_tree.begin());
        m_mutex.unlock();
    }
    return fd;
}
void TimerManger::update_time() {
    gettimeofday(&m_now_timer.m_time, NULL);
}
void TimerManger::add_fd(int fd) {

    m_mutex.lock();
    if(m_timer[fd] == NULL) {
        m_timer[fd] = new Timer(fd, m_now_timer.m_time.tv_sec + 20, m_now_timer.m_time.tv_usec);
    } else {
        m_timer[fd]->m_time.tv_sec = m_now_timer.m_time.tv_sec + 20;
        m_timer[fd]->m_time.tv_usec = m_now_timer.m_time.tv_usec;
    }
    m_tree.insert(m_timer[fd]);
    m_mutex.unlock();
}

void TimerManger::delete_fd(int fd) {
    m_mutex.lock();
    m_tree.erase(m_timer[fd]);
    m_mutex.unlock();
}

TimerManger::TimerManger(int size) {
    m_timer = new Timer*[size];
    for(int i = 0; i < size; i++)m_timer[i] = NULL;
    update_time();

}
TimerManger::~TimerManger() {}
