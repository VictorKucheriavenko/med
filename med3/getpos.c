#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <term.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#define move(y, x) ( printf("\033[%d;%dH", (y), (x)) )
#define SIZE 100
#define get_pos() ( printf("\033[6n") )
#define clear_all() ( printf("\033[2J") );

int setnocanon();
int setcanon();
int getpos(int *y, int *x);


int main() {
 char buf[BUFSIZ];
 setvbuf(stdout, buf, _IONBF, BUFSIZ);
 setvbuf(stdin,  buf, _IONBF, BUFSIZ);
int res;
setnocanon();
setnoecho();
 int y, x;
 move(3, 33);
 clear_all();
 res = getpos(&y, &x);
 printf("main: res:%d, x: %d, y: %d\n", res, x, y);
 getchar();
 setecho();
 exit(1);
 move(3, 33);
 clear_all();
 res = getpos(&y, &x);
 printf("main: res:%d, x: %d, y: %d\n", res, x, y);
 getchar();
 move(23, 15);
 clear_all();
 res = getpos(&y, &x);
 printf("main: res:%d, x: %d, y: %d\n", res, x, y);
 move(23, 15);
 clear_all();
 res = getpos(&y, &x);
 printf("main: res:%d, x: %d, y: %d\n", res, x, y);
 return 0;
}
