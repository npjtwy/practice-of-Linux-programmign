#include <sys/types.h>
#include <utmp.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>


#define SHOWHOST
void show_info(struct utmp * utbufp);
int main()
{
	struct utmp current_record;	//	utmp结构，存储读入的信息
	int			utmpfd;	//文件描述符
	int			reclen = sizeof(current_record);

	if ((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1)
	{
		perror(UTMP_FILE);
		exit(1);
	}

	while( read(utmpfd, &current_record, reclen) == reclen)
		show_info(&current_record);
	close(utmpfd);
	return 0;
}
void show_time(long);
void show_info(struct utmp *utbufp)
{
	if (utbufp->ut_type != USER_PROCESS)
		return;	//修改  只显示当前活动用户
	printf("%-8.8s", utbufp->ut_name);
	printf(" ");
	printf("%-8.8s", utbufp->ut_line);
	printf(" ");
	show_time(utbufp->ut_time);	//使用ctime函数转换时间
	printf(" ");
#ifdef SHOWHOST
	if (utbufp->ut_host[0] != '\0')
		printf("(%s)", utbufp->ut_host);
#endif
	printf("\n");
}

void show_time(long time_val)
{
//	char* cp;
	struct tm *p;
	p = localtime(&time_val);
	printf(" %d-%d-%d %d:%d ",1900+p->tm_year, 1+p->tm_mon, p->tm_mday,
				p->tm_hour, p->tm_min);	//年份加1900 月份加1
	//cp = ctime(&time_val);
	//printf("%12.12s", cp + 4);
}
