#include "http_response.h"

HttpResponse::HttpResponse(int fd)
{
    m_fd=fd;
    m_status="HTTP/1.1 200 OK\r\n";
    m_headers["Server"]=" MyHttp XU\r\n";
    m_headers["Content-Type"]=" text/html\r\n";
    m_write_index=0;
    m_sendfile=false;
}

HttpResponse::HttpResponse(const string &type,int fd)
{
    m_fd=fd;
    m_status="HTTP/1.1 200 OK\r\n";
    m_headers["Server"]=" MyHttp XU\r\n";
    m_headers["Content-Type"]=" "+type+"\r\n";
    m_write_index=0;
    m_sendfile=false;
}
HttpResponse::HttpResponse(const string &&type,int fd)
{
    m_fd=fd;
    m_status="HTTP/1.1 200 OK\r\n";
    m_headers["Server"]=" MyHttp XU\r\n";
    m_headers["Content-Type"]=" "+type+"\r\n";
    m_write_index=0;
    m_sendfile=false;
}

HttpResponse::~HttpResponse()
{
    //dto
    delete []m_buff;
}

bool HttpResponse::buff_init()
{
    if(!m_sendfile)
    {

        buff_init_header(false);
    }
    else
    {
        // m_len+=m_body.size()+2;
        m_file_fd=open(m_path.c_str(),O_RDONLY);
        if(m_file_fd==-1)
        {
            m_file_fd=open("./root/404.html",O_RDONLY);
        }
        else
        {
            if(m_path.back()=='g') m_headers["Content-Type"]=" image/png\r\n";
            else if(m_path.back()=='s')	m_headers["Content-Type"]=" text/css\r\n";
            else if(m_path.back()=='f')m_headers["Content-Type"]=" image/gif\r\n";
        }
        struct stat filebuff;
        fstat(m_file_fd,&filebuff);
        m_headers["Content-length"]=std::to_string(filebuff.st_size)+"\r\n";
        m_file_size=filebuff.st_size;
        m_file_index=0;
        buff_init_header(true);
        //std::cout<<"buff init"<<std::endl;

    }

    // std::cout<<m_buff<<std::endl;
    return true;
}

bool HttpResponse::buff_send()
{
    while(m_write_index<m_len)
    {
        // std::cout<<"send write"<<std::endl;
        int ret=write(m_fd,m_buff+m_write_index,m_len-m_write_index);

        if(ret>0)
        {
            m_write_index+=ret;
        }
        else if(ret==-1)
        {
            //std::cout<<strerror(errno)<<std::endl;
            if(errno==EINTR)continue;
            else if(errno==EWOULDBLOCK||errno==EAGAIN)return false;
           // std::cout<<strerror(errno)<<std::endl;
            if(m_sendfile)close(m_file_fd);
            throw std::exception();
        }
        else if(ret==0)
        {
            if(m_sendfile)    close(m_file_fd);
            throw std::exception();
        }
    }
    if(m_sendfile)
    {
        while(m_file_index<m_file_size)
        {
            // std::cout<<"send file"<<std::endl;
            int ret=sendfile(m_fd,m_file_fd,&m_file_index,m_file_size);
            // std::cout<<ret<<' '<<m_file_index<<' '<<m_file_size<<std::endl;

            if(ret==-1)
            {
                if(errno==EINTR)continue;
                else if(errno==EWOULDBLOCK||errno==EAGAIN)return false;
              //  std::cout<<strerror(errno)<<std::endl;
                close(m_file_fd);
                throw std::exception();


            }
            else if(ret==0)
            {
                close(m_file_fd);
                throw std::exception();
            }
            //std::cout<<ret<<' '<<m_file_index<<std::endl;
        }
        close(m_file_fd);
    }

    return true;
}

bool HttpResponse::write_string(string &text)
{
    m_body.append(text);
    return true;
}

bool HttpResponse::write_string(char* text)
{
    m_body.append(text);
    return true;
}

bool HttpResponse::set_status(const char*str)
{
    m_status=str;
    return true;
}

bool HttpResponse::set_header(string &key,string &value)
{
    m_headers[key]=value;
    return true;
}

bool HttpResponse::write_file(string &path)
{
    m_path=path;
    m_sendfile=true;
    return true;
}

bool HttpResponse::write_file(const char* path)
{
    m_path=path;
    m_sendfile=true;
    return true;
}



bool HttpResponse::buff_init_header(bool falg)
{
    m_len=0;
    m_len+=m_status.size();
    if(!falg)m_headers["Content-length"]=std::to_string(m_body.size())+"\r\n";
    //  std::cout<<m_status<<std::endl;
    for(auto &header:m_headers)
    {
        m_len+=header.first.size()+1;
        m_len+=header.second.size();
    }
    if(!falg) m_len+=m_body.size()+2;
    else m_len+=2;
    m_buff=new  char[m_len+2];
    int i=0;
    strcpy(m_buff,m_status.c_str());
    i+=m_status.size();
    for(auto &header:m_headers)
    {
        strcpy(m_buff+i,header.first.c_str());
        i+=header.first.size();
        m_buff[i]=':';
        i++;
        strcpy(m_buff+i,header.second.c_str());
        i+=header.second.size();
    }
    strcpy(m_buff+i,"\r\n");
    i+=2;
    if(!falg) strcpy(m_buff+i,m_body.c_str());
    return true;

}
