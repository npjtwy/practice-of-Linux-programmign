//弹球游戏 1 
//单词在屏幕移动 按下s和f分别增加和减少单词移动速度 q退出

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <string.h>

#define MESSAGE "HELLO"
#define BLANK   "     "

int row;    //当前所在行
int col;    //当前所在列
int dirc;   //移动方向 -1向右 1向左

void move_msg(int); //处理移动的函数 当定时到时转向这个函数

int main()
{
    int delay;  //控制速度
    int newdelay;   
    int c;  //用户输入

    initscr();
    crmode();
    noecho();

    row = 10;
    col = 0;
    dirc = 1;
    delay = 200;    //200ms 

    move(row, col); //定位到初始位置
    addstr(MESSAGE);
    signal(SIGALRM, move_msg);
    set_ticker(delay);

    while(1)
    {
        newdelay = 0;
        c = getch();
        if (c == 'Q') break;
        if (c == ' ') dirc = -dirc;
        if (c == 'f' && delay > 2) newdelay = delay / 2;
        if (c == 's') newdelay = delay * 2;
        if (newdelay > 0)
            set_ticker(delay = newdelay);
    }

    endwin();
    return 0;
}

void move_msg(int signum)
{
    signal(SIGALRM, move_msg);  //重置信号处理
    move(row, col);
    addstr(BLANK);
    col += dirc;
    move(row, col);
    addstr(MESSAGE);
    refresh();

    //处理边界
    if (dirc == -1 && col <=0)
        dirc = 1;
    else if (dirc == 1 && col+strlen(MESSAGE) >= COLS)
        dirc = -1;
}
