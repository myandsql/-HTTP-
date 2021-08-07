#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include<unordered_map>
#include<cstring>
#include<sys/socket.h>
#include<unistd.h>
#include<iostream>
#include <sys/stat.h>
#include<fcntl.h>
#include <sys/sendfile.h>
#include<error.h>
using std::string;


/*namespace response_statu{
    char*OK="";
    char*
}*/


class HttpResponse {
public:
    HttpResponse(int fd);
    HttpResponse(const string &type, int fd);
    HttpResponse(const string &&type, int fd);
    bool set_status(const char*);
    bool set_header(string &key, string &value);
    virtual ~HttpResponse();
    bool buff_init();
    bool buff_init_header(bool flag);
    bool buff_send();
    bool write_string(string &text);
    bool write_string(char *);
    bool write_file(string &path);
    bool write_file(const char*);
protected:
    int m_fd;
    string m_status;
    std::unordered_map<string, string> m_headers;
    string m_body;
    string m_path;
    bool m_sendfile;
    char*m_buff;
    int m_len;
    int m_write_index;
    //  bool m_sendfile;
    int m_file_fd;
    off_t m_file_index;
    int m_file_size;
private:

};

#endif // HTTP_RESPONSE_H
