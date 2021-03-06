#include <stdio.h>
#include <termios.h>
#include <ctype.h>
#include <string.h>

#define MAX 200

int setnocanon();
int setnoecho();
int setcanon();
int setecho();
int charinput_test();

int
charinput_test() {
  int i;
  char c;
 struct termios restore, term;
/*
 tcgetattr( 0, &restore );
 tcgetattr( 0, &term );
 term.c_lflag &= ~(ISIG | ICANON | ECHO);
 term.c_iflag &= ~(IXON);
 tcsetattr( 0, TCSANOW, &term);
*/
 
  while( c != 030 )
  {
   c = getchar();
   printf("%1$d\t%1$o\t%1$x\n", c, 3);
 // printf("%d\n", c);
  }
// tcsetattr( 0, TCSANOW, &restore);
}

int
main() {
 int i;
 int buf_len;
 int flag = 0;
 char c = 'a';
 char buf[MAX];
struct termios restore, term;
tcgetattr( 0, &restore );
tcgetattr( 0, &term );
term.c_lflag &= ~ICANON;
/*
cfmakeraw(&term);
term.c_lflag |= (ECHO|ISIG|IEXTEN|ICANON);
term.c_cflag |= (CSIZE|PARENB);
term.c_iflag &= ~IGNCR;
*/
tcsetattr( 0, TCSANOW, &term);
printf("%x\n", term.c_lflag);

 while( c != 27 )
 {
	 gets(buf);
	 buf_len = strlen(buf);
	 for( i = 0; i < buf_len; i++ )
	    buf[i] = toupper( buf[i] );

	 if (!strcmp("IGNBRK", buf)) {
		flag = IGNBRK;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }
	 if (!strcmp("CHARINPUT", buf))
		charinput_test();

	 if (!strcmp("BRKINT", buf)) {
		flag = BRKINT;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IGNPAR", buf)) {
		flag = IGNPAR;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("PARMRK", buf)) {
		flag = PARMRK;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("INPCK", buf)) {
		flag = INPCK;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ISTRIP", buf)) {
		flag = ISTRIP;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("INLCR", buf)) {
		flag = INLCR;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IGNCR", buf)) {
		flag = IGNCR;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ICRNL", buf)) {
		flag = ICRNL;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IUCLC", buf)) {
		flag = IUCLC;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IXON", buf)) {
		flag = IXON;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IXANY", buf)) {
		flag = IXANY;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IXOFF", buf)) {
		flag = IXOFF;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IMAXBEL", buf)) {
		flag = IMAXBEL;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IUTF8", buf)) {
		flag = IUTF8;
 	 	if( (term.c_iflag | flag) == term.c_iflag) {
    	     	    term.c_iflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_iflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("OPOST", buf)) {
		flag = OPOST;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("OLCUC", buf)) {
		flag = OLCUC;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ONLCR", buf)) {
		flag = ONLCR;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("OCRNL", buf)) {
		flag = OCRNL;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ONOCR", buf)) {
		flag = ONOCR;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ONLRET", buf)) {
		flag = ONLRET;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("OFILL", buf)) {
		flag = OFILL;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("OFDEL", buf)) {
		flag = OFDEL;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("NLDLY", buf)) {
		flag = NLDLY;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CRDLY", buf)) {
		flag = CRDLY;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("TABDLY", buf)) {
		flag = TABDLY;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("BSDLY", buf)) {
		flag = BSDLY;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("VTDLY", buf)) {
		flag = VTDLY;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("FFDLY", buf)) {
		flag = FFDLY;
 	 	if( (term.c_oflag | flag) == term.c_oflag) {
    	     	    term.c_oflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_oflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CBAUD", buf)) {
		flag = CBAUD;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CBAUDEX", buf)) {
		flag = CBAUDEX;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CSIZE", buf)) {
		flag = CSIZE;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CSTOPB", buf)) {
		flag = CSTOPB;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CREAD", buf)) {
		flag = CREAD;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("PARENB", buf)) {
		flag = PARENB;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("PARODD", buf)) {
		flag = PARODD;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("HUPCL", buf)) {
		flag = HUPCL;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CLOCAL", buf)) {
		flag = CLOCAL;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

/*
	 if (!strcmp("LOBLK", buf)) {
		flag = LOBLK;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }
*/

	 if (!strcmp("CIBAUD", buf)) {
		flag = CIBAUD;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CMSPAR", buf)) {
		flag = CMSPAR;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("CRTSCTS", buf)) {
		flag = CRTSCTS;
 	 	if( (term.c_cflag | flag) == term.c_cflag) {
    	     	    term.c_cflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_cflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ISIG", buf)) {
		flag = ISIG;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ICANON", buf)) {
		flag = ICANON;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("XCASE", buf)) {
		flag = XCASE;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ECHO", buf)) {
		flag = ECHO;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ECHOE", buf)) {
		flag = ECHOE;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ECHOK", buf)) {
		flag = ECHOK;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ECHONL", buf)) {
		flag = ECHONL;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ECHOCTL", buf)) {
		flag = ECHOCTL;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ECHOPRT", buf)) {
		flag = ECHOPRT;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("ECHOKE", buf)) {
		flag = ECHOKE;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

/*
	 if (!strcmp("DEFECHO", buf)) {
		flag = DEFECHO;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }
*/

	 if (!strcmp("FLUSHO", buf)) {
		flag = FLUSHO;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("NOFLSH", buf)) {
		flag = NOFLSH;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("TOSTOP", buf)) {
		flag = TOSTOP;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("PENDIN", buf)) {
		flag = PENDIN;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 if (!strcmp("IEXTEN", buf)) {
		flag = IEXTEN;
 	 	if( (term.c_lflag | flag) == term.c_lflag) {
    	     	    term.c_lflag &= ~flag;
	     	    printf("%s OFF\n", buf);
		}
	 	else {
	      	    term.c_lflag |= flag;
	     	    printf("%s ON\n", buf);
	 	}
	 }

	 tcsetattr(0, TCSANOW, &term);
 }
 /*

if(strcmp(flag, x)) {
if(term.c_yflag | x == term.c_yflag)
   term.c_yflag &= ~x;
else
   term.c_yflag |= x;
}

*/

tcsetattr( 0, TCSANOW, &restore);
}
