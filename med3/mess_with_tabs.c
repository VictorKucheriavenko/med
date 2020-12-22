#include <stdio.h>

int
main ()
{
 printf("\033[;H");/* move to 1, 1 */
 printf("\033[3g"); /* remove tab stops */

 printf("\033[1;3H");/* move to 1, 1 */
 printf("\033H"); /* place tab */

 printf("\033[1;5H");/* move to 1, 1 */
 printf("\033H"); /* place tab */

 printf("\033[1;10H");/* move to 1, 1 */
 printf("\033H"); /* place tab */

 printf("\033[1;50H");/* move to 1, 1 */
 printf("\033H"); /* place tab */
}
