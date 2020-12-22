#include <stdio.h>
//#include <unistd.h>
#include <termios.h>

int
main() {
 int x = 0, y = 0;
 get_pos(&y, &x);
 printf("x:%d, y:%d\n", x, y);
 return 0;
}

int
get_pos(int *y, int *x) {

 *y = 0; *x = 0;

 char buf[30]={0};
 int ret, i, pow;
 char ch;

 struct termios term, restore;

 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag &= ~(ICANON|ECHO);
 tcsetattr(0, TCSANOW, &term);

 write(1, "\033[6n", 4);

 for( i = 0, ch = 0; ch != 'R'; i++ )
 {
	ret = read(0, &ch, 1);
	if ( ret != 1 ) {
	   fprintf(stderr, "getpos: error reading response!\n");
	   return 1;
	}
	buf[i] = ch;
	printf("buf[%d]: \t%c \t%d\n", i, ch, ch);
 }

 if (i < 2) {
	printf("i < 2\n");
	return(1);
	}

	for( i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
	   *x = *x + ( buf[i] - '0' ) * pow;

	for( i-- , pow = 1; buf[i] != '['; i--, pow *= 10)
	   *y = *y + ( buf[i] - '0' ) * pow;
 
 tcsetattr(0, TCSANOW, &restore);
 return 0;
}


#if 0
		printf("*x:%d, buf[%d]: %d\n", *x, i, buf[i]);
int
getpos(int *y, int *x) {
 int i = 0;
 int fd;
 int ret, retval;
 char buf[30] = {0};
 char ch;
 int pow;

 *x = 0;
 *y = 0;

 setnocanon();
 setnoecho();

 ret = fcntl(0, F_SETFL, O_NOCTTY);
 //ret = fcntl(1, F_SETFL, O_NDELAY);

 ret = write(1, "\033[6n", 4);
// sleep(4);
 //setecho();
 /*
 fflush(stdin);
 fflush(stdout);
 */

 for( i = 0, ch = 0; ch != 'R' /*&& ret == 1 || i < 10*/; i++ ) {
	ret = read(0, &ch, 1);
	if ( ret != 1 ) {
	   fprintf(stderr, "getpos: error reading response!\n");
/* appeared */
	   return 1;
	}
	//printf("buf[%d]: %c\n", i, ch);
	buf[i] = ch;
 }
 /* 
 printf("%s\n", buf+1);
 printf("i: %d\n", i);
 */
 //exit(1);
 if(i < 2) {
	printf("i < 2\n");
	exit(1);
	}


	for( i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
	   *x = *x + ( buf[i] - '0' ) * pow;

	for( i-- , pow = 1; buf[i] != '['; i--, pow *= 10)
	   *y = *y + ( buf[i] - '0' ) * pow;
 
 //setcanon();
   return 0;
}
#endif