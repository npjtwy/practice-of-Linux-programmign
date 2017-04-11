//pipe 和 fork 结合起来

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define CHILD_MESS  "child...\n"
#define PAR_MESS    "PARENTS...\n"

int main()
{
    int     pipefd[2];
    //pipefd[0] = 3;pipefd[1] = 4;
    int     len;
    char    buf[BUFSIZ];
    int     read_len;

    if (pipe(pipefd) == -1){
        perror("get pipe err:");exit(2);
    }
    switch(fork())
    {
        case -1:
            
            perror("fork err:");exit(2);
    
            break;
        case 0:
            len = strlen(CHILD_MESS);
            while(1)
            {
                if (write(pipefd[1], CHILD_MESS, len) != (len))
                    {
                    perror("write pipe err:");exit(2);
                    }
                sleep(3);
            }
            break;
        default:
            len = strlen(PAR_MESS);
            while(1)
            {
                if (write(pipefd[1], PAR_MESS, len) != (len))
                    {
                    perror("write pipe err:");exit(2);
                    }
                sleep(2);

                read_len = read(pipefd[0], buf, BUFSIZ);
                if (read_len <= 0)
                    break;
                buf[read_len] = '\0';
                write(1, buf, read_len+1);
            }
    }

    return 0;
}