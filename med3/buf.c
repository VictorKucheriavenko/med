#include <stdio.h>

int main() {
 int i;
 char buf[BUFSIZ];

 setnocanon();
 setnoecho();

 setvbuf(stdout, buf, _IONBF, BUFSIZ);
 setvbuf(stdin,  buf, _IONBF, BUFSIZ);
 //setbuf(stdout, NULL);

 for( i = 0; i < 1; i++) {
  putchar('a');
 printf("dsalasfasdff");
 printf("%d", BUFSIZ);
 }
 getchar();
 

 setcanon();
 setecho();
 return 0;
 }
