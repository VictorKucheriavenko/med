#include <stdio.h>
#include <string.h>

/*
int
pr(char type, char* format)
{
 printf("");
}
printf("scr->head: %d\n", scr->head);
*/

#define \
prv(type, var)\
{\
char buf[100]={0};\
buf[0] = '%';\
buf[1] = type;\
buf[2] = 10;\
printf(buf, var);\
/*strcat(buf, "(type)");*/\
/*printf("buf:%s\n", buf);*/\
}


int
main ()
{
//printf("d:%d\n", 'd');
 int a = 17;
 prv(100, a);
 return 0;
}