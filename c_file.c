#include <stdio.h>
#include "h_file.h"

int (*func)() = &first;

int main() {
 first();
 second();
 third();
}

int first() { return 0;}

char* second2() {
 return 0;
}

char* second() {
 return 0;
}

long 
/* commentary */
third
/* commentary */
() 
/* commentary */
{
 return 0;
}

int fourth(int (*func)(), int i) {
 func();
 return 0;
} 

/*
((int) (*func)()) fifth(char c) {
int (*func)() = &first;
 return func;
} 
*/