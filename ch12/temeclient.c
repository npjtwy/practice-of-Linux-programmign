#include "socklib.h"

#define PORTNUM 13000
#define HOST    "localhost"
void  talk_with_server(int fd){
    char buf[BUFSIZ];
    int n;

    n = read(fd, buf, BUFSIZ);
    write(1, buf, n);
}
int main(int argc, char const *argv[]) {
    int fd;
    fd = connect_to_server(HOST, PORTNUM);
    if (fd == -1)
        exit(1);
    talk_with_server(fd);
    close(fd);
    return 0;
}
