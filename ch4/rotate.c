#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

int main()
{
	int c;
	while((c = getc(stdin)) != EOF){
		if (c == 'z')
			c = 'a';
		else if (islower(c))
		  c++;
		putc(c,stdout);
	}
}


