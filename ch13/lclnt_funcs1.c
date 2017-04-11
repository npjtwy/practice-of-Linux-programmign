//许可证服务器版本1 客户端函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

static int pid = -1;
static int sd = -1;		//客户端通信socket
static struct sockaddr serv_addr;
static socklen_t serv_alen;
static char ticket_buf[128];	//许可证 buffer
static int have_ticket = 0;		//获得许可证标志

#define MSGLEN 			128
#define SERVER_PORTNUM 	2020
#define HOSTLEN 		512
#define oops(p) 		{perror(p); exit(1);}

//在 dgram.c中定义的工具函数
extern
int make_dgram_client_socket();
extern
int make_internet_address(char *, int, struct sockaddr_in *);

//获取许可证函数 成功返回许可证字符串 失败返回 NULL
char * do_transaction(char*);

//调试时打印信息工具函数
void narrate(char *, char *);
void syserr(char *msg1);


//set_up 函数 获得pid socket 服务器address
void set_up(){
	char 	hostname[BUFSIZ];
	pid = getpid();
	sd = make_dgram_client_socket();
	if (sd == -1)
		oops("cannot creat socket");
	gethostname(hostname, HOSTLEN);
	//获取服务其地址
	make_internet_address(hostname, SERVER_PORTNUM,(struct sockaddr_in*) &serv_addr);

	serv_alen = sizeof(serv_addr);
}

void shut_sown()
{
	close(sd);
}

//get_ticket 从服务器获取许可证
int get_ticket()
{
	char * response;
	char buf[MSGLEN];

	if (have_ticket)
		return 0;

	sprintf(buf, "HELO %d", pid);

	if ((response = do_transaction(buf)) == NULL)
	{
		return -1;
	}

	//处理从服务器接收的消息：
	if (strncmp(response, "TICK", 4) == 0)
	{
		strncpy(ticket_buf, response, 5);    //获得许可证id 许可证格式   TICK 63432749
		have_ticket = 1;
		narrate("got ticket", ticket_buf);
		return 0;   //消息正确 成功返回
	}

	if (strncmp(response, "FAIL", 4) == 0)
		narrate("Could not get ticket", response);
	else
		narrate("Unknown message:", response);

	return -1;
}

//向服务其归还许可证
int release_ticket()
{
	char buf[MSGLEN];
	char *response;

	if(! have_ticket)
		return 0;

	sprintf(buf, "GBYE %s",ticket_buf);
	if ((response = do_transaction(buf)) == NULL)
		return -1;

	if (strncmp(response, "THNX", 4) == 0)
	{
		narrate("released ticket OK", "");
	}

	if (strncmp(response, "FAIL", 4) == 0)
		narrate("released faild", response + 5);
	else
		narrate("Unknown message :", response);
}

//向服务器发送请求 然后的回应
//返回指向消息的字符串或者 NULL
char * do_transaction(char *msg)
{
	static char buf[BUFSIZ];
	struct sockaddr retaddr;
	socklen_t       addrlen = sizeof(retaddr);
	int ret;

	ret = sendto(sd, msg, strlen(msg), 0, &serv_addr, serv_alen);
	if (ret == -1)
	{
		syserr("sendto");
		return  NULL;
	}

	ret = recvfrom(sd, buf, MSGLEN, 0, &retaddr, &addrlen);
	if (ret = -1)
	{
		syserr("recvfrom");
		return NULL;
	}

	return  buf;
}

void narrate(char *msg1, char *msg2)
{
	fprintf(stderr, "CLIENT[%d] : %s %s\n", pid , msg1, msg2);
}

void syserr(char *msg1)
{
	char    buf[BUFSIZ];
	sprintf(buf, "CLIENT[%d] : %s ", pid, msg1);
	perror(buf);
}