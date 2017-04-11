#include <stdio.h>
#include <signal.h>
#define INPUTLEN 100

void inthandler(int);

int main()
{
    struct sigaction newhandler;
    sigset_t blocked;
    void inthandler();  //中断处理
    char x[INPUTLEN];

    newhandler.sa_handler = inthandler;
    newhandler.sa_flags = SA_RESETHAND | SA_RESTART;

    //设置阻塞 SIGQUIT 信号
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGQUIT);
    newhandler.sa_mask = blocked;

    if (sigaction(SIGINT, &newhandler, NULL) == -1)
        perror("sigaction");
    else
        while(1){
            fgets(x, INPUTLEN, stdin);
            printf("input: %s", x);
        }
    return 0;
}


void inthandler(int s)
{
    printf("Called with signal %d\n");
    sleep(s);
    printf("done handling signal %d\n", s);
}

