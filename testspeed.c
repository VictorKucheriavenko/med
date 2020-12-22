#include <stdio.h>
#include <stdlib.h>
#include "macros.h"

#define moveyx(y, x) ( printf("\033[%d;%dH", (y), (x)) )
#define movex(x) ( printf("\033[;%dH", (x)) )
#define movey(y) ( printf("\033[%d;H", (y)) )
#define clreol() (printf("\033[K") )
#define CLR "\033[K"
#define CLL "\033[2K"
#define clear_all() ( printf("\033[2J") );
#define clear_down() ( printf("\033[0J") );

int
main()
{
 setbuf(stdout, NULL);
 int i;
 char buf[1] = {'a'};
 
 for (i = 0; i < 100000; i++)
 {
	putchar('b');
//	write(1, buf, 1);
// 	putchar('\n');
//	moveyx(20, 10);
//	moveyx(14, 4);
 }
// clear_all();
 putchar('\n');
 
 return 0;
}