//
// mypipe  接受两个参数 分别代表两个命令
//将第一个命令的输出重定向到第二个命令的输入

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int ag, char **av)
{
    if (ag != 3)
    {
        fprintf(stderr, "usage: pipe cmd1 cmd2\n");
        exit(1);
    }

    int thepipe[2], newfd, pid;

    if (pipe(thepipe) == -1)
    {
        perror ("make pipe err:");
        exit(1);
    }

    if ((pid = fork()) == -1)
    {
        perror("fork err:");
        exit(1);
    }
    //父进程 处理第二个命令
    else if (pid > 0)
    {
        close(thepipe[1]);

        //将输入重定向到 管道
        if (dup2(thepipe[0], 0) == -1)
        {
            perror("dup2 err:");
            exit(1);
        }
        close(thepipe[0]);  //只保留 fd=0 的链接

        execlp(av[2], av[2], NULL);
        perror(av[2]);
    }
    //子进程处理第一个命令
    else if (pid == 0)
    {
        close(thepipe[0]);

        //将输出重定向到 管道
        if (dup2(thepipe[1], 1) == -1)
        {
            perror("dup2 err:");
            exit(1);
        }
        close(thepipe[1]);  //只保留 fd=1 的链接

        execlp(av[1], av[1], NULL);
        perror(av[1]);
    }

    return 0;
}