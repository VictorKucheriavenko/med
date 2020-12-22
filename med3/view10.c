#include <stdio.h>

char* open_buf(char* path);

int view10( char* buf ); 

int
main(int argc, char* argv[])
{
 if (argc!=2) return 0;
 int i;

 for( i = 0; i < 8; i++)
    printf("1234567890");
 printf("\n");

 char *buf;
 buf = open_buf(argv[1]);
 puts(buf);
 view10(buf);
}

