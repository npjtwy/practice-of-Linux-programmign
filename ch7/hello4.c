#include <stdio.h>
#include <curses.h>
#include <stdlib.h>

int main()
{
	initscr();

	clear();
	int i = 0;
	for (i = 0; i < LINES; i++)
	{
		move(i,i+1);
		if (i % 2 == 1)
		  standout();
		addstr("Hello World");
		if (i % 2 == 1)
		  standend();
		refresh();
		sleep(1);
		move(i, i+1);
		addstr("                            ");
	}
	endwin();
	getch();

	return 0;
}

