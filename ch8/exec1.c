//编写程序运行另一个程序
//

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
    char * arglist[3] = {"ls", "-l", 0};
    printf("开始运行ls：\n");
    execvp("ls", arglist);
    printf("ls运行完毕\n");
    return 0;
}
