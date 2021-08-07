#ifndef MYSQL_POOL_H
#define MYSQL_POOL_H
#include<string>
#include<iostream>
#include<mysql/mysql.h>
#include <stdlib.h>
#include"../syn/cond.h"
#include "../syn/locker.h"
#include"../log/log.h"
#include <queue>
using std::string;
class MysqlPool {
public:
    static MysqlPool *GetInstance();
    bool ReleaseConnection(MYSQL *conn); //释放连接
    int GetFreeConn();
    MYSQL* GetConnection();
    bool init(string server, string user, string password, string db, int max_size);
protected:
    MysqlPool();
    virtual ~MysqlPool();
    string m_server;
    string m_password;
    string m_user;
    string m_db;
    int m_size;
    int m_free;
    std::queue<MYSQL*> m_queue;
    cond m_cond;
    locker m_mutex;


private:
};

class mysqlRAII {
public:
    mysqlRAII(MYSQL**m);
    virtual ~mysqlRAII();
private:
    MYSQL* m_con;
};

#endif // MYSQL_POOL_H
