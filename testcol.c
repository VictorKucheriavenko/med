#include <stdio.h>
#include "colors.h"

#define \
color(col)\
(printf("\x1B[%dm", col));

int
main()
{
 int i;
 char buf[20] = "write system call";
 
 color(3);
 write(1, buf, 5);
 for (i = 0; i <10; i++)
	 putchar('!');
 printf("test\n");
 return 0;
}