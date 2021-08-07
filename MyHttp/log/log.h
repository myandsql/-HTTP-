#ifndef LOG_H
#define LOG_H
#include <cstdarg>
#include<pthread.h>
#include <cstdio>
#include <fstream>
#include <time.h>
#include <queue>
#include<cstring>
#include<sys/types.h>
#include<dirent.h>
#include <sys/stat.h>
#include "locker.h"
#include<iostream>

using std::queue;
#define LOG_ERROR(...)      Log::getLog()->push(2,##__VA_ARGS__)
#define LOG_DEBUG(...)      Log::getLog()->push(1,##__VA_ARGS__)
#define LOG_INFO(...)       Log::getLog()->push(0,##__VA_ARGS__)

#define UPDATE_SIZE 5

#define debug

class Log {
public:
    static Log* getLog() {
        static Log s_log;
        return &s_log;
    }
    bool push(int level, const char*format, ...);
    void init();
    pthread_t m_fd;
protected:
    bool open(int level, const char*path);
    char* get();
    bool update_path();
    char m_buff[128];
    std::ofstream m_file[3];
    queue<char*> m_queue;
    const char*const m_level[3] = {"[INFO]", "[DEBUG]", "[ERROR]"};
    const char*const str = "./Log";
    int m_len[3] = {6, 7, 7};
    locker m_lock;
    cond   m_cond;
    time_t m_t;
    tm m_now;
    bool m_run;
private:
    Log();
    virtual ~Log();
    static void *write_log(void*) {
        char *text = NULL;
        Log*t = Log::getLog();
        //std::cout<<"    45454"<<std::endl;
        int cnt[2] = {0};
        while (true) {
            text = t->get();
            if(text[1] == 'I') {
                t->m_file[0] << text << '\n';
                cnt[0]++;
                if(cnt[0] % UPDATE_SIZE == 0) {
                    t->m_file[0].flush();
                    cnt[0] = 0;
                }
            } else if(text[1] == 'D') {
                t->m_file[1] << text << '\n';
                cnt[1]++;
                if(cnt[1] % UPDATE_SIZE == 0) {
                    t->m_file[1].flush();
                    cnt[1] = 0;
                }
            } else t->m_file[2] << text << std::endl;
#ifdef debug
            std::cout << text << std::endl;
#endif // debug
            delete []text;
        }
        return (void*)0;
    }
};

#endif // LOG_H
