#include "mysql_pool.h"

MysqlPool::MysqlPool() {}
MysqlPool::~MysqlPool() {
    while(m_queue.size() > 0) {
        MYSQL*t = m_queue.front();
        mysql_close(t);
        m_queue.pop();
    }
}

MysqlPool* MysqlPool::GetInstance() {
    static MysqlPool s_pool;
    return &s_pool;
}
bool MysqlPool::init(string server, string user, string password, string db, int max_size) {
    m_server = server;
    m_user = user;
    m_password = password;
    m_db = db;
    m_size = max_size;
    m_free = 0;
    for(int i = 0; i < max_size; i++) {
        MYSQL *temp = NULL;
        temp = mysql_init(temp);
        if(temp == NULL) {
            LOG_ERROR("mysql_init falil");
            throw std::exception();
        }
        temp = mysql_real_connect(temp, m_server.c_str(), user.c_str(), password.c_str(), db.c_str(), 3306, NULL, 0);
        if(temp == NULL) {
            LOG_ERROR("mysql_real_connect falil");
            throw std::exception();
        }
        m_free++;
        m_queue.push(temp);
    }
    LOG_DEBUG("mysql_pool init success!");
    return true;
}
bool MysqlPool::ReleaseConnection(MYSQL* temp) {
    m_mutex.lock();
    m_queue.push(temp);
    m_free++;
    m_cond.signal();
    m_mutex.unlock();
    return true;
}

int MysqlPool::GetFreeConn() {
    return m_free;
}

MYSQL* MysqlPool::GetConnection() {
    MYSQL *temp = NULL;
    m_mutex.lock();
    while(1) {
        if(m_free > 0) {
            temp = m_queue.front();
            m_queue.pop();
            break;
        } else m_cond.wait(m_mutex.get());
    }
    m_mutex.unlock();
    return temp;
}


mysqlRAII::mysqlRAII(MYSQL**m) {
    *m = m_con = MysqlPool::GetInstance()->GetConnection();
}
mysqlRAII::~mysqlRAII() {
    MysqlPool::GetInstance()->ReleaseConnection(m_con);
}
