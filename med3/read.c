#include <stdio.h>
#include <unistd.h>
#include <term.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

int main() {
int i;
   char buf[100];
   for(i = 0; i<10; i++)
      read(0, buf, 1);
   return 0;
}
