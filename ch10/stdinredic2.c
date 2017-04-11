#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//使用dup dup2重定向
//dup : open -> close -> dup -> close
//dup2: open -> dup2 -> close

//#define CLOSE_DUP

int main(int argc, const char *argv[])
{
    int fd;
    int newfd;
    char line[BUFSIZ];
    fgets(line, 100, stdin); printf("%s\n", line);

    if ((fd = open("/etc/passwd", O_RDONLY)) == -1)
    {
        perror("Can't open file as fd 0\n");
        exit(1);
    }

#ifdef CLOSE_DUP
    close(0);
    newfd = dup(fd);    
#else
    newfd = dup2(fd, 0);
#endif

    if (newfd != 0)
    {
        fprintf(stderr, "Can't duplicate fd to 0\n");
        exit(1);
    }

    fgets(line, 100, stdin);
    printf("%s\n", line);
    return 0;
}
