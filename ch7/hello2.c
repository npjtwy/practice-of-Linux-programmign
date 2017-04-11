#include <stdio.h>
#include <curses.h>

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
	}
	refresh();
	getch();
	endwin();

	return 0;
}

