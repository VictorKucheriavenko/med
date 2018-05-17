#include <stdio.h>
#include <termios.h>

int setnocanon();
int setnoecho();
int setcanon();
int setecho();

int
main() {
 int i;
 char c;
struct termios restore, term;
tcgetattr( 0, &restore );
tcgetattr( 0, &term );
term.c_lflag &= ~(ISIG | ICANON | ECHO);
term.c_iflag &= ~(IXON);
tcsetattr( 0, TCSANOW, &term);

 while( c != 030 )
 {
  c = getchar();
  printf("%1$d\t%1$o\t%1$x\n", c, 3);
// printf("%d\n", c);
 }
tcsetattr( 0, TCSANOW, &restore);
}
