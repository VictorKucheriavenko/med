/********************************
char* open_buf(char* path)
int view10( char* buf ) 
********************************/

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

char* 
open_buf(char* path)
{
 printf("path: %s\n", path);
 int fd;
 off_t len;
 char* buf;
 fd = open(path, O_RDWR);
 perror("open");
 len = lseek(fd, 0, SEEK_END);
 lseek(fd, 0, SEEK_SET);
 buf = malloc(len + 1);
 perror("malloc");
 *(buf + len) = '\0';
 read(fd, buf, len); 
 perror("read");
 return buf;
}

int 
view10( char* buf ) 
{
 int i;
 for( i = 0; buf[i] != 0; i++)
    switch(buf[i]) {
       case '\t':
          printf("\\t ");
	  break;
       case '\n':
          printf("\\n\n");
	  break;
       case 32:
          printf("\\s ");
	  break;
       default:
	  printf("%c ", buf[i]);
	  break;
    }
 printf("\n");
 return 0;
}
