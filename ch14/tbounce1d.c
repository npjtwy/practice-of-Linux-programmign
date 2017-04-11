//弹球游戏多线程版本

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define 	MESSAGE "hello"
#define 	BLANK 	"     "

int row;
int col;
int dir;
int delay;

void *moving_msg(void *);	//线程函数

int main(int argc, char const *argv[]) {
	int ndelay;
	int c;		//接收用户输入
	pthread_t msg_thread;

	initscr();
	crmode();
	noecho();
	clear();

	row = 10;
	col - 0;	//初始位置

	dir  = 1;
	delay = 200;	//200ms

	if ((pthread_create(&msg_thread, NULL, moving_msg, (void *)MESSAGE)) != 0)
	{
		perror("creating thread err:");
		endwin();
		exit(1);
	}

	while (1) {
		ndelay = 0;
		c = getch();

		if ( c == 'Q')
			break;
		if (c == ' ')
			dir = -dir;
		if (c == 'f' && delay > 2) ndelay = delay / 2;
		if (c == 's') ndelay = delay * 2;
		if (ndelay > 0)
			delay = ndelay;
	}
	pthread_cancel(msg_thread);
	endwin();

	return 0;
}

void *moving_msg(void * msg)
{
	while (1) {
		usleep(delay * 1000);
		move(row, col);

		addstr((char*)BLANK);
		col += dir;

		if (col <= 0 && dir == -1)
			dir = 1;
		else if (col + strlen((char*)msg) >= COLS && dir == 1)
			dir = -1;

		addstr((char*)msg);
		refresh();

	}
	return NULL;
}
