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
#include <malloc.h>
#include <sys/ioctl.h>

//ls版本3 实现输出排序 和 参数为目录
//还有许多不足 如没有检查内存分配成功与否  结构安排不合理 等等

int get_win_size(struct winsize *size);
void do_ls(char []);
void do_ls_l(char []);
void do_stat(char *);	//打印文件状态
void show_file_info(char*, struct stat *);
void mode_to_letters(mode_t , char[]);
char * uid_to_name(uid_t);
char * gid_to_name(gid_t);



//按照名称排序
static int
compar(const void* p1, const void *p2)
{
    return strcmp((*(struct dirent **)p1)->d_name, (*(struct dirent **)p2)->d_name);
}


void qsort(void *base, size_t nmemb, size_t size,
            int (*compar)(const void *, const void *));


//获取窗口大小
int get_win_size(struct winsize *size)
{
    if (isatty(STDOUT_FILENO) == 0)
        exit(1);
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, size)<0)
    {
        perror("ioctl TIOCGWINSZ error");
        exit(1);
    }

    return 0;
}

//只需列出档期内文件夹下的文件名
void do_ls(char * dirname)
{
    size_t total = 0;    //记录目录下文件总数
    DIR * dir_ptr;
	struct dirent *direntp;
    char ** name_array;   //保存文件名的数组
    size_t array_size = 64; //初始时数组大小是64
    size_t max_name_len = 0;    //最大文件名长度 字节为单位
    struct winsize size;    //窗口大小信息

    get_win_size(&size);
    name_array = (char **)malloc(sizeof(char[MAXNAMLEN+1]) * array_size);  //一次性分配64个单元 不够再分

	if ((dir_ptr = opendir(dirname)) == NULL)	//打开目录
	{
		fprintf(stderr, "ls1: cannot open%s\n", dirname);
	}
	else
	{
		while((direntp = readdir(dir_ptr)) != NULL)	//读取目录信息， 存储在目录的结构指针中
        {

            //重新分配内存
            if(total == array_size)
            {   array_size =  array_size * 2;
                name_array = (char **)realloc(name_array,sizeof(char[MAXNAMLEN+1]) * array_size);
            }
            if (strlen(direntp->d_name) > max_name_len)
                max_name_len = strlen(direntp->d_name);     //更新最大名字长度
            *(name_array + total) = direntp->d_name;	//向数组里填充数据
            total++;    //文件数加1
        }
        size_t col = size.ws_col / (max_name_len+1);    //每行最多放置的文件数量
        size_t i=0 ,j = 0;

        printf("total:%d\n", total);
        for(;i < total; i++)
        {
            if (**(name_array+i) == '.')
                continue;
            else{
                printf("%-*s",max_name_len+1, *(name_array + i));    //控制输出宽度
                //不能处理文件名含有汉字情况
                j++;
                if( j % col == 0)
                    printf("\n");//达到每行最多数量 回车
            }
        }
        printf("\n");

        closedir(dir_ptr);

    }
}

//列出详细信息
void do_ls_l(char dirname[])
{
    size_t total = 0;    //记录目录下文件总数
	DIR * dir_ptr;
	struct dirent *direntp;
    struct dirent ** dparray;   //目录数组
    size_t array_size = 64; //初始时数组大小是64
    dparray = (struct dirent **)malloc(sizeof(direntp) * array_size);  //一次性分配64个单元 不够再分

	if ((dir_ptr = opendir(dirname)) == NULL)	//打开目录
	{
		fprintf(stderr, "ls1: cannot open%s\n", dirname);
	}
	else
	{
		while((direntp = readdir(dir_ptr)) != NULL)	//读取目录信息， 存储在目录的结构指针中
        {

            //重新分配内存
            if(total == array_size)
            {   array_size = array_size*2;
                dparray = (struct dirent **)realloc(dparray,sizeof(direntp) * array_size);
            }

            *(dparray + total) = direntp;	//向数组里填充数据
            total++;    //文件数加1
        }
        qsort(dparray, total, sizeof(struct dirent *), compar); //按照名称排序
        size_t i = 0;
        for (i = 0; i < total; i++)
		      do_stat((*(dparray+i))->d_name);
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

	char modestr[11];	//用来存储 文件类型说明

	mode_to_letters(infop->st_mode, modestr);	//将文件类型转换成合适的形式

	printf("%s ", modestr);	//文件类型及权限信息
	printf("%-4d", (int)infop->st_nlink);	//文件当前链接信息
	printf("%-8s", uid_to_name(infop->st_uid));	//用户名
	printf("%-8s", gid_to_name(infop->st_gid));	//组名
	printf("%-12ld", (long)infop->st_size);	//文件大小
	printf("%.12s", 4 + ctime((const time_t*)&infop->st_mtim));	//修改时间
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


int main(int ac, char * av[])
{
	if (ac == 1)
	  do_ls(".");
    if (ac == 2)
    {
        if (*av[1] == '-')
            do_ls_l(".");
        else{
            chdir(av[1]);
            do_ls(".");
        }
    }
    if(ac > 2)
    {
        int flag = 1;
        if (*av[1] == '-')
            flag = 2;
        while(ac != flag)
        {
            chdir(av[ac-1]);
            if (flag == 2)
                do_ls_l(".");
            if (flag == 1)
                do_ls(".");
            ac--;
        }

    }
	return 0;
}
