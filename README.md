# -HTTP-
基于Linux环境下使用Code:blocks开发的简易HTTP服务器。  
  
使用非阻塞的SOCKET。  
EPOLL采用边缘触发模式。  
使用状态机来解析HTPP请求。  
由线程池来解析，处理，响应请求。  
编写了一个异步的日记系统。  
封装红黑树实现了一个配合EPOLL的定时器   
最终实现了GET和POST方法，支持长连接。   
  
主要的类如下：  

编写EventLoop类，封装了epoll的相关接口，实现事件监听。  
编写HttpRequest类，用于解析HTTP请求。  
编写HttpHandler类，用于处理HTTP请求。  
编写HttpResponse类，用于响应HTTP请求。  
编写Log类，实现异步写日记。  
编写TimerManger类，用于定时任务，来支持长连接。  
编写ThreadPool类，线程池，负责执行任务。即解析，处理，响应HTTP请求的执行者。  
编写MysqlPool类，数据库连接池，预先建立数据库连接。用于支持登陆，注册功能。  

