#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#define OOPS(msg) {perror(msg), exit(1);}

int main(int ar, char **av)
{
    struct sockaddr_in  servadd;  //socket 地址结构（后面有_in代表用于互联网）
    struct hostent      *hp;
    char                message[BUFSIZ];
    int                 sock_id, sock_fd;
    int                 messlen;

    //step1 建立 socket
    sock_id = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_id == -1)
        OOPS("socket");
    
    //step2 给socket 分配地址 端口 
    bzero((void*)&servadd, sizeof(servadd));    //初始话结构体为0

    gethostbyname(av[1]); 

    servadd.sin_addr.s_addr = htonl(INADDR_ANY);
    //填写端口 地址簇信息
    servadd.sin_port = htons(atoi(av[2]));
    servadd.sin_family = AF_INET;

    if (connect(sock_id, (struct sockaddr*)&servadd, sizeof(servadd) )!= 0)
        OOPS("connect");


    messlen = read(sock_id, message, BUFSIZ);
    if (messlen == -1)
        OOPS("read");
    if (write(1, message, messlen) != messlen)
        OOPS("write to stdout");
    close(sock_id);

    return 0;
}