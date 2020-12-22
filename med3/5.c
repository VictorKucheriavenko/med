#include <stdio.h>

int
main()
{
 int a, b = 0;
 while( 1 ) {
    scanf("%d", &a);
    if(a)
      printf("%d != 0\n", a);
    if(!a)
      printf("%d == 0\n", a);
    printf("b: %d\n", b);
   fprintf(stderr, "malloc failed\n");
 }
 return 0;
}
