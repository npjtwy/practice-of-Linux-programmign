/*
* 一个简单的基于数据报 的服务器 使用命令行传来的端口号建立 socket
* 然后进入循环接受和打印从客户端发来的数据报
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define oops(m,x) {perror(m); exit(x);}

extern
int make_dgram_server_socket(int);
extern
int get_internet_address(char *, int , int *, struct sockaddr_in*);

void say_who_called(struct sockaddr_in*);

int main(int ac, char const *av[]) {
	int 	port;
	int 	sock;
	char 	buf[BUFSIZ];
	size_t 	msglen;
	struct 	sockaddr_in saddr;	//客户端地址结构
	socklen_t 	saddrlen;

	if ((ac == 1) || (port = atoi(av[1])) <= 0)
		{
			fprintf(stderr, "usage: dgrecv portnumber\n" );
			exit(1);
		}


		if ((sock = make_dgram_server_socket(port)) == -1)
		{
			oops("cannot make socket", 2);
		}

		//接收消息

		saddrlen = sizeof(saddr);

		while ((msglen = recvfrom(sock,buf,BUFSIZ,0, (struct sockaddr *)&saddr,&saddrlen)) > 0) {
			buf[msglen] = '\0';
			printf("dgrecv: got a message : %s\n", buf);
			say_who_called(&saddr);
		}
	return 0;
}


void say_who_called(struct sockaddr_in* saddr)
{
	int portnum;
	char host[BUFSIZ];
	get_internet_address(host, BUFSIZ, &portnum, saddr);
	printf("from: %s: %d\n", host, portnum);
}
