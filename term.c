#define ON 1
#define OFF 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termcap.h>
#include <termios.h>

#define \
priv(var, f)\
 fprintf(stderr, #var":%"#f"\n", (var));\

#define moveyx(y, x) ( printf("\033[%d;%dH", (y), (x)) )
#define tc_savecur() ( printf("\0337"))
#define tc_restcur() ( printf("\0338"))
#define clear_all() ( printf("\033[2J") );

int
set_raw_mode(int act)
{
 static struct termios term, restore;
 
 if (act == ON) {
	 tcgetattr(0, &term);
	 tcgetattr(0, &restore);
	 term.c_iflag &= ~IXON;
	 term.c_lflag &= ~(ICANON|ECHO);
	 term.c_lflag |= ISIG;
	// term.c_iflag &= ~(INLCR|IGNCR|ICRNL);
	 tcsetattr(0, TCSANOW, &term);
 }
 if (act == OFF)
 	tcsetattr(0, TCSANOW, &restore);
 
 return 0;
 
}


int
slp(int n)
{
/*
 sleep(n);
*/
 return 0;
}

int
getch()
{
 getchar();
 return 0;
}


int
main()
{
/*
 int n = 10;
 int *p = &n;
 printf("pointer: %n\n", p);
 return 0;
*/
 
 set_raw_mode(ON);
 int ok;
 char buf[2048] = {0};
 char area[2048] = {0};
 char *pch;  
 ok = tgetent(buf, getenv("TERM"));
/*
 printf("ok: %d, %s\n", ok, buf);
*/
	clear_all();
	moveyx(10, 4);
	tc_savecur();
	printf("cursor_position");
	slp(1);
	getch();	
	
	moveyx(10, 30);
	tc_savecur();
	printf("cursor_position");
	slp(1);
	getch();	
	
	moveyx(10, 50);
	tc_savecur();
	printf("cursor_position");
	slp(1);
	getch();	
	
	tc_restcur();
	printf("-1-");
	slp(1);
	getch();	
	
	tc_restcur();
	printf("-2-");
	slp(1);
	getch();	
	
	tc_restcur();
	printf("-3-");
	slp(1);
	getch();	
	
	getch();	
	putchar('\n');
 	set_raw_mode(OFF);
	return 0;
 
 while(1) 
 {
/*
	fgets(buf, 2048, stdin); 
	buf[2] = 0;
	pch = tgetstr(buf, NULL);
	puts(pch ? (pch[0] == 27) ? pch+0 : pch : "nul");
//	free(pch);
	gets(buf);
	printf("%s", buf);
*/
 }
 
 return 0;
}
