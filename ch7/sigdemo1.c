#include <stdio.h>
#include <signal.h>

//信号捕捉例子

void sighandler(int);   //自定义信号处理函数

int main(int argc, const char *argv[])
{
    int i;

    signal(SIGINT, sighandler);//使用自定义函数处理中断信号
    while(1)
    {
        printf("hello %d\n", i++);
        sleep(5);
    }
    return 0;
}

void sighandler(int signum)
{
    printf("ouch!\n");
}
