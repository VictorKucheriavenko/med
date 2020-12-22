#include <stdio.h>
#define BCSP 0x7f
#define DEL 0x08

int
getstring(char* buf, int nmax) {
 int i, res;
 char ch;

 setnoecho();


 i = 0;
 do {
    res = read(0, &ch, 1);

    switch (ch) {

       case BCSP:
	  ch = DEL;

       case DEL:
	  *(buf+i) = ch;
	  write(1, buf+i, 1);
	  i--;
	  break;

       case 27: case 10: case 0:
	  ch = 0;
	  *(buf+i) = 0;
	  break;

       default:
	  *(buf+i) = ch;
	  write(1, buf+i, 1);
	  i++;
    }

 } while ( ch != 0 );

 setecho();

 return 0;
}


int main() {
 int i;
 char buf[1000];
 setnocanon();
 getstring(buf, 1000, 0);
 printf("\n");
 printf("%s\n", buf);
 setcanon();
 return 0;
}
