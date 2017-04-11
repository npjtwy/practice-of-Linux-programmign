#include <sys/types.h>
#include <utmp.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "utmplib.c"

#define SHOWHOST

void show_time(long);
void show_info(struct utmp * utbufp);

int main()
{
	struct utmp *utbufp, *utmp_next();	//	utmp结构，存储读入的信息

	if (utmp_open(UTMP_FILE) == -1)
	{
		perror(UTMP_FILE);
		exit(1);
	}

	while((utbufp = utmp_next()) != NULLUT)
		show_info(utbufp);
	utmp_close();
	return 0;
}
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
	struct tm *p;
	p = localtime(&time_val);
	printf(" %d-%d-%d %d:%d ",1900+p->tm_year, 1+p->tm_mon, p->tm_mday,
				p->tm_hour, p->tm_min);	//年份加1900 月份加1
}
