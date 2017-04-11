#include "smsh.h"

//处理命令字符串的主要函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void fatal(char *, char*, int);

//从文件中读取字符 存入字符串
char * next_cmd(char *prompt, FILE *fp)
{
    char    *buf;   //buffer
    int     bufspace = 0;   //字符串大小
    int     pos = 0;    //记录当前在字符串的位置
    int     c;  //输入字符

    printf("%s", prompt);
    while ((c = getc(fp) )!= EOF)
    {

        //判断空间大小 不够就重新分配
        if (pos + 1 >= bufspace)
        {
            if (bufspace == 0)
                buf = emalloc(BUFSIZ);
            else
            {
                buf = erealloc(buf, bufspace + BUFSIZ);
            }
            bufspace += BUFSIZ;
        }

        if (c == '\n')  //到达字符串结尾？
        {
            break;
        }

        buf[pos++] = c; //将字符存入buffer
    }

    if (c == EOF && pos == 0)
        return NULL;

    buf[pos] = '\0';    //字符串末尾处理
    return buf;
}

//定义宏 判断空白字符
#define is_delim(x) ((x) == ' ' || (x) == '\t')

char * newstr(char *s , int len); //处理单个命令字符串
char ** splitline(char *line)
{
    char    **args;     //命令列表 返回值
    int     spots = 0;  //列表容量
    int     bufspace;   //buf 大小
    int     argnum = 0;     //列表已用容量
    char    *cp = line; //迭代字符串的指针
    char    *start;     //指向单个命令字符串的开始位置
    int     len;

    if (line == NULL)
        return NULL;

    args        = emalloc(BUFSIZ);
    bufspace    = BUFSIZ;
    spots       = BUFSIZ / sizeof(char *);

    while (*cp != '\0')
    {
        while (is_delim(*cp))
        {
            cp++;
        }

        if (*cp == '\0')
            break;

        //确保数组还有足够的空间容纳

        if (argnum + 1 >= spots)
        {
            args = (char**)erealloc(args, bufspace+BUFSIZ);

            //更新变量
            bufspace += BUFSIZ;
            spots += (bufspace/sizeof(char *));
        }

        start = cp;
        len = 1;
        while(*++cp != '\0' && !(is_delim(*cp)))
            len++;
        args[argnum++] = newstr(start, len);
    }
    args[argnum] = NULL;
    return args;

}

char * newstr(char *s , int len)
{
    char *rv = emalloc(len + 1);
    rv[len] = '\0';
    strncpy(rv, s, len);
    return rv;
}

void freelist(char **list)
{
    char **cp = list;
    while(*cp)
        free(*cp++);
    free(list);
}

void * emalloc(size_t n)
{
    void *rv;
    if ((rv = malloc(n)) == NULL)
        fatal("out of memory", "",1);
    return rv;
}

void *erealloc(void *p, size_t n)
{
    void *rv;
    if ((rv = realloc(p, n)) == NULL)
        fatal("realloc() failed", "", 1);
    return rv;
}