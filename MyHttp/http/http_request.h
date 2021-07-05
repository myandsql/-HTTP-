#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H
#include<string>
#include<unistd.h>
#include<sys/socket.h>
#include<errno.h>
#include<queue>
#include<unordered_map>
#include"../log/log.h"
using std::string;
using std::move;
enum class STATUS{
    READ_HTTP_HEAD=0,
    READ_HTTP_LINE,
    READ_HTTP_BODY,
    READ_HTTP_FINSH,
    READ_HTTP_ERROR
};
enum class END{
        NO,
        LINE_END,
        WORD_END
};
class HttpRequest
{
    protected:
        STATUS m_status;
        END m_end_status;
        const int m_fd;
        string m_method;
        string m_path;
        string m_protocolVersion;
        //std::unordered_map<string,string> m_headers;
        string m_body;
        int m_read_index=0,m_end_index=0;
        char m_buffer[1024];
        string m_string_buff1;
        string m_string_buff2;
        int m_string_index;
    public:
        std::unordered_map<string,string> m_values;
        std::unordered_map<string,string> m_headers;
        HttpRequest(int socket);
        HttpRequest();
        ~HttpRequest();
        //bool setSocket(int socket);
        bool init();
        bool parse();
        //static void*fun(void*);
        //int  get_fd()const;
        const string& get_method();
        const string& get_path();
        const string& get_protocolVersion();
        const string& get_header(const string &key);
        const string& get_header(string &&key);
        const string& get_value(const string &key);
        const string& get_value(string &&key);
      //  vector<string> get_headerName()const;
        const string& get_body();
   protected:
        bool read_byte(char &c);
        void read_word(string &word);
        void read_line(string &word);
        int read_socket();


};

#endif // HTTP_REQUEST_H
