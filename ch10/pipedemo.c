#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//管道 
int main()
{
    int len, apipe[2];
    char buf[BUFSIZ];
    apipe[0] = 3;apipe[1] = 4;  //管道从fd=4写入 从fd=3 读出

    if (pipe(apipe) == -1)  //创建管道
    {
        perror("make pipe err:");
        exit(1);
    }

    
    while(fgets(buf, BUFSIZ, stdin))    //从标准输入读取
    {
        
        len = strlen(buf);
        
        //写入管道
        if (write(apipe[1], buf, len) == -1)
        {
            perror("write pipe err:");
            exit(1);
        }

        //从管道读出
        len = read(apipe[0], buf, BUFSIZ);
        if(len == -1)
        {
            perror("reading from pipe:");
            exit(1);
        } 

        //向标准输出写
        if (write(1, buf, len) != len)
        {
            perror("writing to stdout:");
            exit(1);
        }

    }

    return 0;
}