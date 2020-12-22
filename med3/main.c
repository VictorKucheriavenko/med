#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "med.h"
#include "term.h"
#include "color.h"

char* open_buf(char* path);
line_t* read_lines(char* path);
int read_buffer(char* path);
int set_tabs(int ncols, int tabs);


int main(int argn, char *argv[])
{

 if( argn != 2 )
    return 0;

 int i, fd;
 char ch;
 char buf[500];
 struct termios term, restore;

 x_offs = 0;

 setvbuf(stdout, NULL, _IONBF, 0);
 setvbuf(stdin,  NULL, _IONBF, 0);

 fd = read_buffer(argv[1]);
 cur_line = lines_head->next;
 printf("lines_head: %p\n", lines_head);
 //print_lines(lines_n);




 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag &= ~(ICANON | ECHO);
 tcsetattr(0, TCSANOW, &term);


 set_tabs(NCOLS, TABS);
 printf("\033[;H");
 clear_all();
 printf("\033[13;H");
 print_lines(1, DWN - CENT);
/*
 for(i = 0; i < lines_n; i++){
  //  printf("%d:", i);
    printf("%s\n", (lines+i)->str);
    }
*/




 printf("\033[13;H");

 while( 1 ) {
    ch = getchar();

    switch( ch ) {
       case K_UP:
	  move_up(1);
	  break;
       case K_UP1:
	  move_up(4);
	  break;
       case K_UP2:
	  move_up(16);
	  break;
       case K_DOWN:
	  move_down(1);
	  break;
       case K_DOWN1:
	  move_down(4);
	  break;
       case K_DOWN2:
	  move_down(16);
	  break;
       case K_LEFT:
	  move_left(1);
	  break;
       case K_LEFT1:
	  move_left(2);
	  break;
       case K_LEFT2:
	  move_left(8);
	  break;
       case K_RIGHT:
	  move_right(1);
	  break;
       case K_RIGHT1:
	  move_right(2);
	  break;
       case K_RIGHT2:
	  move_right(8);
	  break;
       case K_DELLINE:
          del_line();
//	  x_offs = 0;
	  break;
       case K_WRITE: 
          write_buf(fd);
	  break;
       case K_QUIT:
	  clear_all();
	  close(fd);
          goto exit;
	  break;
       case K_DEL:
	  //del_char_right();
	  del_char();
	  break;
       case K_INS_MOD:
	  insert_string();
	  break;
       case 'y':
	  move(7,7);
	  break;
       default:
	  break;
    }
 }


 exit:
 puts("exiting\n");
 tcsetattr(0, TCSANOW, &restore);
 return 0;
}
