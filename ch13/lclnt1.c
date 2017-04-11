//
// Created by yang on 17-4-9.
//


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

extern
void set_up();
extern
int get_ticket();
extern
int release_ticket();
extern
void shut_down();

void do_regular_work();

int main(int ac, char **av)
{
	set_up();
	if (get_ticket() != 0)
		exit(0);

	do_regular_work();
	release_ticket();
	shut_down();
}


void do_regular_work()
{
	printf("SuperSleep version 1.0 Running-Licensed Software\n");
	sleep(10);
}