//重定向输入到文件

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
    int fd;
    char line[BUFSIZ];
    fgets(line, 100, stdin); printf("%s\n", line);

    close(0);
    if ((fd = open("/etc/passwd", O_RDONLY)) == -1)
    {
        perror("Can't open file as fd 0\n");
        exit(1);
    }

    fgets(line, 100, stdin);
    printf("%s\n", line);
    return 0;
}
