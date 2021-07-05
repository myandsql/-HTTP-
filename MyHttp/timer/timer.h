#ifndef TIMER_H
#define TIMER_H

#include<sys/time.h>
#include<set>
#include<vector>
#include<functional>
#include"../syn/locker.h"

namespace MOD{
    const int mod=1e6;
}
class Timer
{
    public:
        Timer(int fd=-1,int sec=0,int usec=0);
        virtual ~Timer();
        timeval m_time;
        int m_fd;
        //bool m_delet_falg=false;
        Timer operator+(const Timer &oth)const{
            Timer temp;
            temp.m_time.tv_usec=m_time.tv_usec+oth.m_time.tv_usec;
            temp.m_time.tv_sec=m_time.tv_sec+oth.m_time.tv_sec+temp.m_time.tv_usec/MOD::mod;
            temp.m_time.tv_usec%=MOD::mod;
            return temp;
        }
        Timer operator-(const Timer &oth)const{
            Timer temp;
            temp.m_time.tv_usec=m_time.tv_usec-oth.m_time.tv_usec;
            temp.m_time.tv_sec=m_time.tv_sec-oth.m_time.tv_sec;
            if(temp.m_time.tv_usec<0){
                temp.m_time.tv_usec+=MOD::mod;
                temp.m_time.tv_sec--;
            }
            return temp;
        }
        bool operator <(const Timer  &oth)const{
            if(m_time.tv_sec==oth.m_time.tv_sec)
            {
                if(m_time.tv_usec==oth.m_time.tv_usec)return m_fd<oth.m_fd;
                return m_time.tv_usec<oth.m_time.tv_usec;
            }
            return m_time.tv_sec<oth.m_time.tv_sec;
        }
        bool operator >(const Timer  &oth)const{
            return !(*this<oth);
        }
    protected:
    private:
};

class TimerManger{
    struct cmp{
        bool operator()(const Timer*me,const Timer*oth){
            return *me<*oth;
        }
    };
    std::set<Timer*,cmp> m_tree;
    Timer** m_timer;
    Timer m_now_timer;
    locker m_mutex;
    public:
    void update_time();
    TimerManger(int size);
    virtual ~TimerManger();
    int get_time_out_fd();
    void add_fd(int fd);
    void delete_fd(int fd);
    int get_time();
};

#endif // TIMER_H
