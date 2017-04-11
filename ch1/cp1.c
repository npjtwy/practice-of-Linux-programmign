#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//自定义cp 命令从源文件以读方式打开，目标文件以写方式打开

#define BUFFSIZE 4096
#define COPYMODE 0644


void oops(const char*, const  char*);

int main(int argc, char const *argv[])
{
	int in_fd, out_fd, n_chars;
	char buf[BUFFSIZE];

	if(argc != 3)
	{
	  fprintf(stderr, "usage: %s source destination\n", *argv);
	  exit(1);
	}

	if((in_fd = open(argv[1], O_RDONLY)) == -1)
		oops("Cannot open ", argv[1]);
	if ((out_fd =creat(argv[2], COPYMODE)) == -1)
		oops("Cannot creat ", argv[2]);

	while((n_chars = read(in_fd, buf, BUFFSIZE)) > 0)
	{
		if (write(out_fd, buf, n_chars) != n_chars)
			oops("Write error to ", argv[2]);
	}
	
	if (n_chars == -1)
		oops("Read error from ", argv[2]);
	
	if (close(in_fd) == -1 || close(out_fd) == -1)
		oops("ERROR closing", "");
	return 0;	
}


void oops(const char *s1, const  char *s2)
{
	fprintf(stderr, "ERROR: %s ", s1);
	perror(s2);
	exit(1);
}

