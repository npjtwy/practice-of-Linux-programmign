//简易 web 服务器
// 运行参数是 端口号
// ./webserver 12345
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "socklib.h"

#include "webserver.h"

int main(int argc, char const *argv[]) {
    int sock, fd;
    FILE *fpin;
    char request[BUFSIZ];

    if (argc == 1)
    {
        fprintf(stderr, "usage: ws portnum\n");
        exit(1);
    }

    sock = make_server_socket(atoi(argv[1]));

    if (sock == -1) exit(2);

    while (1) {
        /* code */
        fd = accept(sock, NULL, NULL);  //等待链接
        fpin = fdopen(fd, "r");

        fgets(request, BUFSIZ, fpin);
        printf("got a call: request =  %s\n", request );

		read_til_crnl(fpin);

		process_rq(request, fd);
		fclose(fpin);

    }
    return 0;
}

void read_til_crnl(FILE *fp)
{
	char buf[BUFSIZ];
	while (fgets(buf, BUFSIZ,fp) != NULL && strcmp(buf, "\r\n") != 0)
		;
}

//主要函数 处理各种可能的请求情况
void process_rq(char *rq, int fd)
{
	char cmd[BUFSIZ], arg[BUFSIZ];
	if (fork() != 0)
		return;
	strcpy(arg, "./");
	if (sscanf(rq, "%s %s",cmd, arg+2) != 2)
		return;
	if (strcmp(cmd, "GET") != 0)
		cannot_do(fd);
	else if (not_exist(arg))
		do_404(arg, fd);
	else if (isadir(arg))
		do_ls(arg,fd);
	else if (ends_in_cgi(arg))
		do_exec(arg, fd);
	else
		do_cat(arg,fd);
}

//头部信息
void header(FILE *fp, char * content_type/* arguments */) {
	fprintf(fp, "HTTP/1.0 200 OK\r\n");
	if (content_type)
		fprintf(fp, "Content - type: %s\r\n",content_type );
}
