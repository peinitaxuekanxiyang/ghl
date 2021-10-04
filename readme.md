客户端连上后，发送token给服务端，服务端收到后回个ack=1给客户端

上线之后如果本地文件有持久化的数据，应该发送给服务端；当中间因为某些原因断开后，重新登录上，得发送本地数据

do_login  三次握手后，如果客户端没有发token，或者发了服务端没有接收到，这里采用select方法轮询如果一万微秒没有收到客户端的验证，客户端就直接显示连不上

服务端还得避免重复添加客户端，如果已经在线就显示已经在线，重复登录

服务端这边是采用时钟信号方式，时间到了就发送心跳给客户端，客户端回应后服务端反应堆去接收，并重新设置客户端isonline=10





客户端：

登录-三次握手；（在登录的时候，需要通过简单的验证确保服务端收到的登录请求都是由客户端发起的，所以需要通过简单的机制来保证这一点。这就得模拟简单的三次握手）

客户端连上后发送token给服务端，服务端收到后回个ack=1给客户端，客户端这边也得做个select防止服务器长时间没给通知

心跳接收；

上线之后如果本地有持久化的数据，就发给服务端；长时间收不到服务端信息，得重新登录pthread_cond_wait(),因为客户端不是无缘无故重新登录，是在等待一个信号，信号被激活才重新登录。客户端收到心跳时tmp_for_relogin=10;当为0时，认为和服务端已经断开得重新登录，发送信号重新登录；登录上后处理持久化数据



服务端：

do_login  三次握手后，如果客户端没有发token，或者发了服务端没有接收到，这里采用select方法轮询如果一万微秒没有收到客户端的验证，客户端就直接显示连不上

创建好监听套接字；

客户端来连；

（1.采用单独线程用来等待用户来登录系统

2.当通过三次握手的方式确定到达的主机为client端后，使用epoll_create将客户注册到反应堆中）

客户的存储；

（1.将认定为在线的用户存储在服务端的内存中，采用数组存储

2.请自己定义用户存储结构，在内部关联到连接的sockfd）

并发机制；

（在实现并发的时候，使用epoll搭配线程池的方式来做基本并发

1.使用一个epoll实例，用来承载已经经过三次握手，确定了客户端的身份的主机

2.上述主机，一定是用户数组中）

服务端还得避免重复添加客户端

服务端心跳；（1.遍历所有在线的客户列表

2.发送心跳信息

3.对已经满足剔除条件的客户端进行移除操作

tips：定义心跳的数据报文。参考比特掩码）

服务端发送心跳给客户端，客户端收到后给服务端回个ack，服务端在反应堆去接收，如果接收到了就重新设置该客户端isonline为10

隔段时间发起一次心跳 信号机制

服务端反应堆收到脚本数据后，将其放在线程池之后解析，然后插到数据库中去
