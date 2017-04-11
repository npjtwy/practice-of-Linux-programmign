#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define HOSTLEN 256

int make_internet_address(char *, int, struct sockaddr_in *); //获取地址结构

int make_dgram_server_socket(int portnum) //根据端口号 建立socket
{
  struct sockaddr_in saddr;
  char hostname[HOSTLEN];
  int sock_id;

  sock_id = socket(PF_INET, SOCK_DGRAM, 0);
  if (sock_id == -1)
    return -1;

  gethostname(hostname, HOSTLEN);
  make_internet_address(hostname, portnum, &saddr);

  if (bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    return -1;
  return sock_id;
}

int make_dgram_client_socket() {
	//客户端没有指定端口号 因此系统随机分配
  return socket(PF_INET, SOCK_DGRAM, 0); //客户端socket
}

int make_internet_address(char *hostname, int port, struct sockaddr_in *addrp) {
  struct hostent *hp;

  bzero((void *)addrp, sizeof(struct sockaddr_in));

  hp = gethostbyname(hostname);
  printf("%s\n", hp->h_name);
  if (hp == NULL)
    return -1;
  addrp->sin_addr = *((struct in_addr *)hp->h_addr);

  addrp->sin_port = htons(port);
  addrp->sin_family = AF_INET;
  return 0;
}

//从给定的地址结构中获取信息
int get_internet_address(char *host, int len , int *portp, struct sockaddr_in * addrp)
{
	strncpy(host, inet_ntoa(addrp->sin_addr), len);
	*portp = ntohs(addrp->sin_port);
	return 0;
}
