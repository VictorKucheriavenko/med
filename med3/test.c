
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char*
del (char *str, int o, int n)
{
 char *buf;
 int i;
 int len = strlen(str);
 buf = calloc( len + 1, 1 );

 if( o <= 0 || n < 0 )
    return NULL;



 if( o > len ) {
    strcpy(buf, str);
 } else


 if( o + n > len ) {
    strncpy(buf, str, o - 1);
 } else

 {
 strncpy(buf, str, o - 1);
 strcat(buf, str + o - 1 + n);
 }
return buf;
}



char*
insert (char *str1, int n, char *str2)
{
 char *buf;
 int i;
 buf = calloc( strlen(str1) + strlen(str2) + 1, 1);
 strncpy(buf, str1, n);
 strcat(buf, str2);
 strcat (buf, str1+n);
 return buf;
}

main(int argc, char* argv[]) {
int i, o, n;
o = atoi(argv[1]);
n = atoi(argv[2]);
char str[] = "123456789";
char *str_res = del(str, o, n);
if (str_res != NULL)
printf("%s\n", str_res );
else
printf("%p\n", str_res);
/*
printf("string, INSERT, 3:\n");
printf("%s\n", insert( "string", 3, "INSERT"));
*/
return 0;
}
