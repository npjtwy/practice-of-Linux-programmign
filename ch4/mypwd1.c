/*
 * 自定义pwd命令:
 * 1. 用 stat 得到 . 的i-node N
 * 2. chdir ..切换到上层目录
 * 3. 找到N链接的名字，使用opendir, readdir, closedir
 *
 * */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

ino_t getinode(char *);		//从readdir获取iNode
void printpathto(ino_t);	//递归获取目录名
void inum_to_name(ino_t, char*, int);	//将inode转换成名字


//无法 打印出 /home 。。。。。
int main()
{
	printpathto(getinode("."));
	putchar('\n');
	return 0;
}

void printpathto(ino_t this_node)
{
	char its_name[BUFSIZ];
	if (getinode("..") != this_node)
	{
		chdir("..");
		inum_to_name(this_node, its_name, BUFSIZ);
		printpathto(getinode("."));
		printf("/%s", its_name);
	}

}

void inum_to_name(ino_t inode, char* namebuf, int bufsize)
{
	DIR *dir_ptr;
	struct dirent *direntp;

	//从当前目录信息中找当前目录名
	if ((dir_ptr = opendir(".")) == NULL)
	{
		fprintf(stderr, "ls1: cannot open .\n");
	}
	else
	{
		while((direntp = readdir(dir_ptr)) != NULL)
		{
			if(direntp->d_ino == inode)
			{
				strncpy(namebuf, direntp->d_name, bufsize+1);
				//namebuf[bufsize - 1] = '\0';
				closedir(dir_ptr);
				return;
			}
		}

		fprintf(stderr, "error looking for inum %d\n", inode);
		closedir(dir_ptr);
		exit(1);
	}
}

ino_t getinode(char * dirname )
{
	struct stat dirinfo;
	if (stat(dirname, &dirinfo) == -1)
	{
		perror(dirname);
		exit(1);
	}
	else
		return dirinfo.st_ino;
}
