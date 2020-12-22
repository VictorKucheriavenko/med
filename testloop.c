#include <stdio.h>

int
main()
{
 setbuf(stdout, NULL);
 
 int i;
 
 printf("for loop\n");
 for (i = 0; i < 10; i++)
 {
 	if (i == 5)
 		continue;
 	printf("i: %d\n", i);
 }
 
 printf("while loop\n");
 i = 0;
 while (i < 10)
 {
 	if (i == 5)
 		continue;
 	printf("i: %d\n", i);
 	i++;
 }
 
 return 0;
}