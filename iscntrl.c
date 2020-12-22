#include <stdio.h>
#include <ctype.h>

int
main()
{
 int i;
 for(i = 0; i < 65; i++)
	printf("%02d:%d\n", i, iscntrl(i));
 return 0;
}