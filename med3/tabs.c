#include <stdio.h>
#include <term.h>
#define clear_all() ( printf("\033[2J") );
#define move(y, x) ( printf("\033[%d;%dH", (y), (x)) )

int
set_tabs(int ncols, int tabs)
{
 printf("\033[3g"); /*** remove tab stops ***/

 for( i = 0, j = 0; i < ncols; i++) {
    if( j == tabs ) {
       j = 0;
       printf("\033H"); /*** place tab ****/
    }
    else j++;
    printf("\033[C");
 return 0;
}

int main () 
{
 int i, j;

 printf("\033[;H");/* move to 1, 1 */
 printf("\033[3g"); /* remove tab stops */

 for( j = 0; j < 6; j++) {
    for( i = 0; i < 9; i++)
       printf("\033[C");
    printf("\033H"); /* place tab */
 }
 move(1,1);
 clear_all();
 for( i = 0; i < 6; i++)
    printf("\t%d", i);

 return 0;
}
