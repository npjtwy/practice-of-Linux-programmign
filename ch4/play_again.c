/*
 * 通过play_again 程序了解 如何控制终端驱动
 *了解如何处理中断信号
 * */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define ASK         "Do you want another transaction?"
#define TRIES       3 //尝试次数
#define SLEEPTIME   2
#define BEEP        putchar('\a')   //发出警告声


void ctrl_c_handle(int);    //处理ctrl-c中断
void tty_mode(int);     //保存0或恢复1 tty模式
void set_cr_noecho_mode();  //将tty设置为 char-by-char 和 不回显 模式
void set_nodelay_mode();    //设置tty启动非阻塞模式 用来实现超时相应
int  get_response(char * , int);
int get_ok_char();



int main()
{
	int response;
	
	tty_mode(0);	//保存当前tty模式
	set_cr_noecho_mode();
	set_nodelay_mode();
	signal(SIGINT, ctrl_c_handle);	//处理终端信号 将中断处理交给 ctrl_c_handle
	signal(SIGQUIT, SIG_IGN);	//忽略退出信号
	response = get_response(ASK, TRIES);
	if (response == 0)
	  printf("play again\n");
	tty_mode(1);	//恢复tty 
	return response;
}


int get_response( char * question, int maxtries)
{
	int input;

	printf("%s(y/n)\n", question);
	fflush(stdout);	//强制输出
	while(1)
	{
		sleep(SLEEPTIME);
		input = tolower(get_ok_char());
		if (input == 'y')
			return 0;
		if (input == 'n')
			return 1;
		if (maxtries-- == 0)
			return 2;
		BEEP;
	}
}

int get_ok_char()
{
	int c;
	while((c = getchar()) != EOF && strchr("yYnN", c) == NULL)
	  ;
	return c;
}

void set_cr_noecho_mode()
{
	struct termios ttystate;

	tcgetattr(0, &ttystate);	//获取当前tty设置
	ttystate.c_cflag &= ~ICANON;	//设置ICSNON位为0， 使输入字符立即传送给进程，不经过缓存
	ttystate.c_lflag &= ~ECHO;	//设置终端不回显
	ttystate.c_cc[VMIN] = 1;	//设置每次读取一个字符
	tcsetattr(0, TCSANOW, &ttystate);
}

void set_nodelay_mode()
{
	int termflags;
	termflags = fcntl(0, F_GETFL);	//获取文件描述符 0 的属性
	termflags |= O_NDELAY;
	fcntl(0, F_SETFL, termflags);
}

void tty_mode(int how)
{
	static struct termios	original_mode;
	static int				original_flags;
	static int				stored = 0;

	if(how == 0)
	{
		tcgetattr(0, &original_mode);
		original_flags= fcntl(0, F_GETFL);
		stored = 1;
	}
	else if (stored)
	{
		tcsetattr(0, TCSANOW, &original_mode);
		fcntl(0, F_SETFL, original_flags);
	}
}

void ctrl_c_handle(int signum)
{
	tty_mode(1);
	exit(1);
}
















