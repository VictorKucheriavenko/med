#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

int 
setnocanon()
{
 struct termios term;
 tcgetattr(0, &term);
 term.c_lflag &= ~ICANON;
 tcsetattr(0, TCSANOW, &term);
 return 0;
}


int 
setcanon()
{
 struct termios term;
 tcgetattr(0, &term);
 term.c_lflag |= ICANON;
 tcsetattr(0, TCSANOW, &term);
 return 0;
}


int 
setnoecho()
{
 struct termios term;
 tcgetattr(0, &term);
 term.c_lflag &= ~ECHO;
 tcsetattr(0, TCSANOW, &term);
 return 0;
}


int 
setecho()
{
 struct termios term;
 tcgetattr(0, &term);
 term.c_lflag |= ECHO;
 tcsetattr(0, TCSANOW, &term);
 return 0;
}

int
main()
{
 setnocanon();
 char inbuf[256]={0};
 char outbuf[256]={0};
 char buf[20] = "hello world";
 char c;
 int i;
 
 setvbuf(stdout, outbuf, _IONBF, 256);
 setvbuf(stdin,  inbuf, _IONBF, 256);
 
 for (i = 0; i < 10; i++)
	ungetc(buf[i], stdin);
	
 while (c != 27)
 {
	c = getchar();
	if (c == 13)
		printf("Ctrl-M\n");
	if (c == 10)
		printf("newline\n");
//	putchar(c);
 }
 printf("inbuf:%s\n", inbuf);
 printf("len:%d\n", strlen(inbuf));
 printf(":%s\n", outbuf);
 setcanon();
 return 0;
}