#include <stdio.h>

int main() {
 int i;
 char buf[BUFSIZ];
 setvbuf(stdout, buf, _IOLBF, BUFSIZ);
 setvbuf(stdin,  buf, _IONBF, BUFSIZ);
printf("start ");
fprintf(stderr, "stderr:%s\n", buf);
sleep(1);
printf("stop\n");
}
