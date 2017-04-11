//实现 more 功能 版本01
//读取并打印24行信息

#include <stdio.h>
#include <stdlib.h>
#define PAGELEN 24
#define LINELEN 256

void do_more(FILE *fp);
int see_more();

int main(int argc, char const *argv[]) {
    FILE *fp;
    if (argc == 1)
        do_more(stdin);
    else
    {
        while (-- argc)
        if ((fp = fopen(* ++ argv, "r")) != NULL)
        {
            do_more(fp);
            fclose(fp);
        }
        else
            exit(1);
    }
    return 0;
}

void do_more(FILE *fp) {
    //读取一页内容 然后调用see_more查看更多信息
    char line[LINELEN];
    int num_of_lines = 0;
    int see_more(),reply;
    while(fgets(line, LINELEN, fp))
    {
        if (num_of_lines == PAGELEN)
        {
            reply = see_more();
            if (reply == 0)
                break;
            num_of_lines -= reply;
        }
        if(fputs(line, stdout) == EOF)
            exit(1);
        num_of_lines++;
    }
}

int see_more()
{
    int c;
    printf("\033[7m more?\033[m");
    while ((c = getchar()) != EOF) {
        if (c == 'q')
            return 0;
        if (c == ' ')
            return PAGELEN;
        if (c == '\n')
            return 1;
        /* code */
    }
    return 0;
}


