#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define \
moveyx(y, x) \
( printf("\033[%d;%dH", (y), (x)) )

int
main()
{
 int i, j, k;
 char *p;
 char buf[100] = "a string";
 
 for (i = 0; i < 10000; i++)
 for (j = 0; j < 1000; j++)
// for (k = 0; k < 20; k++)
 {
 	p = malloc(1000000);
// 	free(p);
/*
 moveyx(10, 10);
 moveyx(5, 5);
*/ 
// 	char c[1000];
//	putchar(buf[0]);
//	putchar(' ');
//	puts(buf);
//	fprintf(stdout, "%s", buf);
//	write(1, buf, 8);
/*
for (k = 0; k < 8; k++)
putchar(buf[k]);
*/
 }
 
 
 return 0;
}