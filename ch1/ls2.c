#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
//ls版本2
void do_ls(char []);
void do_stat(char *);	//打印文件状态
void show_file_info(char*, struct stat *);
void mode_to_letters(mode_t , char[]);
char * uid_to_name(uid_t);
char * gid_to_name(gid_t);


int main(int ac, char * av[])
{
	if (ac == 1)
	  do_ls(".");
	else
	{
		while( --ac)
		{
			printf("%s:\n", *++av);
			do_ls(*av);
		}
	}

	return 0;
}

void do_ls(char dirname[])
{
	DIR * dir_ptr;
	struct dirent *direntp;

	if ((dir_ptr = opendir(dirname)) == NULL)	//打开目录
	{
		fprintf(stderr, "ls1: cannot open%s\n", dirname);
	}
	else
	{
		while((direntp = readdir(dir_ptr)) != NULL)	//读取目录信息， 存储在目录的结构指针中
		  do_stat(direntp->d_name);
		closedir(dir_ptr);
	}
}

void do_stat(char *filename)
{
	struct stat info;	//存储文件状态信息
	if(stat(filename, &info) == -1)	//stat 将 filename 中的状态信息存储在info中
		perror(filename);
	else
		show_file_info(filename, &info);
}

void show_file_info(char* filename, struct stat *infop)
{
	//打印保存在 struct stat中的文件信息
	void mode_to_letters();
	//以上是用到的函数的声明
	char modestr[11];	//用来存储 文件类型说明
	
	mode_to_letters(infop->st_mode, modestr);	//将文件类型转换成合适的形式

	printf("%s ", modestr);	//文件类型及权限信息
	printf("%-4d", (int)infop->st_nlink);	//文件当前链接信息
	printf("%-8s", uid_to_name(infop->st_uid));	//用户名
	printf("%-8s", gid_to_name(infop->st_gid));	//组名
	printf("%-8ld", (long)infop->st_size);	//文件大小
	printf("%.12s", 4 + ctime(&infop->st_mtim));	//修改时间
	printf(" %s\n", filename);
}

void mode_to_letters(mode_t  mode, char str[])
{
	strcpy(str, "----------");	//10个字符 表示文件类型信息
	
	//下面判断文件类型以及权限
	if (S_ISDIR(mode)) str[0] = 'd';
	if (S_ISCHR(mode)) str[0] = 'c';	//字符设备
	if (S_ISBLK(mode)) str[0] = 'b';	//块设备
	
	//判断用户权限权限
	if (mode & S_IRUSR) str[1] = 'r';	 
	if (mode & S_IWUSR) str[2] = 'w';	 
	if (mode & S_IXUSR) str[3] = 'x';	 
	
	//判断组权限
	if (mode & S_IRGRP) str[4] = 'r';
	if (mode & S_IWGRP) str[5] = 'w';
	if (mode & S_IXGRP) str[6] = 'x';

	//判断其他用户权限
	if (mode & S_IROTH) str[7] = 'r';
	if (mode & S_IWOTH) str[8] = 'w';
	if (mode & S_IXOTH) str[9] = 'x';
}

char* uid_to_name(uid_t uid)
{
	struct passwd* pw_p;
	static char numstr[10];
	
	if ((pw_p = getpwuid(uid)) == NULL)
	{
		sprintf(numstr, "%d", uid);	//如果获取用户名失败 则打印用户ID
		return numstr;
	}
	else
	  return pw_p->pw_name;
}

char* gid_to_name(gid_t gid)
{
	struct group * gr_p;
	static char numstr[10];

	if ((gr_p = getgrgid(gid)) == NULL)
	{
		sprintf(numstr, "%d", gid);
		return numstr;
	}
	else
	  return gr_p->gr_name;
}


