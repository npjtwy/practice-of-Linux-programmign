#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFSIZE 2048

int main(int argc, char *argv[])
{
	if (argc < 2){
		printf("please input target file\n");
		return 0;
	}
	
	char buf[BUFFSIZE];
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		perror("error");
		return -1;
	}
	while(read(fd, buf, BUFFSIZE) != 0)
	{
		printf("%s", buf);
	}

	if (close(fd)==-1)
	  perror("close error");

	return 0;
}

