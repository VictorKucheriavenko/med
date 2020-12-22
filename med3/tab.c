#include <stdio.h>
int
main() {
int i;

for( i = 0; i < 10; i++ );
printf("aaa\033[H");
return 0;
}
