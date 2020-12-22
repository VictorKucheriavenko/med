#include <stdio.h>

int main()
{
 int i;
 char buf[BUFSIZ];
 setvbuf(stdout, buf, _IONBF, BUFSIZ);
 setvbuf(stdin,  buf, _IONBF, BUFSIZ);
    printf("lksjfj\n");
    char c[10] = {'\0'};
    scanf("%9s", c);
    //scanf("%9s", c);
    //gets(c);
    fflush(stdin);
    fflush(stdout);
    printf("%s, and left is: %d\n", c, getchar());
    for(i = 0; i < 3; i++)
    printf("%s, and left is: %d\n", c, getchar());
    return 0;
}
