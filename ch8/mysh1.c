//shell 版本1 带提示符 能执行程序

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAXARGS     20  //最大命令数量
#define ARGLEN      100 //最大命令长度

int execute(char* arglist[]);   //执行命令函数
char* makestring(char* buf);    //将输入字符转换成字符串

int main()
{
    char *arglist[MAXARGS+1];
    int  numargs;
    char argbuf[ARGLEN];
    numargs = 0;

    while(numargs < MAXARGS)
    {
        printf("Arg[%d]?", numargs);    //提示输入参数
        if (fgets(argbuf, ARGLEN, stdin) && *argbuf != '\n')
        {
            //添加退出条件
            if (strncmp(argbuf,"exit",strlen(argbuf)-1) == 0 || *argbuf == EOF)
            {
                exit(1);
            }
            
            arglist[numargs++] = makestring(argbuf);
        }
        else
        {
            if (numargs > 0)
            {
                arglist[numargs] = NULL;

                int fork_ret = 0;
                fork_ret = fork();
                if (fork_ret == -1)
                    perror("fork error");              
                else if (fork_ret == 0)
                    execute(arglist);
                else
                {
                    signal(SIGINT, SIG_IGN);    //父进程忽略中断信号
                    wait(NULL);     //等待子进程退出
                    numargs = 0;
                    continue;
                }
            }
        }
    }

    return 0;
}

int execute(char *arglist[])
{
    execvp(arglist[0], arglist);
    perror("execvp");


    exit(1);
}

char * makestring(char *buf)
{
    char *cp;

    buf[strlen(buf) - 1] = '\0';

    cp = (char *)malloc(strlen(buf) + 1);
    if (cp == NULL)
    {
        fprintf(stderr, "no memory\n");
        exit(1);
    }

    strcpy(cp, buf);
    return cp;
}
