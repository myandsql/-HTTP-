# -HTTP-
基于Linux环境下使用Code:blocks开发的简易HTTP服务器。  
  
使用非阻塞的socket。  
epoll采用边缘触发模式。  
Rectaor多线程模型  
使用状态机来解析HTPP请求。  
由线程池来解析，处理，响应请求。  
编写了一个异步的日记系统。  
封装红黑树实现了一个配合EPOLL的定时器   
最终实现了GET和POST方法，支持长连接。   
  
  
### 主要的类如下：  

编写EventLoop类，封装了epoll的相关接口，实现事件监听。  
编写HttpRequest类，用于解析HTTP请求。  
编写HttpHandler类，用于处理HTTP请求。  
编写HttpResponse类，用于响应HTTP请求。  
编写Log类，实现异步写日记。  
编写TimerManger类，用于定时任务，来支持长连接。  
编写ThreadPool类，线程池，负责执行任务。即解析，处理，响应HTTP请求的执行者。  
编写MysqlPool类，数据库连接池，预先建立数据库连接。用于支持登陆，注册功能。  

### 项目心得
该项目为为练手项目，主要是为了锻炼编码能力，熟悉网络编程及多线程编程。
在该项目中，我使用多线程的reactor模式,一个线程事件循环，线程池线程负责处理，在多并发的情况很容易出问题，
经过观察日记，发现同一个连接，有可能多个HTTP请求，有可能有一个请求在读，一个在写，就出现了段错误。
所以我对epoll_event事件加上EPOLLONESHOT，保证同一个连接只有一个请求在线程池里任务队列里。解决了问题。

优化空间：
日记系统，线程池都是一个安全队列实现的，在进列，出列都是要加锁的，解锁，
但是对与日记系统出列只会有一个线程来做，对与线程池，只有一个线程来入列，
所以可以考虑使用2个队列，通过swap交换指针，这样一个线程的那边入列出列就不需要加锁，解锁了，
可以减少一半的加锁操作。

  
### 模块机构图如下：

![Image text](https://github.com/myandsql/-HTTP-/blob/main/%E6%A8%A1%E5%9D%97%E7%BB%93%E6%9E%84.PNG)


### webbench测压如下：

![Image text](https://github.com/myandsql/-HTTP-/blob/main/%E6%B5%8B%E5%8E%8B.PNG)

### 功能测试

登录：

![Image gif](https://github.com/myandsql/-HTTP-/blob/main/login.gif)

注册：
![Image gif](https://github.com/myandsql/-HTTP-/blob/main/register.gif)




