//将常用的 socket 打包 方便调用
#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define HOSTLEN     256
#define BACKLOG     1

int make_server_socket_q(int ,int);

int make_server_socket(int portnum)
{
    return make_server_socket_q(portnum, BACKLOG);
}

int make_server_socket_q(int portnum, int backlog)
{
    struct sockaddr_in  saddr;  //socket 地址结构（后面有_in代表用于互联网）
    //struct hostent      *hp;
    char                hostname[HOSTLEN];
    int                 sock_id;

    //step1 建立 socket
    sock_id = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_id == -1)
        return -1;

    //step2 给socket 分配地址 端口
    bzero((void*)&saddr, sizeof(saddr));    //初始话结构体为0

    gethostname(hostname, HOSTLEN);     //主机名称

    //将主机地址赋值给 socket
    //bcopy((void*)hp->h_addr_list, (void*)&saddr.sin_addr, hp->h_length);
	//saddr.sin_addr.s_addr = inet_addr("127.0.0.1")''
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY 代表所有地址
    //填写端口 地址簇信息
    saddr.sin_port = htons(portnum);
    saddr.sin_family = AF_INET;

    if (bind(sock_id, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
        return -1;

    //step3 允许socket接入呼叫

    if (listen(sock_id, 1) != 0)
        return -1;
    return sock_id;
}



int connect_to_server(char *host, int portnum)
{
    struct sockaddr_in  servadd;  //socket 地址结构（后面有_in代表用于互联网）
    struct hostent      *hp;
    int sock;
    //step1 建立 socket
    sock= socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        return -1;
    //step2 给socket 分配地址 端口
    bzero((void*)&servadd, sizeof(servadd));    //初始话结构体为0

    if ((hp = gethostbyname(host)) == NULL)
        return -1;

    //获取地址
    servadd.sin_addr = *((struct in_addr *)hp->h_addr);

    //servadd.sin_addr.s_addr = ;
    //填写端口 地址簇信息
    servadd.sin_port = htons(portnum);
    servadd.sin_family = AF_INET;

    if (connect(sock, (struct sockaddr*)&servadd, sizeof(servadd) )!= 0)
        return -1;

    return sock;
}
