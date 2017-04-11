//弹球游戏多线程  多文本改进版版本

//啊啊啊 有问题

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define 	MAXMSG 10

#define		TUNIT 	20000	//更新画面时间

struct propset{		//每个线程所要使用的参数
	char *str;
	int row;
	int delay;
	int dir;
};

struct infobox
{
	int row;
	int col;
	char *str;
	int flag;
};


struct infobox propbox;

pthread_cond_t flag = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

void * animate(void *arg);		//每个字符串对应线程的处理函数  完成自身信息的更新
int setup(int , char *arg[], struct propset p[]);	//初始化
//更新屏幕函数  每个字符串对应线程发送消息给屏幕控制线程 调用此函数对屏幕进行更新
void * draw_screen(void *arg);


int main(int ac, char *av[]) {
	int 			c;
	pthread_t 		threads[MAXMSG];
	struct propset 	props[MAXMSG];
	int 			num_msg;
	int 			i;
	pthread_t 		control_thread;		//控制屏幕显示的线程

	propbox.col = -1;
	propbox.row = -1;
	propbox.str = NULL;
	propbox.flag = 0;

	if (ac == 1)
	{
		printf("usage: tanimate string ..\n");
		exit(1);
	}

	num_msg = setup(ac - 1, av, props);

	//创建线程

	if (pthread_create(&control_thread, NULL, draw_screen,&propbox))
	{
		fprintf(stderr, "error creating contorl_thread threads\n" );
		endwin();
		exit(1);
	}

	for (i =0; i < num_msg; i ++)
	{
		if (pthread_create(&threads[i], NULL, animate, &props[i]))
		{
			fprintf(stderr, "error creating threads\n" );
			endwin();
			exit(1);
		}
	}

	while (1) {
		c = getch();
		if ( c == 'Q')
			break;
		if (c == ' ')
			for ( i = 0; i < num_msg; i++)
				props[i].dir = -props[i].dir;

		if ( c >= '0' && c <= '9'){
			i = c - '0';
			if (i < num_msg)
				props[i].dir = -props[i].dir;
		}
	}

	pthread_mutex_lock(&mx);
	for (i = 0; i< num_msg ; i++)
		pthread_cancel(threads[i]);
	endwin();

	return 0;
}


void *animate(void * arg)
{
	struct propset *info = (struct propset*)arg;
	int len = strlen(info->str - 3);
	int col = rand() % (COLS - len -3);

	//循环调用屏幕控制线程函数来更新
	while (1) {
		usleep(info->delay * TUNIT);
		//更新信箱
		pthread_mutex_lock(&mx);
		if (propbox.flag != 0)
			pthread_cond_wait(&flag, &mx);

		propbox.col = col;
		propbox.row = info->row;
		propbox.str = info->str;
		propbox.flag = 1;

		pthread_cond_signal(&flag);
		pthread_mutex_unlock(&mx);


		col += info->dir;

		if (col <= 0 && info->dir == -1)
			info->dir = 1;
		else if( (col + len) >= COLS && info->dir == 1)
			info->dir = -1;
	}
	return NULL;
}

void * draw_screen(void *arg)
{
	while(1)
	{
		pthread_cond_wait(&flag, &mx);

		struct infobox *info = (struct infobox*)arg;
		move(info->row, info->col);
		addch(' ');
		addstr(info->str);
		addch(' ');
		move(LINES-1, COLS-1);
		refresh();

		pthread_mutex_lock(&mx);
		info->flag = 0;
		pthread_mutex_unlock(&mx);
		pthread_cond_signal(&flag);
	}
	return NULL;
}


//初始化每个线程的结构体参数
int setup(int nstrings, char **strings, struct propset props[])
{
	int num_msg = (nstrings > MAXMSG ? MAXMSG : nstrings);
	int i;
	srand(getpid());
	for (i = 0; i< num_msg; i ++)
	{
		props[i].str  = 	strings[i+1];
		props[i].row  = 	i;
		props[i].delay= 	1 + (rand() % 15);
		props[i].dir  =		(rand()%2) ? 1 : -1;
	}

	initscr();
	crmode();
	noecho();
	clear();

	mvprintw(LINES-1, 0, "'Q' to quit, '0'...'%d' to bounce", num_msg-1);

	return num_msg;
}
