#define _GNU_SOURCE
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include "color.h"

#define INIT_NUM 1000
#define MAX_LINE 500
#define TABS 8
#define NCOLS 80
#define NROWS 25
#define DEBUG_BAR 24 
#define SHOW_BAR 25 


typedef struct line {
   char* str;
   int len;
   struct line *forw;
   struct line *backw;
} line_t;


 
/*
keys
*/

#define K_REDRAW 'R'
#define K_DEAD '\t'
#define K_KMOD 0x60 
#define K_UP 'k'
#define K_UP1 'K'
#define K_UP2 'i'
#define K_DOWN 'j'
#define K_DOWN1 'J'
#define K_DOWN2 'u'
#define K_RIGHT 'f'
#define K_RIGHT1 'v'
#define K_RIGHT2 'r'
#define K_GOWORD 'w'
#define K_GOWORD1 'q'
#define K_LEFT 'd'
#define K_LEFT1 'c'
#define K_LEFT2 'e'
#define K_DELLINE 'y'
#define K_MKLINE 'l'
#define K_DEL 'g'
#define K_DEL_WORD 'G'
#define K_CHANGE_WORD 'F'
#define K_INS_MOD 'b'
#define K_INS_MOD1 'B'
#define K_INS_SPACE 32
#define K_INS_CHAR 'p'
#define K_FIND1 '/'
#define K_FIND2 '?'
#define K_SRCH_RPL ';'
#define K_REFIND 'n'
#define K_WRITE 'W'
#define K_QUIT 'Q'
#define K_SET_COPY_START 'a'
#define K_SET_COPY_END 'z'
#define K_SET_MARK1 'A'
#define K_SET_MARK2 'Z'
#define K_GO_MARK 'm'
#define K_COPY 's'
#define K_PASTE 'x'
#define K_CONCAT 'C'
#define K_DEL_LINES 'o'
#define K_GO_LINE 'U'

#define ESC 27
#define BCSP 0x7f
#define DEL 0x08

#define TOP 2
#define BOT 24
#define CENT 13

#define moveyx(y, x) ( printf("\033[%d;%dH", (y), (x)) )
#define movex(x) ( printf("\033[;%dH", (x)) )
#define movey(y) ( printf("\033[%d;H", (y)) )
#define clreol() (printf("\033[K") )
#define CLR "\033[K"
#define CLL "\033[2K"
#define clear_all() ( printf("\033[2J") );
#define clear_down() ( printf("\033[0J") );


#define scroll_down() \
		( printf("\033[%d;H\033M\033[%d;H", (_wintop), (_wincur) ) )

#define scroll_up() \
		( printf("\033[%d;H\033D\033[%d;H", (_winbot), (_wincur) ) )

#define  set_scroll_win(top, bot) \
	( printf("\033[%d;%dr", (top), (bot) ) )
/*
#define scroll_down(top, base) \
	( printf("\033[%d;H\033M\033[%d;H", (top), (base) ) )

#define scroll_up(bot, base) \
		( printf("\033[%d;H\033D\033[%d;H", (bot), (base) ) )
*/

/*
#define scroll_down() ( printf("\033[1;H\033M\033[13;H") )
#define scroll_up() ( printf("\033[25;H\033D\033[13;H") )
*/

#define get_pos() ( printf("\033[6n") )

/*** function declarations ***/
int reset();
int save_cur();
int rest_cur();
int getsize ();
int setnocanon();
int setcanon();
int setnoecho();
int setecho();
int getpos(int *y, int *x);
int set_tabs(int ncols, int tabs);
int set_tabs1(int ncols, int tabs);
int del (char **str, int o, int n);
int del_char();
int del_word();
//int change_word();
int del_junk();
int del_char_right();
char* insert (char **pstr1, int n, char *str2);
int getstring(char* buf, int nmax);
int getstring1();
int calculate_all_lengths();
line_t* make_line_after(line_t *prev);
line_t* make_line_before(line_t *next);
int destroy_line(line_t *line);
int make_chain();
int make_line();
int insert_string();
int insert_string_after();
int insert_string_after2();
int insert_block();
int insert_space();
int insert_char(char ch);
int insert_char_next(char ch);
int read_line(FILE* file, char* buf);
int read_buffer(FILE* file, line_t **head, line_t **tail);
int message(char* s);
int write_buf(int fd);
int work_buffer();
int move_down(int n);
int move_up(int n);
int move_first();
int move_last();
int move_left(int n);
int move_right(int n);
int move_first_col();
int move_last_col();
//int move_word();
int get_bow();
int get_eow();
int move_bow();
int move_eow();
int move_tab_forw(int pos);
int move_tab_backw(int pos);
int print_line(char* str);
int del_line();
int output_line(line_t* l);
int print_lines(int s, int n);
int print_cur();
char* find_str();
int find_beginning();
int search_and_replace();
int redraw();
int concat_lines();
int copy_lines(line_t *start, line_t *end, 
     line_t **buf_head, line_t **buf_tail);
int insert_lines(line_t *place, line_t *buf_head, line_t *buf_tail);
int set_copy_start_pos();
int set_copy_end_pos();
int set_mark1();
int set_mark2();
int cmd_yank();
int cmd_paste();
int cmd_del_line();
int free_lines(line_t *l, line_t *m);
int tabs_perform_check();
line_t* goto_line(int n);
line_t* get_head();
line_t* get_tail();
int show_bar();
int debug_bar();
int cur_line_number();
void sig_handler(int i);
void sigsegv_handler(int i);
void exit_handler();
int get_next_tabstop();
int print_top_line();
int print_bottom_line();
int cmd_del_lines();
int cmd_goto_line();
int go_mark(line_t *mark);
int init_line( line_t *ln );

/***** global variables *****/
//line_t* lines;
line_t* lines_head = NULL;
line_t* lines_tail = NULL;
line_t* cur_line;
line_t* search_pos;

line_t *copy_buffer_head = NULL;
line_t *copy_buffer_tail = NULL;
line_t *copy_start_pos; //from...
line_t *copy_end_pos;
line_t *mark1;
line_t *mark2;

int _key_mode = 0;
int _switch_key_mode = 0;
int _offs;
int _x_curs;
int _cols, _rows;
int _wintop, _winbot, _wincur;
int *_tabstops;
int _x_cur, _y_cur;
char _filename[MAX_LINE];

char search_buf[MAX_LINE];


char *str_global;

int
main( int argn, char *argv[])
{
 int i;
 int readonly = 0;

 if( argn != 2 && argn !=3 )
    return 0;

 _offs = 0;
 reset();

 setvbuf(stdout, NULL, _IONBF, 0);
 setvbuf(stdin,  NULL, _IONBF, 0);

 int fd;
 char ch;
 FILE* file;
 int file_len;
 char buf[MAX_LINE];

 copy_buffer_head = NULL;
 copy_buffer_tail = NULL;

 signal (SIGABRT, &sig_handler);
 signal (SIGTERM, &sig_handler);
 signal (SIGSEGV, &sigsegv_handler);
 atexit( &exit_handler );


 if(argn == 3 && !strcmp(argv[1], "-r")) {
    fd = open(argv[2], O_RDONLY);
    readonly = 1;
    if ( fd < 0 ) {
       perror("open");
       return 1;
    }
    strcpy(_filename, argv[2]);
    file = fdopen(fd, "r");
 }
 else {
    fd = open(argv[1], O_RDWR);
    if ( fd < 0 ) {
       perror("open");
       return 1;
    }
    strcpy(_filename, argv[1]);
    file = fdopen(fd, "r+");
 }


 file_len = lseek(fd, 0, SEEK_END);
 lseek(fd, 0, SEEK_SET);

 if( file_len )
    read_buffer(file, &lines_head, &lines_tail);
 cur_line = lines_head;
   
 setnocanon();
 setnoecho();


 getsize();

 _wintop = 1 + 1;
 _winbot = _rows - 2;
 _wincur = ( _winbot - _wintop ) / 2 + _wintop;

 set_scroll_win(_wintop, _winbot);

 set_tabs1(_cols, TABS);
// tabs_perform_check();
 clear_all();
 moveyx(1,1);

 moveyx(_wincur, 1);
 print_cur();

 printf("\033[%d;H", _wincur);

 while( 1 ) {
loop:
    ch = getchar();

/*
switch
*/
if ( !cur_line )
    switch( ch ) {
       case K_REDRAW:
	  redraw();
	  break;
       case K_KMOD:
	  save_cur();
	  _key_mode = (_key_mode) ? 0 : 1;
	  moveyx(_winbot+1, 1);
	  printf("_key_mode: %d", _key_mode);
	  rest_cur();
	  break;
       case K_DEAD:
	  if( _switch_key_mode != 1) {
	  _key_mode = (_key_mode) ? 0 : 1;
	  _switch_key_mode = 1;
	  }
	  goto loop;
       case K_MKLINE:
	  if (_key_mode == 1) {
	     cur_line = make_line_before(cur_line);
	     init_line(cur_line);
	  }
	  else {
	     cur_line = make_line_after(cur_line);
	     init_line(cur_line);
	  }
	  redraw();
          //make_line();
	  reset();
	  break;
       case K_WRITE: 
	  if(readonly == 0)
            write_buf(fd);
	  break;
       case K_QUIT:
	  clear_all();
	  close(fd);
          goto exit;
	  break;
       case K_INS_MOD1:
	  insert_block();
	  break;
       case K_PASTE:
	  cmd_paste();
          break;
//swend
}
else
    switch( ch ) {
       case K_REDRAW:
	  redraw();
	  break;
       case K_KMOD:
	  save_cur();
	  _key_mode = (_key_mode) ? 0 : 1;
	  moveyx(_winbot+1, 1);
	  printf("_key_mode: %d", _key_mode);
	  rest_cur();
	  break;
       case K_DEAD:
	  if( _switch_key_mode != 1) {
	  _key_mode = (_key_mode) ? 0 : 1;
	  _switch_key_mode = 1;
	  }
	  goto loop;
       case K_UP:
	  move_up(1);
	  break;
       case K_UP1:
	  move_up(25);
	  break;
       case K_UP2:
	  move_first();
	  break;
       case K_DOWN:
	  move_down(1);
	  break;
       case K_DOWN1:
	  move_down(25);
	  break;
       case K_DOWN2:
	  move_last();
	  break;
       case K_LEFT:
	  move_left(1);
	  break;
       case K_LEFT1:
	  move_left(10);
	  break;
       case K_LEFT2:
	  move_first_col();
	  break;
       case K_RIGHT:
	  move_right(1);
	  break;
       case K_RIGHT1:
	  move_right(10);
	  break;
       case K_RIGHT2:
	  move_last_col();
	  break;
       case K_GOWORD:
	  if(!_key_mode)
	    move_bow();
	  if(_key_mode)
	    move_eow();
	  break;
       case K_GOWORD1:
	  if(_key_mode)
	    move_bow();
	  if(!_key_mode)
	    move_eow();
	  break;
       case K_DELLINE:
          del_line();
	  break;
       case K_MKLINE:
	  if (_key_mode == 1) {
	     cur_line = make_line_before(cur_line);
	     init_line(cur_line);
	  }
	  else {
	     cur_line = make_line_after(cur_line);
	     init_line(cur_line);
	  }
	  redraw();
          //make_line();
	  reset();
	  break;
       case K_FIND1: 
	  moveyx(_rows-1, 1);
	  printf(CLL);
	  getstring(search_buf, MAX_LINE);
          //moveyx(_wincur, 1);
	  if(_key_mode == 0)
            find_str();
	  else
	  if(_key_mode == 1)
       	    find_beginning();
	  break;
       case K_FIND2: 
	  moveyx(_rows-1, 1);
	  printf(CLL);
	  getstring(search_buf, MAX_LINE);
	  //moveyx(_wincur, 1);
          find_beginning();
	  break;
       case K_REFIND: 
	  if(search_pos)
	  cur_line = cur_line->forw;
	  //search_pos = search_pos->forw;
	  if(_key_mode == 0)
            find_str();
	  else
	  if(_key_mode == 1)
	    find_beginning();
	  break;
       case K_SRCH_RPL: 
          search_and_replace();
	  break;
       case K_WRITE: 
	  if(readonly == 0)
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
       case K_DEL_WORD:
	  del_word();
	  break;
       case K_CHANGE_WORD:
	  del_word();
	  insert_string();
	  break;
       case K_INS_MOD:
          if( _key_mode == 0 )
       	  insert_string();
          if( _key_mode == 1 )
       	  insert_string_after();
	  break;
       case K_INS_MOD1:
	  insert_block();
	  break;
       case K_INS_SPACE:
          insert_space();
          break;
	  char c;
       case K_INS_CHAR:
	  c = getchar();
	  if(!_key_mode)
            insert_char(c);
	  else
	    insert_char_next(c);
          break;
       case K_SET_MARK1:
	  set_mark1();
	  break;
       case K_SET_MARK2:
	  set_mark2();
	  break;
line_t *tmp;
       case K_GO_MARK:
tmp = mark1;
mark1 = mark2;
mark2 = tmp;
	  go_mark(mark1);
	  break;
       case K_SET_COPY_START:
	  set_copy_start_pos();
          break;
       case K_SET_COPY_END:
	  set_copy_end_pos();
          break;
       case K_COPY:
	  cmd_yank();
          break;
       case K_PASTE:
	  cmd_paste();
          break;
       case K_DEL_LINES:
	  cmd_del_lines();
          break;
      case K_CONCAT:
	  concat_lines();
	  break;
       case K_GO_LINE:
	  cmd_goto_line();
          break;
       default:
	  break;
    }
    debug_bar();
    show_bar();
//gmessage("debug");
    if(_switch_key_mode == 1) {
	  _key_mode = (_key_mode) ? 0 : 1;
	  _switch_key_mode = 0;
    }
 }


 exit:
 puts("exiting\n");
 setcanon();
 setecho();
 return 0;
}


int
save_cur() {
 getpos(&_y_cur, &_x_cur);
 return 0;
}

int
rest_cur() {
 moveyx(_y_cur, _x_cur);
}

int getsize () {
   struct winsize ws;
   ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
   _rows = ws.ws_row;
   _cols = ws.ws_col;
   //printf ("Rows: %d, Cols: %d\n", ws.ws_row, ws.ws_col);
   printf ("Rows: %d, Cols: %d\n", _rows, _cols);
   //getchar();
   return 0;
}


int 
setnocanon()
{
 struct termios term;
 tcgetattr(0, &term);
 term.c_lflag &= ~ICANON;
 tcsetattr(0, TCSANOW, &term);
 return 0;
}

int 
setcanon()
{
 struct termios term;
 tcgetattr(0, &term);
 term.c_lflag |= ICANON;
 tcsetattr(0, TCSANOW, &term);
 return 0;
}

int 
setnoecho()
{
 struct termios term;
 tcgetattr(0, &term);
 term.c_lflag &= ~ECHO;
 tcsetattr(0, TCSANOW, &term);
 return 0;
}

int 
setecho()
{
 struct termios term;
 tcgetattr(0, &term);
 term.c_lflag |= ECHO;
 tcsetattr(0, TCSANOW, &term);
 return 0;
}


int
getpos(int *y, int *x) {
 int i = 0;
 int fd;
 int res, retval;
 char buf[30] = {0};
 memset( buf, 0, 30);
 char ch;
 int pow;

 *x = 0;
 *y = 0;

 setnocanon();
 setnoecho();

 res = fcntl(0, F_SETFL, O_NOCTTY);
 //res = fcntl(1, F_SETFL, O_NDELAY);

 res = write(1, "\033[6n", 4);
// sleep(4);
 //setecho();
 /*
 fflush(stdin);
 fflush(stdout);
 */

 for( i = 0, ch = 0; ch != 'R' /*&& res == 1 || i < 10*/; i++ ) {
    res = read(0, &ch, 1);
    if ( res != 1 ) {
       fprintf(stderr, "getpos: error reading response!\n");
       return 1;
    }
    //printf("buf[%d]: %c\n", i, ch);
    buf[i] = ch;
 }
 /* 
 printf("%s\n", buf+1);
 printf("i: %d\n", i);
 */
 //exit(1);
 if(i < 2) {
    printf("i < 2\n");
    exit(1);
    }


    for( i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
       *x = *x + ( buf[i] - '0' ) * pow;

    for( i-- , pow = 1; buf[i] != '['; i--, pow *= 10)
       *y = *y + ( buf[i] - '0' ) * pow;
 
 //setcanon();
   return 0;
}

int
set_tabs(int ncols, int tabs)
{
 int i, j;

 printf("\033[;H");/* move to 1, 1 */
 printf("\033[3g"); /* remove tab stops */

 for( i = 0, j = 0; i < ncols; i++) {
    if( j == tabs ) {
       j = 0;
       printf("\033H"); /* place tab */
    }
    else j++;
    printf("\033[C");
 }
 return 0;
}

int
set_tabs1(int ncols, int tabs)
{
 int i, j;
 int k = 0;

 _tabstops = calloc(_cols, sizeof(int));
 //memset( _tabstops, 0, _cols * sizeof(int));

 printf("\033[;H");/* move to 1, 1 */
 for(i = 0; i < 8; i++)
    printf("1234567890");

 printf("\033[;H");/* move to 1, 1 */
 printf("\033[3g"); /* remove tab stops */
	  //sleep(3);
 printf("\033H"); /* place tab */
 getpos(&_y_cur, &_x_cur);
 _tabstops[k++] = _x_cur;

	  //usleep(300000);
    printf("\033[C");

 for( i = 0, j = 0; i < ncols - 1; i++) {
    if( j == tabs - 1) {
       j = 0;
	  //sleep(3);
       printf("\033H"); /* place tab */
       getpos(&_y_cur, &_x_cur);
       _tabstops[k++] = _x_cur;
    }
    else j++;
	  //usleep(300000);
    printf("\033[C");
 }
 for( i = 0; *(_tabstops+i); i++);
 //_tabstops = realloc(_tabstops, (i + 1) * sizeof(int));
    
 printf("\033[;H");/* move to 1, 1 */
 //getchar();
 //exit(1);
 return 0;
}


int
tabs_perform_check()
{
 int i;
 int k = 0;
 moveyx(1,1);
 getpos(&_y_cur, &_x_cur);
 _tabstops[k++] = _x_cur;
 for( i = 0; _x_cur < _cols; i++) {
    putchar('\t');
//    getchar();
    getpos(&_y_cur, &_x_cur);
    _tabstops[k++] = _x_cur;
 }
 _tabstops[k++] = 0;
}



int
del (char **pstr, int o, int n)
{
 char *buf;
 int i;
 int len = strlen(*pstr);
 buf = calloc( len + 1, 1 );

 if( o <= 0 || n < 0 )
    return 0;

 if( o > len ) {
    strcpy(buf, *pstr);
 } else
 if( o + n > len ) {
    strncpy(buf, *pstr, o - 1);
    buf[o-1] = 0;
 } 
 else {
    strncpy(buf, *pstr, o - 1);
    buf[o-1] = 0;
    strcat(buf, *pstr + o - 1 + n );
 }
free(*pstr);
*pstr = malloc(strlen(buf)+1);
strcpy(*pstr, buf);
free(buf);
return 0;
}


int
del_char()
{
 if( !cur_line )
   return 0;

 if( cur_line->len == 0 )
    del_line();
 del(&cur_line->str, _offs+1, 1);
 save_cur();
 printf( CLR"%s", cur_line->str + _offs);
 rest_cur();
 cur_line->len = strlen(cur_line->str);
 if(_offs == cur_line->len)
    move_left(1);
 return 0;
}

int
del_char_right()
{
 char buf[MAX_LINE];
 int len = strlen( cur_line->str );
//*(cur_line->
 memmove(cur_line->str + _offs, cur_line->str + _offs + 1, len + 1);
 printf( CLR"%s", cur_line->str + _offs);
 moveyx(13, _offs+1);
 //printf(KGRN"%d\n"KNRM, _offs );
 return 0;
}


char*
insert (char **pstr1, int n, char *str2)
{
 char *buf;
 int i;

 if ( n > strlen(*pstr1) ) return NULL;

 buf = malloc( strlen(*pstr1) + strlen(str2) + 1 );
 strncpy(buf, *pstr1, n);
 buf[n] = '\0';
 strcat(buf, str2);
 strcat (buf, *pstr1+n);
 *pstr1 = realloc( *pstr1, strlen(buf) + 1 );
 strcpy( *pstr1, buf );
 return buf;
}

int
getstring(char* buf, int nmax) {
 int i, res, ret;
 char ch;

 setnocanon();
 setnoecho();

 i = 0;
 do {
    res = read(0, &ch, 1);

    switch (ch) {
       case BCSP:
	  ch = DEL;

       case DEL:
	  if( i ) {
  	    *(buf+i) = ch;
	    write(1, buf+i, 1);
	    i--;
	  }
	  break;

       case 10: case 0:
       case 27:
	  ret = ch;
	  ch = 0;
	  *(buf+i) = 0;
	  break;

       default:
	  *(buf+i) = ch;
	  write(1, buf+i, 1);
	  i++;
    }

 } while ( ch != 0 );
 return ret; 
}

int
insert_string_after()
{
 int ret;
 save_cur();
 moveyx(_y_cur, _x_cur + 1);
 _offs++;
 ret = insert_string();
 return ret;
}

int
insert_string()
{
 int res, ret;
 char ch;
 char ch2[2] = {0};
 char *tmp;

 setnocanon();
 setnoecho();

 do {
    res = read(0, &ch, 1);

    switch (ch) {
       case BCSP:
	  ch = DEL;

       case DEL:
	  if( _offs ) {
	    write(1, &ch, 1);
            _offs--;
	    del(&cur_line->str, _offs + 1, 1);
	  }
	  break;

       case 0:
       case 27:
	  ret = ch;
	  ch = 0;
	  break;

      case 10:
	  cur_line->len = strlen(cur_line->str);
	  tmp = cur_line->str + _offs;
          cur_line = make_line_after(cur_line);
  	  cur_line->str = malloc( strlen(tmp) + 1 );
	  strcpy(cur_line->str, tmp);
	  cur_line->backw->str =
	    realloc(cur_line->backw->str, cur_line->backw->len - strlen(tmp)+1);
	    cur_line->backw->str[cur_line->backw->len - strlen(tmp)] = 0;
	  _offs = 0;
	  moveyx(_wincur, 1);

	  redraw();
	  break;


       default:
	  ch2[0] = ch;
	  insert(&cur_line->str, _offs, ch2);
	  _offs++;
    }
    save_cur();
    moveyx(_y_cur, 1);
    printf(CLL"%s", cur_line->str);
    moveyx(_y_cur, _offs + 1);
    debug_bar();

 } while ( ch != 0 );
 _offs--;
 moveyx(_y_cur, _offs + 1);
 cur_line->len = strlen( cur_line->str);
 return ret; 
}

int
concat_lines()
{
 if(!cur_line->forw)
   return 0;

 int i;
 char *buf;
 line_t *tmp;
 
 buf = malloc( strlen(cur_line->str) + strlen(cur_line->forw->str) + 1 );
 strcpy(buf, cur_line->str);
 strcat(buf, cur_line->forw->str);
 free(cur_line->str);
 cur_line->str = buf;
 
 destroy_line(cur_line->forw);
 
 redraw();
 calculate_all_lengths();
 
 return 0;
}

int
calculate_all_lengths()
{
 line_t *line;
 int len;
 int i = 0; 
 
 line = get_head();
 while(line) { 
	len = strlen(line->str);
	if(line->len != len)
		i++;
	line->len = strlen(line->str);
	line = line->forw;
 }
 return i;
}

int
destroy_line(line_t *line)
{
 if (!line)
    return 0;

 line_t *tmp = line;
 if(line->backw)
   line->backw->forw = line->forw;
 if(line->forw)
   line->forw->backw = line->backw;
 free(tmp->str);
 free(tmp);

 return 0;
}

line_t*
make_line_after(line_t *prev)
{
 line_t *cur, *next = NULL;

 
 if(prev)
   next = prev->forw;
 cur = malloc( sizeof(line_t) );
 if(!cur) {
   fprintf(stderr, "malloc failed\n");
   return cur;
 }
 cur->forw = next;
 if(next)
   next->backw = cur;
 if(prev)
   prev->forw = cur;
 cur->backw = prev;

 return cur;
}

line_t*
make_line_before(line_t *next)
{
 line_t *cur, *prev = NULL;
 
 if(next)
   prev = next->backw;
 cur = malloc( sizeof(line_t) );
 if(!cur) {
   fprintf(stderr, "malloc failed\n");
   return cur;
 }
 cur->backw = prev;
 if(prev)
   prev->forw = cur;
 if(next)
   next->backw = cur;
 cur->forw = next;

 return cur;
}

int
init_line( line_t *ln )
{
 ln->str = malloc(1);
 *(ln->str) = '\0';
 ln->len = 0;
 return 0;
}


int
make_chain()
{
 int i;

 line_t *pcur, *phead, *ptail;
 pcur = NULL;
 pcur = make_line_after(NULL);
 printf("pcur->backw: %p\n", pcur->backw);
 printf("pcur: %p\n", pcur);
 printf("pcur->forw: %p\n", pcur->forw);
 phead = pcur;
 pcur = make_line_after(pcur);
 printf("pcur->backw: %p\n", pcur->backw);
 printf("pcur: %p\n", pcur);
 printf("pcur->forw: %p\n", pcur->forw);
 pcur = make_line_after(pcur);
 printf("pcur->backw: %p\n", pcur->backw);
 printf("pcur: %p\n", pcur);
 printf("pcur->forw: %p\n", pcur->forw);

 //pcur = phead;
 for(i = 0; pcur->backw; i++)
    pcur = pcur->backw;
 printf("i: %d\n", i);
 return 0;

}


int 
make_line()
{
 int i;
 line_t *prev, *next;

 if( cur_line == NULL ) {
    cur_line = malloc( sizeof(line_t) );
    cur_line->backw = NULL;
    cur_line->str = malloc(1);
    *(cur_line->str) = '\0';
    return 0;
 }

 if( cur_line->backw == NULL) {
    next = cur_line;
    cur_line = malloc( sizeof(line_t) );
    next->backw = cur_line;
    cur_line->backw = NULL;
    cur_line->forw = next;
    cur_line->str = calloc(1,1);
    cur_line->len = 0;
    print_cur();
 moveyx(_wincur, 1);
    return 0;
 }


 prev = cur_line->backw;
 next = cur_line;
 prev->forw = malloc( sizeof(line_t) );
 cur_line = prev->forw;
 next->backw = cur_line;

 cur_line->backw = prev;
 cur_line->forw = next;
 cur_line->str = calloc(1,1);
 cur_line->len = 0;
 
 print_cur();
 moveyx(_wincur, 1);

 return 0;
}


int
insert_string_after2()
{
 int i;
 char *buf = malloc(MAX_LINE);

 struct termios term;
 struct termios restore;

 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag |= (ICANON | ECHO);
 tcsetattr(0, TCSANOW, &term);

 save_cur();
 getstring(buf, MAX_LINE);
 insert( &cur_line->str, _offs + 1, buf);
 cur_line->len = strlen( cur_line->str);
 moveyx(_y_cur, 1);
 printf(CLL"%s", cur_line->str  );
 rest_cur();

 tcsetattr(0, TCSANOW, &restore);
 return 0;
}


insert_block()
{
 struct termios term;
 struct termios restore;
 int ret = -1;
 int tmp;
 int len = 0;
 char buf[MAX_LINE]; 

 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag |= (ICANON | ECHO);
 tcsetattr(0, TCSANOW, &term);

 line_t *first, *last;

 tmp = _winbot;
 _winbot = _wincur;
 set_scroll_win(_wintop, _winbot);  

 while( ret != 27 ) { 
    scroll_up();
    moveyx(_wincur, 1);
    ret = getstring(buf, MAX_LINE);
    cur_line = make_line_after(cur_line);
    len = strlen(buf);
    cur_line->str = malloc( len + 1 );
    strcpy( cur_line->str, buf ); 
    cur_line->len = len;
 }
 _winbot = tmp;
 set_scroll_win(_wintop, _winbot);  
 moveyx(_wincur, 1);
 tcsetattr(0, TCSANOW, &restore);
 return 0;
}


int
insert_space()
{
 insert( &cur_line->str, _offs, " ");
 cur_line->len++;
 moveyx(_wincur, 1);
 printf(CLR"%s", cur_line->str);
 moveyx(_wincur, _offs+1);
 return 0;
}

int
insert_char(char ch)
{
if( !cur_line )  return 0;

 char str[2];
 str[0] = ch;
 str[1] = 0;
 insert( &cur_line->str, _offs, str);
 cur_line->len++;
 moveyx(_wincur, 1);
 printf(CLR"%s", cur_line->str);
 moveyx(_wincur, _offs+1);
 return 0;
}


int
insert_char_next(char ch)
{
 save_cur();
 moveyx(_y_cur, _x_cur + 1);
 _offs++;
 insert_char(ch);
 return 0;
}


int 
read_line(FILE* file, char* buf)
{
 int fd, i;
 char c;
 c = fgetc(file);
 for( i = 0; c != '\n' && c != EOF && c != '\0'; i++ ) {
    buf[i] = c;
    c = fgetc(file);
 }
 buf[i] = '\0';
 return c;
}


int
read_buffer(FILE* file, line_t **head, line_t **tail)
{
 char buf[MAX_LINE];
 line_t *lines = NULL;

 while( read_line(file, buf) != EOF ) {
    lines = make_line_after(lines);
    lines->len = strlen( buf );
    lines->str = malloc( lines->len + 1 );
    strcpy( lines->str, buf);
 }
    lines = make_line_after(lines);
    lines->len = strlen( buf );
    lines->str = malloc( lines->len + 1 );
    strcpy( lines->str, buf);

    *tail = lines;
    while( lines->backw )
       lines = lines->backw;
    *head = lines;

 return 0;
}


int
message(char* s)
{
 save_cur();
 moveyx(1, 1);
 printf(CLL"%s", s);
 rest_cur();
 return 0;
}

int
gmessage(char* s)
{
 save_cur();
 moveyx(1, 1);
 printf(CLL"%s", s);
 getchar();
 rest_cur();
 return 0;
}

int 
write_buf(int fd)
{
 message("writing");
 lseek(fd, 0, SEEK_SET);
 int i, res;
 char buf[500];
 line_t *line;

 FILE *file = fdopen(fd, "r+");
 if (!file) {
 perror("fdopen");
   return 1;
 }
 ftruncate(fd, 0);
 

 line = get_head();

 if (!line) {
 gmessage("line == NULL");
//    fclose(file);
    return 0;
 }

 while( line->forw ) 
 {
    strcpy( buf, line->str);
    for(i = 0; buf[i] != '\0'; i++)
       fputc(buf[i], file);
    fputc('\n', file);
    line = line->forw;
 }
 strcpy( buf, line->str);
 for(i = 0; buf[i] != '\0'; i++)
    fputc(buf[i], file);

 fflush(file);
// fclose(file);
}


int
move_down(int n)
{
 int i;

 if( cur_line == NULL)
    return 0;

 for( i = 0; i < n && cur_line->forw; i++) {
    reset();
    scroll_up();
    cur_line = cur_line->forw;
    print_bottom_line();
 }
 return 0;
}

int
move_up(int n)
{
 int i;

 if( cur_line == NULL )
    return 0;

 for( i = 0; i < n && cur_line->backw; i++) {
    reset();
    scroll_down();
    cur_line = cur_line->backw;
    print_top_line();
 }
 return 0;
}

int
move_first()
{
 while( cur_line->backw != NULL )
    cur_line = cur_line->backw;
 //print_cur();
 redraw();
 return 0;
}

int
move_last()
{
 while( cur_line->forw != NULL )
    cur_line = cur_line->forw;
 //print_cur();
 redraw();
 return 0;
}

int
reset()
{
 _offs = 0;
 _x_curs = 1;
 return 0;
}

int
move_first_col()
{
 _offs = 0;
 _x_curs = 1;
 getpos(&_y_cur, &_x_cur);
 moveyx(_y_cur, 1);
 return 0;
}

int
move_last_col()
{
if ( !cur_line )
	return 0;

 move_right(cur_line->len);
 return 0;
}


int
move_right(int n)
{
 int i;

 if (cur_line)
 for( i = 0; i < n; i++ )
 if( _offs + 1 < cur_line->len ) {
    if( *(cur_line->str + _offs + 0) == '\t' ) {
       _x_curs = get_next_tabstop();
       moveyx(_wincur, _x_curs);
    }
    else 
       printf("\033[%dC", 1);
       _offs++;
       _x_curs++;
 }
  return 0;
}

int
move_left(int n)
{
 int i, tmp = _offs;
 for( i = 0; i < n && _offs != 0; i++ ) {
    move_first_col();
    move_right(--tmp);
 }
 return 0;
}

int
get_next_tabstop()
{
 int i;
 getpos(&_y_cur, &_x_cur);
 message("tab");
 for(i = 0; _x_cur >= _tabstops[i]; i++)
 	;
 return _tabstops[i];
}

int 
get_prev_tabstop()
{
 int i;
 getpos(&_y_cur, &_x_cur);
 for(i = 0; _x_cur > _tabstops[i]; i++)
 	;
 return _tabstops[i];
}

int
move_tab_forw(int pos)
{
 /*
 int x;
 x = pos  / TABS + TABS;
 moveyx(13, x);
 */
 write(1, "\t", 1);
 return 0;
}

int
move_tab_backw(int pos)
{
 /*
 int x;
 x = (_offs + 1) / TABS;
 moveyx(13, x);
 write(1, "\t", 1);
 */
// write(1, "\033[8D", strlen("\033[8D") );
 write(1, "\b", 1 );
 return 0;
}

/*
word
*/

int
get_bow()
{
 int i, tmp;
 char *str;

 str = cur_line->str;
 i = tmp = _offs;
 for ( ; isalnum( str[i] ) && str[i] != 0; i++ );
 for ( ; !isalnum( str[i] ) && str[i] != 0; i++ );
/*
 if ( isalnum( str[i] ) ) {
    tmp = i;
}
*/
 return i;
}

int
get_eow()
{
 int i, tmp;
 char *str;

 str = cur_line->str;
 i = tmp = _offs;
 for ( ; !isalnum( str[i] ) && str[i] != 0; i++ );
 for ( ; isalnum( str[i] ) && str[i] != 0; i++ );
/*
 if ( !isalnum( str[i] ) && str[i] ) {
    tmp = i;
}
*/
 return i;
}

int
move_bow()
{
char buf[100];
 int i = 0;
i = get_bow();
sprintf(buf, "gt_bow:%d", i);
message(buf);
 move_first_col();
 move_right(i);
}

int
move_eow()
{
char buf[100];
 int i = 0;
i = get_eow();
sprintf(buf, "gt_eow:%d", i);
message(buf);
 move_first_col();
 move_right(i);
}

int
del_word()
{
if( !cur_line )
    return 0;

 int i, to;

 to = ( isalnum( *(cur_line->str + _offs) ) ) ? get_eow() : get_bow();


 del( &cur_line->str, _offs + 1, to - (_offs + 0) );
 cur_line->len = strlen(cur_line->str);
 save_cur();
 moveyx(_y_cur, 1);
 printf(CLL"%s", cur_line->str);
 rest_cur();

if( *(cur_line->str + _offs) == 0)
  move_left(1);

 char buf[200];
 sprintf(buf, "_offs:%d", _offs);
 message(buf);

return 0;
}

/*
int
print_line(char* str)
{
 int i;
 for( i = 0; *(str+i) != '\0'; i++ ) {
    switch ( *(str+i) ) {
       case '\t':
	  place_tab_forw();
	  break;
       default:
	  putchar( *(str+i) );
	  break;
    }
 }
 return 0;
}
*/


int
del_line()
{
    line_t *prev;
    line_t* tmp;
 /*
 printf("cur_line->backw%p\n", cur_line->backw);
 printf("cur_line->forw%p\n", cur_line->forw);
 getchar();
 */

 if( cur_line == NULL)
    return 0;
 else

 if( cur_line->backw == NULL && cur_line->forw == NULL ) {
    free(cur_line->str);
    free(cur_line);
    cur_line = NULL;
 print_cur();
    //return 0;
 }
 else

 if( cur_line->forw == NULL && cur_line->backw != NULL) {
    tmp = cur_line; 
    cur_line = cur_line->backw;
    cur_line->forw = NULL;
    free( tmp->str );
    free( tmp );
    scroll_down();
 print_cur();
    //move_up(1);
 }
 else

 if( cur_line->backw == NULL && cur_line->forw != NULL ) {

    tmp = cur_line; 
    cur_line = cur_line->forw;
    cur_line->backw = NULL;
    //tmp->backw->forw = cur_line;
    free( tmp->str );
    free( tmp );
    //move_down(1);
 }
 
 else {
 tmp = cur_line; 
 cur_line = cur_line->forw;
 cur_line->backw = tmp->backw;
 tmp->backw->forw = cur_line;
 free( tmp->str );
 free( tmp );
 }

 print_cur();

 /*
 if( cur_line->forw == NULL )
    move_up(1);
 */
 return 0;
}

int
output_line(line_t* l)
{
 printf("%s\n", l->str);
 return 0;
} 



int
print_lines(int s, int n)
{
 int i;
 line_t* pr_line;
 pr_line = get_head();
 for( i = 0; i < s && pr_line->forw != NULL; i++ )
    pr_line = pr_line->forw; 
 for( i = 0; pr_line->forw != NULL && i < n; i++ ) {
    printf("%s%s\n", CLL, pr_line->str);
    pr_line = pr_line->forw;
    }
}


int
print_top_line()
{
 int i;
 line_t* pr_line;
 pr_line = cur_line;
 for( i = 0; i < _wincur - _wintop; i++ ) {
    if (pr_line->backw == NULL) return;
    pr_line = pr_line->backw; 
 }
 save_cur();
 moveyx(_wintop,1);
 printf("%s", pr_line->str);
 rest_cur();
}

int
print_bottom_line()
{
 int i;
 line_t* pr_line;
 pr_line = cur_line;
 for( i = 0; i < _winbot - _wincur; i++ ) {
    if (pr_line->forw == NULL) return;
    pr_line = pr_line->forw; 
 }
 save_cur();
 moveyx(_winbot,1);
 printf("%s", pr_line->str);
 rest_cur();
}

print_lines_cur(int s, int n)
{
 int i;
 line_t* pr_line;
 pr_line = cur_line;
 for( i = 0; i < s && pr_line->forw != NULL; i++ )
    pr_line = pr_line->forw; 
 for( i = 0; pr_line->forw != NULL && i < n; i++ ) {
    printf("\033[2K%s\n", pr_line->str);
    pr_line = pr_line->forw;
 }
}

int
print_cur()
{
 int i;
 line_t* pr_line;
 pr_line = cur_line;
 moveyx(_wincur, 1);
 clear_down();
 if( pr_line == NULL)
    return 0;
 for( i = _wincur; /*pr_line->forw != NULL &&*/ i <= _winbot - 1; i++ ) {
    printf("\033[2K%s\n",  pr_line->str );
    if ( pr_line->forw == NULL ) { moveyx(_wincur, 1); return; }
    pr_line = pr_line->forw;
 }
    printf("\033[2K%s",  pr_line->str );
 moveyx(_wincur, 1);
 return 0;
}

int
redraw()
{
 int i, n;
 line_t* ln;
 ln = cur_line;
 clear_all();
 if(!ln)
  return 1;
 for( n = _wincur; n > _wintop && ln->backw != NULL; ) {
    ln = ln->backw;
    n--;
 }
//gmessage("debug");
 moveyx(n+0, 1);
 for( i = 0; i < _winbot - n - 0  && ln->forw != NULL; i++) {
    printf("%s\n", ln->str);
    ln = ln->forw;
    //getchar();
 }
    printf("%s", ln->str);
 moveyx(_wincur, 1);
}


char* 
find_str()
{
 int i;
 char* res = NULL;

 moveyx(_wincur, 1);
 search_pos = cur_line;
 while(res == NULL && search_pos != NULL) {
//    res = strcasestr(search_pos->str, search_buf);
    res = strstr(search_pos->str, search_buf);
    if ( res != NULL )
       break;
    search_pos = search_pos->forw;
 }
  if(res == NULL) {
  message("no matching");
  return res;
 }
 cur_line = search_pos;
 redraw();
 message(search_pos->str);
 return res;
}

int 
find_beginning()
{
 int i;
 int res = -1;

 moveyx(_wincur, 1);
 search_pos = cur_line;
 while(res != 0 && search_pos != 0) {
    res = strncmp(search_pos->str, search_buf, strlen(search_buf));
    if ( res == 0 )
       break;
    search_pos = search_pos->forw;
 }
  if(res != 0) {
  message("no matching");
  return res;
 }
 cur_line = search_pos;
 redraw();
 message(search_pos->str);
 return res;
}

int
search_and_replace()
{
 int i;
 int len, s_len, r_len, o;
 char *pch;
 line_t *pos = get_head();
 static char s_buf[MAX_LINE];
 static char r_buf[MAX_LINE];
 static char buf[MAX_LINE];

 moveyx(_rows - 1, 1);
 printf(CLL"Search:");
 getstring(s_buf, MAX_LINE);
 s_len = strlen(s_buf);
 printf(" Replace:");
 getstring(r_buf, MAX_LINE);
 r_len = strlen(r_buf);
 while( pos != NULL )
 {
   pch = strstr( pos->str, s_buf );
   o = pch - pos->str;
   while(pch) {
      strncpy(buf, pos->str, pch - pos->str);
      buf[pch - pos->str] = 0;
      strcat(buf, r_buf);
      strcat(buf, pch + s_len);
      pos->len = strlen(buf);
      pos->str = realloc(pos->str, pos->len + 1);
      strcpy(pos->str, buf);
      pch = strstr( pos->str + o + r_len, s_buf );
      o = pch - pos->str;

   }
   pos = pos->forw;
 }
 redraw();
 return 0;
}



/************ copy buffer ***************/
int
copy_lines(line_t *start, line_t *end, line_t **buf_head, line_t **buf_tail)
{

 line_t *source = start;
 line_t *dest = NULL;

 if( start == NULL || end == NULL ) return 1;

 dest = make_line_after(dest);
 dest->str = malloc( strlen( source->str ) + 1 );
 strcpy( dest->str, source->str );
 dest->len = source->len;
 source = source->forw;
 *buf_head = dest;
 while( source != end->forw ) {
    dest = make_line_after(dest);
    dest->str = malloc( strlen( source->str ) + 1 );
    strcpy( dest->str, source->str );
    dest->len = source->len;
    source = source->forw;
 }
 *buf_tail = dest;
 message(CLL"copy_lines... DONE");
 return 0;
}

int
insert_lines(line_t *place, line_t *buf_head, line_t *buf_tail)
{
 if( !place || !buf_head || !buf_tail ) return 1;

 int i;
 line_t *tmp;
 line_t *lines;
 tmp = place->forw;
 buf_head->backw = place;
 place->forw = buf_head;
 if(tmp)
 tmp->backw = buf_tail;
 buf_tail->forw = tmp;
 return 0;
}

int
insert_lines_before(line_t *place, line_t *buf_head, line_t *buf_tail)
{
 if( !place || !buf_head || !buf_tail ) return 1;

 int i;
 line_t *tmp;
 line_t *lines;
 tmp = place->backw;
 buf_tail->forw = place;
 place->backw = buf_tail;
 if(tmp)
 tmp->forw = buf_head;
 buf_head->backw = tmp;
 return 0;
}

int
set_copy_start_pos()
{
 copy_start_pos = cur_line;
 return 0;
}

int
set_copy_end_pos()
{
 copy_end_pos = cur_line;
 return 0;
}

int
set_mark1()
{
 mark1 = cur_line;
 return 0;
}

int
set_mark2()
{
 mark2 = cur_line;
 return 0;
}

int
go_mark(line_t *mark)
{
 line_t *line;
 line = get_head();
 while(line != mark && line != NULL)
    line = line->forw;
 if( line == mark && mark != NULL ) {
    cur_line = line;
    redraw();
 }
 return 0;
}

int
arrange_markers()
{
 line_t *tmp, *lines, *start, *end;
 int match = 0;
 int valid_start = 0;
 int valid_end = 0;

 start = copy_start_pos;
 end = copy_end_pos;
 lines = get_head();

 while( lines ) {
    if( start == lines )
       valid_start = 1;
    if( end == lines )
       valid_end = 1;
    lines = lines->forw;

 }

 if( valid_start != 1 || valid_end != 1 )
    return 1;


 while( start != NULL && start != end ) {
    start = start->forw;
 }

 if( start == end) {
    match = 1;
 } 
 else {
    tmp = copy_start_pos;
    copy_start_pos = copy_end_pos;
    copy_end_pos = tmp;

    start = copy_start_pos;
    end = copy_end_pos;

    while( start != NULL && start != end )
       start = start->forw;
    if( start == end ) {
       match = 1;
    }
    else
    return 1;
 }

 start = copy_start_pos;
 end = copy_end_pos;

 return 0;
}

int
cmd_yank()
{

int  i = arrange_markers();
 if(i)
   return 1; 

 free_lines( copy_buffer_head, NULL);

 copy_lines( copy_start_pos, copy_end_pos, 
		&copy_buffer_head, &copy_buffer_tail );
 return 0;
}

int
cmd_paste()
{
 line_t *start_ins = NULL; 
 line_t *end_ins = NULL;
 copy_lines( copy_buffer_head, copy_buffer_tail, &start_ins, &end_ins );
 insert_lines( cur_line,  start_ins, end_ins);
 if (cur_line == NULL)
    cur_line = start_ins;
 message(CLL"insert line done");
 redraw();
 return 0;
}

int
cmd_del_lines()
{
 int i = arrange_markers();
 if(i)
   return 1;

 line_t *l = copy_start_pos;
 line_t *m = copy_end_pos;
 line_t *prev, *next;

 if(l)
 prev = l->backw;
 if(m)
 next = m->forw;

 assert(l != NULL && m != NULL);

 free_lines(l, m);
 if(prev)
 prev->forw = next;
 if(next)
 next->backw = prev;

 if(prev)
 cur_line = prev;
 else
 cur_line = next;
 redraw();
 return 0;
}

int 
free_lines(line_t *l, line_t *m)
{
 line_t *next;
 while( l != NULL && l != m ) {
    next = l->forw;
    free( l->str );
    free( l );
    l = next;
 }
 return 0;
}

int
cur_line_number()
{

 if( !cur_line )
   return 0;


 int i;
 line_t *line;

 line = get_head();
 for(i = 1; line != cur_line; i++) {
   line = line->forw;
 }
 return i;
}


/************* end buffer functions **********************/
line_t*
get_head(){
 int i;
 line_t *line = cur_line;

 if (!line) return line;

 while( line->backw != NULL )
    line = line->backw;
 return line;
}

line_t*
get_tail(){
 int i;
 line_t *line = cur_line;

 if (!line) return line;

 while( line->forw != NULL )
    line = line->forw;
 return line;
}

int 
debug_bar()
{
 save_cur();
 moveyx(_rows-1, 1);
 printf(CLL"_x_curs: %d\t", _x_curs);
 printf("%1$d %2$x %2$c", _offs, *(cur_line->str + _offs) );
 printf("  :%1$d %2$x %2$c", _offs + 1, *(cur_line->str + _offs + 1) );
// printf("  :%1$d %2$x %2$c", _offs + 2, *(cur_line->str + _offs + 2) );
// printf("  :%1$d %2$x %2$c", _offs + 3, *(cur_line->str + _offs + 3) );
 rest_cur();
 return 0;
}

int
show_bar()
{
 int line_no = cur_line_number();
 save_cur();
 moveyx(_rows, 1);
 printf(CLL);
 printf("\t\t\t\t%s", _filename);
 moveyx(_rows, 1);
 printf("\t\t_key_mode: %d", _key_mode);
 moveyx(_rows, 1);
 printf("line: %d", line_no);
 rest_cur();
 return 0;
}

line_t*
goto_line( int n )
{
 int i;
 line_t *line = get_head();

 for( i = 1; i != n && line != NULL; i++)
 line = line->forw;

 return line;
}

int
cmd_goto_line()
{
 char buf[MAX_LINE];
 int n;
 line_t *line;
 save_cur();
 moveyx(_winbot + 1, 1);
 printf(CLL);
 getstring(buf, MAX_LINE);
 rest_cur();
 n = atoi(buf);
 if( line = goto_line( n ) ) {
    cur_line = line;
    redraw();
 }
 reset();
 return 0;
}


void sig_handler(int i)
{
   setecho();
   setcanon();
   //printf("%d\n", i);
}

void sigsegv_handler(int i)
{
   printf("SIGSEGV\n");
   abort();
}

void exit_handler()
{
   printf("exit\n");
   setecho();
   setcanon();
}

//*&^^**^jsdfhf*^***&*word!##$#$%$^
