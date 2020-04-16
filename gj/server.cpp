#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<event.h>
#include<event2/listener.h>
#include<event2/bufferevent.h>
#include<event2/thread.h>

void listener_cb(evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg);
void socket_read_cb(bufferevent *bev, void *arg);
void socket_event_cb(bufferevent *bev, short events, void *arg);

int main()
{
  //开启线程安全
  //evthread_use_pthreads();

  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;   //服务器IP地址--允许连接到所有本地地址上
  sin.sin_port = htons(9999);

  //创建事件管理器
  event_base *base = event_base_new();

  //创建监听器
  evconnlistener *listener
    = evconnlistener_new_bind(base, listener_cb, base,
        LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
        -1, (struct sockaddr*)&sin,
        sizeof(struct sockaddr_in));

  //循环监听事件
  event_base_dispatch(base);

  //释放监听器
  evconnlistener_free(listener);

  //释放事件管理器
  event_base_free(base);

  return 0;
}


//一个新客户端连接上服务器了
//当此函数被调用时，libevent已经帮我们accept了这个客户端。该客户端的
//文件描述符为fd
void listener_cb(evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sock, int socklen, void *arg)
{
  printf("accept a client %d\n", fd);

  event_base *base = (event_base*)arg;

  //为这个客户端分配一个bufferevent
  bufferevent *bev =  bufferevent_socket_new(base, fd,
      BEV_OPT_CLOSE_ON_FREE);

  //设置回调函数
  bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, NULL);

  //启动相应监听事件
  bufferevent_enable(bev, EV_READ | EV_PERSIST);
}


void socket_read_cb(bufferevent *bev, void *arg)
{
  char msg[4096];

  size_t len = bufferevent_read(bev, msg, sizeof(msg)-1 );

  msg[len] = '\0';
  printf("server read the data %s\n", msg);

  char reply[] = "I has read your data";
  bufferevent_write(bev, reply, strlen(reply) );
}

void socket_event_cb(bufferevent *bev, short events, void *arg)
{
  if (events & BEV_EVENT_EOF)
    printf("connection closed\n");
  else if (events & BEV_EVENT_ERROR)
    printf("some other error\n");

  //这将自动close套接字和free读写缓冲区
  bufferevent_free(bev);
}
