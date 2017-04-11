//使用 socklib.c 的timeserve
#include "socklib.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define PORTNUM 13000

void process_request(int);
void child_waiter(int);

int main(int argc, const char *argv[])
{
    int sock, fd;
    signal(SIGCHLD, child_waiter);
    if ((sock = make_server_socket(PORTNUM)) == -1)
        {
            perror("make_server_socket");
            exit(1);
        }
    while(1)
    {
        fd = accept(sock, NULL, NULL);
        if (fd == -1)
            break;
        process_request(fd);
        close(fd);
    }
    return 0;
}

void process_request(int fd)
{
    if (fork() == 0)
    {
        dup2(fd,1);
        close(fd);
        execlp("data", "date", NULL);
        perror("execlp");exit(1);
    }
}

void child_waiter(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* code */
    }
}
