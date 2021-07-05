#include "http_handler.h"



HttpHandler::HttpHandler()
{
    //ctor
}

HttpHandler::~HttpHandler()
{
    //dtor
}



bool HttpHandler::registration(HttpRequest& request,HttpResponse& response)
{
    const string &body=request.get_body();
    int index=body.rfind('=');
    if(index==-1)
    {
        response.write_file("./root/login_error.html");
        return false;
    }
    string user =body.substr(5,index-14);
    string password = body.substr(index+1);
    string select_query="select * from user where number='"+user+"'";
    MYSQL *temp;
    mysqlRAII raii(&temp);
    mysql_real_query(temp,select_query.c_str(),select_query.size());
    MYSQL_RES *res = mysql_store_result(temp);
    int len=mysql_num_rows(res);
    MYSQL_ROW row=mysql_fetch_row(res);
    mysql_free_result(res);
  //  std::cout<<len<<std::endl;
    if(len==1)
    {
        response.write_file("./root/register_error.html");

    }
    else
    {
        select_query="insert into user(number,password)  values('"+user+"','"+password+"')";
        mysql_real_query(temp,select_query.c_str(),select_query.size());
        response.write_file("./root/welcome.html");
    }

}
bool HttpHandler::login(HttpRequest& request,HttpResponse& response)
{

    const string &body=request.get_body();
    int index=body.rfind('=');
    if(index==-1)
    {
        response.write_file("./root/login_error.html");
        return false;
    }

    string user =body.substr(5,index-14);
    string password = body.substr(index+1);
    string select_query="select * from user where number="+user+" and password="+password;
    MYSQL *temp;
    mysqlRAII raii(&temp);
    mysql_real_query(temp,select_query.c_str(),select_query.size());
    MYSQL_RES *res = mysql_store_result(temp);
    int len=mysql_num_rows(res);
    mysql_free_result(res);
    if(len==1)
    {
        response.write_file("./root/welcome.html");
    }
    else response.write_file("./root/login_error.html");
    return true;
}


bool HttpHandler::http_handle(HttpRequest& request,HttpResponse& response)
{
    const string &method=request.get_method();
    const string &p=request.get_path();
    string path("./root");
    path.append(p);
    if(method=="GET")
    {
        if(path=="./root/")response.write_file("./root/Navigation.html");
        else response.write_file(path.c_str());
    }
    else
    {
        if(path=="./root/login")
        {
            HttpHandler::login(request,response);
        }
        else if(path=="./root/registration")
        {
            HttpHandler::registration(request,response);
        }
        else response.write_file("./root/Navigation.html");
    }
    return true;
}

void* HttpHandler::run_read(void*args)
{
    arg* ptr=(arg*)args;
    if(*ptr->arg_request==NULL)
    {
        *ptr->arg_request=new HttpRequest(ptr->fd);
    }
    try
    {
        if((*ptr->arg_request)->parse())
        {
            ptr->arg_listener->change_event(ptr->fd,EPOLLOUT);
        }
        else
        {
            ptr->arg_listener->change_event(ptr->fd,EPOLLIN);
        }
        ptr->arg_manger->add_fd(ptr->fd);

    }
    catch(std::exception e)
    {
        ptr->arg_listener->delete_event(ptr->fd,EPOLLIN);
        delete *(ptr->arg_request);
        std::cout<<"exception"<<std::endl;
        *(ptr->arg_request)=NULL;
        close(ptr->fd);
    }
    return (void*)0;
}

void* HttpHandler::run_write(void*args)
{
    arg* ptr=(arg*)args;
    if(*ptr->arg_response==NULL)
    {
        *ptr->arg_response=new HttpResponse(ptr->fd);
        HttpHandler::http_handle(**ptr->arg_request,**ptr->arg_response);
        (*ptr->arg_response)->buff_init();
    }
    try
    {
        bool flag;
        if((*ptr->arg_response)->buff_send())
        {

            flag=(*ptr->arg_request)->get_header("Connection")==" keep-alive";
            delete *(ptr->arg_request);
            *(ptr->arg_request)=NULL;
            delete  *(ptr->arg_response);
            *(ptr->arg_response)=NULL;
            if(!flag)
            {
                ptr->arg_listener->delete_event(ptr->fd,EPOLLOUT);
                close(ptr->fd);
            }
            else ptr->arg_listener->change_event(ptr->fd,EPOLLIN);
        }
        else ptr->arg_listener->change_event(ptr->fd,EPOLLOUT);
        if(flag)ptr->arg_manger->add_fd(ptr->fd);
    }
    catch(std::exception e)
    {
        if(*(ptr->arg_request))
        {
            delete *(ptr->arg_request);
            *(ptr->arg_request)=NULL;
        }
        if(*(ptr->arg_response))
        {
            delete  *(ptr->arg_response);
            *(ptr->arg_response)=NULL;
        }
        ptr->arg_listener->delete_event(ptr->fd,EPOLLOUT);
        close(ptr->fd);
    }
    return (void*)0;
}
