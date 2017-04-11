#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

int set_ticker(int);    //设置计时器相关属性
void killouch(int);     //计时器到点时执行的函数

int main(int ac, char* ag[])
{
    
    signal(SIGALRM, killouch);

    if (set_ticker(500) == -1)
        perror("set_ticker");
    else
        while(1)
            pause();
    return 0;


}
void killouch(int signum)
{
    pid_t ouchpid = 23884;
    fflush(stdout);
    if(kill(ouchpid, SIGINT) == -1)
        perror("killouch");
}


int set_ticker(int n_msecs)
{
    struct itimerval new_timeset;
    long n_sec, n_usecs;

    //设置秒数和微秒数
    n_sec = n_msecs / 1000;
    n_usecs = (n_msecs % 1000) * 1000L;

    new_timeset.it_interval.tv_sec = n_sec;
    new_timeset.it_interval.tv_usec = n_usecs;

    new_timeset.it_value.tv_sec = n_sec;
    new_timeset.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

