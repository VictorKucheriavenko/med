#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM 20

int main () {
 int i;
 int *arr;
 arr = malloc(NUM * sizeof(int));
 for( i = 0; i < NUM; i++ )
    arr[i] = 5;
 memset(arr, 0, sizeof(int)*NUM+20);
 for( i = 0; i < NUM; i++ )
    printf("arr[%d]: %d\n", i, arr[i]);
 return 0;
}
