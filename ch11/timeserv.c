/*
socket 基础练习  实现一个简单的 返回时间的服务器
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define PORTNUM     13000   //服务端口
#define HOSTLEN     256
#define OOPS(msg)   {perror(msg); exit(1);}

int main(int ac, char *av[])
{
    struct sockaddr_in  saddr;  //socket 地址结构（后面有_in代表用于互联网）
    struct hostent      *hp;
    char                hostname[HOSTLEN];
    int                 sock_id, sock_fd;
    FILE                *sock_fp;
    char                *ctime();
    time_t              thetime;

    //step1 建立 socket
    sock_id = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_id == -1)
        OOPS("socket");

    //step2 给socket 分配地址 端口
    bzero((void*)&saddr, sizeof(saddr));    //初始话结构体为0

    gethostname(hostname, HOSTLEN);     //主机名称

    //将主机地址赋值给 socket
    //bcopy((void*)hp->h_addr_list, (void*)&saddr.sin_addr, hp->h_length);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //填写端口 地址簇信息
    saddr.sin_port = htons(PORTNUM);
    saddr.sin_family = AF_INET;

    if (bind(sock_id, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
        OOPS("bind");

    //step3 允许socket接入呼叫

    if (listen(sock_id, 1) != 0)
        OOPS("listen");


    //主循环  接受请求 发送消息 关闭链接

    while(1)
    {
        sock_fd = accept(sock_id , NULL, NULL);     //等待链接
        printf("connected!!!\n");
        if (sock_fd == -1)
            OOPS("accept");

        sock_fp = fdopen(sock_fd, "w"); //准备向链接写数据
        if (sock_fp == NULL)
            OOPS("fdopen");

        thetime = time(NULL);

        fprintf(sock_fp, "the time is ...\n");
        fprintf(sock_fp, "%s", ctime(&thetime));

        fclose(sock_fp);
    }

return 0;
}
