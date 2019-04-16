#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<event.h>
#include<event2/bufferevent.h>

void accept_cb(int fd, short events, void* arg);
void socket_read_cb(bufferevent* bev, void* arg);
void event_cb(struct bufferevent *bev, short event, void *arg);
int tcp_server_init(int port, int listen_num);

int main(int argc, char** argv)
{
    //初始化网络监听
    int listener = tcp_server_init(9999, 10);
    if( listener == -1 )
    {
        perror(" tcp_server_init error ");
        return -1;
    }

    //创建事件管理器
    struct event_base* base = event_base_new();

    //添加监听客户端请求连接事件
    struct event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, base);

    //注册事件
    event_add(ev_listen, NULL);
    
    //循环监听事件
    event_base_dispatch(base);
    
    //释放事件
    event_base_free(base);

    return 0;
}

//事件回调函数-接受链接
void accept_cb(int fd, short events, void* arg)
{
    evutil_socket_t sockfd;

    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    sockfd = ::accept(fd, (struct sockaddr*)&client, &len );
    evutil_make_socket_nonblocking(sockfd);

    printf("accept a client %d\n", sockfd);

    struct event_base* base = (event_base*)arg;

    //创建bufferevent
    bufferevent* bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
    
    //设置回调函数
    bufferevent_setcb(bev, socket_read_cb, NULL, event_cb, arg);
    
    //设置bufferevent事件类型
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

//读取事件的回调函数
void socket_read_cb(bufferevent* bev, void* arg)
{
    char msg[4096];

    //bufferevent 读取函数
    size_t len = bufferevent_read(bev, msg, sizeof(msg));

    msg[len] = '\0';
    printf("recv the client msg: %s", msg);

    char reply_msg[4096] = "I have recvieced the msg: ";

    strcat(reply_msg + strlen(reply_msg), msg);
    
    //bufferevent输出函数
    bufferevent_write(bev, reply_msg, strlen(reply_msg));
}

//事件函数的回调函数
void event_cb(struct bufferevent *bev, short event, void *arg)
{
    if (event & BEV_EVENT_EOF)
        printf("connection closed\n");
    else if (event & BEV_EVENT_ERROR)
        printf("some other error\n");

    //这将自动close套接字和free读写缓冲区
    bufferevent_free(bev);
}



int tcp_server_init(int port, int listen_num)
{
    int errno_save;
    evutil_socket_t listener;

    listener = ::socket(AF_INET, SOCK_STREAM, 0);
    if( listener == -1 )
        return -1;

    //允许多次绑定同一个地址。要用在socket和bind之间
    evutil_make_listen_socket_reuseable(listener);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);

    if( ::bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0 )
        goto error;

    if( ::listen(listener, listen_num) < 0)
        goto error;


    //跨平台统一接口，将套接字设置为非阻塞状态
    evutil_make_socket_nonblocking(listener);

    return listener;

    error:
        errno_save = errno;
        evutil_closesocket(listener);
        errno = errno_save;

        return -1;
}