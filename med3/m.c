#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
/*
#include "med.h"
#include "term.h"
*/
#include "color.h"

#define INIT_NUM 1000
#define MAX_LINE 500
#define TABS 8
#define NCOLS 80
#define NROWS 25


typedef struct line {
   char* str;
   int len;
   struct line *next;
   struct line *prev;
} line_t;


//enum {};


#define K_KMOD '`'
#define K_UP 'k'
#define K_UP1 ','
#define K_UP2 'i'
#define K_DOWN 'j'
#define K_DOWN1 'm'
#define K_DOWN2 'u'
#define K_RIGHT 'f'
#define K_RIGHT1 'v'
#define K_RIGHT2 'r'
#define K_LEFT 'd'
#define K_LEFT1 'c'
#define K_LEFT2 'e'
#define K_DELLINE 'y'
#define K_MKLINE 'l'
#define K_DEL 'g'
#define K_INS_MOD 'b'
#define K_INS_MOD1 'B'
#define K_INS_SPACE 32
#define K_FIND1 '/'
#define K_REFIND 'n'
#define K_WRITE 'w'
#define K_QUIT 'q'
#define K_SET_COPY_START 'a'
#define K_SET_COPY_END 'z'
#define K_COPY 's'
#define K_PASTE 'x'

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
		( printf("\033[%d;H\033M\033[%d;H", (wintop), (wincent) ) )

#define scroll_up() \
		( printf("\033[%d;H\033D\033[%d;H", (winbot), (wincent) ) )

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

/*** fundction definitions ***/
int save_cur();
int rest_cur();
int getsize ();
int setnocanon();
int setcanon();
int setnoecho();
int setecho();
int getpos(int *y, int *x);
int set_tabs(int ncols, int tabs);
char* del (char *str, int o, int n);
int del_char();
int del_char_right();
char* insert (char *str1, int n, char *str2);
int getstring(char* buf, int nmax);
line_t* make_line_after(line_t *prev);
line_t* make_line_before(line_t *next);
int make_chain();
int make_line();
int insert_string();
int insert_space();
int insert_char(char ch);
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
int move_tab_forw(int pos);
int move_tab_backw(int pos);
int print_line(char* str);
int del_line();
int output_line(line_t* l);
int print_lines(int s, int n);
int print_cur();
char* find_str();
int goto_curline();
int copy_lines(line_t *start, line_t *end, 
     line_t **buf_head, line_t **buf_tail);
int insert_lines(line_t *place, line_t *buf_head, line_t *buf_tail);
int set_copy_start_pos();
int set_copy_end_pos();
int cmd_yank();
int cmd_paste();
int free_lines(line_t *l);

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

int key_mode = 0;
int x_offs;
int cols, rows;
int wintop, winbot, wincent;

char search_buf[MAX_LINE];

int x_cur, y_cur;

char *str_global;

int save_cur() {
 getyx(stdscr, y_cur, x_cur);

}

int rest_cur() {
 moveyx(y_cur, x_cur);
}

int getsize () {
   struct winsize ws;
   ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
   rows = ws.ws_row;
   cols = ws.ws_col;
   //printf ("Rows: %d, Cols: %d\n", ws.ws_row, ws.ws_col);
   printf ("Rows: %d, Cols: %d\n", rows, cols);
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
 sleep(4);
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
 printf("%s\n", buf+1);
 printf("i: %d\n", i);
 //exit(1);
 if(i<2) {
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

char*
del (char *str, int o, int n)
{
 char *buf;
 int i;
 int len = strlen(str);
 buf = calloc( len + 1, 1 );

 if( o <= 0 || n < 0 )
    return NULL;

 if( o > len ) {
    strcpy(buf, str);
 } else
 if( o + n > len ) {
    strncpy(buf, str, o - 1);
 } 
 else {
    strncpy(buf, str, o - 1);
    strcat(buf, str + o - 1 + n);
 }
return buf;
}

int
del_char()
{
 if( cur_line->len == 0 )
    del_line();
 char *buf = calloc( strlen( cur_line->str ) - 1 + 1, 1 );
 strcpy( buf, del(cur_line->str, x_offs+1, 1) );
/*
 if(cur_line->str == NULL) {
  printf("Alarm!!!\n");
  exit(1); 
  }
  */
 free( cur_line->str );
 cur_line->str = buf;
 printf( CLR"%s", cur_line->str + x_offs);
 moveyx(13, x_offs+1);
 cur_line->len = strlen(cur_line->str);
 return 0;
}

int
del_char_right()
{
 char buf[MAX_LINE];
 int len = strlen( cur_line->str );
//*(cur_line->
 memmove(cur_line->str + x_offs, cur_line->str + x_offs + 1, len + 1);
 printf( CLR"%s", cur_line->str + x_offs);
 moveyx(13, x_offs+1);
 //printf(KGRN"%d\n"KNRM, x_offs );
 return 0;
}


char*
insert (char *str1, int n, char *str2)
{
 char *buf;
 int i;

 if ( n > strlen(str1) ) return NULL;

 buf = calloc( strlen(str1) + strlen(str2) + 1, 1);
 strncpy(buf, str1, n);
 strcat(buf, str2);
 strcat (buf, str1+n);
 return buf;
}

int
getstring(char* buf, int nmax) {
 int i, res;
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
	  *(buf+i) = ch;
	  write(1, buf+i, 1);
	  i--;
	  break;

       case 10: case 27: case 0:
	  ch = 0;
	  *(buf+i) = 0;
	  break;

       default:
	  *(buf+i) = ch;
	  write(1, buf+i, 1);
	  i++;
    }

 } while ( ch != 0 );

 //setecho();

 return 0;
}

line_t*
make_line_after(line_t *prev)
{
 line_t *cur, *next = NULL;

 
 if(prev)
   next = prev->next;
 cur = malloc( sizeof(line_t) );
 if(!cur) {
   fprintf(stderr, "malloc failed\n");
   return cur;
 }
 cur->next = next;
 if(next)
   next->prev = cur;
 if(prev)
   prev->next = cur;
 cur->prev = prev;

 return cur;
}

line_t*
make_line_before(line_t *next)
{
 line_t *cur, *prev = NULL;
 
 if(next)
   prev = next->prev;
 cur = malloc( sizeof(line_t) );
 if(!cur) {
   fprintf(stderr, "malloc failed\n");
   return cur;
 }
 cur->prev = prev;
 if(prev)
   prev->next = cur;
 if(next)
   next->prev = cur;
 cur->next = next;

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
 printf("pcur->prev: %p\n", pcur->prev);
 printf("pcur: %p\n", pcur);
 printf("pcur->next: %p\n", pcur->next);
 phead = pcur;
 pcur = make_line_after(pcur);
 printf("pcur->prev: %p\n", pcur->prev);
 printf("pcur: %p\n", pcur);
 printf("pcur->next: %p\n", pcur->next);
 pcur = make_line_after(pcur);
 printf("pcur->prev: %p\n", pcur->prev);
 printf("pcur: %p\n", pcur);
 printf("pcur->next: %p\n", pcur->next);

 //pcur = phead;
 for(i = 0; pcur->prev; i++)
    pcur = pcur->prev;
 printf("i: %d\n", i);
 return 0;

}


int 
make_line()
{
 int i;
 line_t *tmp, *prev, *next;

 if( cur_line == NULL ) {
    cur_line = malloc( sizeof(line_t) );
    cur_line->prev = NULL;
    cur_line->str = malloc(1);
    *(cur_line->str) = '\0';
    return 0;
 }

 if( cur_line->prev == NULL) {
    next = cur_line;
    cur_line = malloc( sizeof(line_t) );
    next->prev = cur_line;
    cur_line->prev = NULL;
    cur_line->next = next;
    cur_line->str = calloc(1,1);
    cur_line->len = 0;
    print_cur();
 moveyx(wincent, 1);
    return 0;
 }


 prev = cur_line->prev;
 next = cur_line;
 prev->next = malloc( sizeof(line_t) );
 cur_line = prev->next;
 next->prev = cur_line;

 cur_line->prev = prev;
 cur_line->next = next;
 cur_line->str = calloc(1,1);
 cur_line->len = 0;
 
 print_cur();
 moveyx(wincent, 1);

 return 0;
}

int
insert_string()
{
 int i;
 char buf[MAX_LINE];

 struct termios term;
 struct termios restore;

 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag |= (ICANON | ECHO);
 tcsetattr(0, TCSANOW, &term);

 getstring(buf, MAX_LINE);
 cur_line->str = insert( cur_line->str, x_offs, buf);
 cur_line->len = strlen( cur_line->str);
 moveyx(wincent, 1);
 printf(CLR"%s", cur_line->str);
 moveyx(wincent, x_offs+1);

 tcsetattr(0, TCSANOW, &restore);
 return 0;
}

insert_string2()
{
 struct termios term;
struct termios restore;

 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag |= (ICANON | ECHO);
 tcsetattr(0, TCSANOW, &term);


 line_t *first, *last;
 read_buffer(stdin, &first, &last);
 if( cur_line ) {
    if( cur_line->next )
       cur_line->next->prev = last;
    last->next = cur_line->next;
    cur_line->next = first;
    first->prev = cur_line;
 }
 else {
    lines_head = first;
    lines_tail = last;
    cur_line = lines_head;
 }
 goto_curline();


 tcsetattr(0, TCSANOW, &restore);
 return 0;
}



int
insert_space()
{
 cur_line->str = insert( cur_line->str, x_offs, " ");
 cur_line->len++;
 moveyx(wincent, 1);
 printf(CLR"%s", cur_line->str);
 moveyx(wincent, x_offs+1);
 return 0;
}

int
insert_char(char ch)
{
 char str[2];
 str[0] = ch;
 str[1] = 0;
 cur_line->str = insert( cur_line->str, x_offs, str);
 cur_line->len++;
 moveyx(wincent, 1);
 printf(CLR"%s", cur_line->str);
 moveyx(wincent, x_offs+1);
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
    while( lines->prev )
       lines = lines->prev;
    *head = lines;

 return 0;
}


int
message(char* s)
{
 moveyx(25, 1);
 printf(CLL"%s", s);
 moveyx(wincent, 1);
 return 0;
}

int
gmessage(char* s)
{
 moveyx(25, 1);
 printf(CLL"%s", s);
 getchar();
 moveyx(wincent, 1);
 return 0;
}

int 
write_buf(int fd)
{
 message("writing");
 lseek(fd, 0, SEEK_SET);
 int i, res;
 char buf[500];
 line_t *lines;

 FILE *file = fdopen(fd, "r+");
 if (!file)
 perror("fdopen");
 ftruncate(fd, 0);
 
 if( lines_head == NULL ) {
    return 0;
 }

 while( lines_head->prev != NULL )
    lines_head = lines_head->prev;

 lines = lines_head;
 while( lines->next ) 
 {
    strcpy( buf, lines->str);
    for(i = 0; buf[i] != '\0'; i++)
       fputc(buf[i], file);
    fputc('\n', file);
    lines = lines->next;
 }
 strcpy( buf, lines->str);
 for(i = 0; buf[i] != '\0'; i++)
    fputc(buf[i], file);

 fflush(file);
}

int
work_buffer()
{
 int i;
 return 0;
}

int
move_down(int n)
{
 int i;

 if( cur_line == NULL)
    return 0;

 for( i = 0; i < n; i++)
 if ( cur_line->next != NULL ) {
 //if ( cur_line->next->next != NULL ) 
    x_offs = 0;
    scroll_up();
    cur_line = cur_line->next;
    moveyx(winbot,1);
    print_bottom_line();
    moveyx(wincent,1);
 }
 return 0;
}

int
move_up(int n)
{
 int i;

 if( cur_line == NULL)
    return 0;

 for( i = 0; i < n; i++)
 if ( cur_line->prev != NULL ) {
 //if ( cur_line->prev->prev != NULL ) 
 x_offs = 0;
    scroll_down();
    cur_line = cur_line->prev;
    moveyx(wintop,1);
    print_top_line();
    moveyx(wincent,1);
 }
 return 0;
}

int
move_first()
{
 while( cur_line->prev != NULL )
    cur_line = cur_line->prev;
 //print_cur();
 goto_curline();
 return 0;
}

int
move_last()
{
 while( cur_line->next != NULL )
    cur_line = cur_line->next;
 //print_cur();
 goto_curline();
 return 0;
}

int
move_left(int n)
{
 int i;
;	;
 for( i = 0; i < n; i++ )
 if( x_offs + 1 > 0 ) {
     if( *(cur_line->str + x_offs) == '\t' )
	   move_tab_backw(4);//????
	 else
    printf("\033[%dD", 1);
    if( x_offs > 0)
    x_offs--;
 }
 return 0;
}

int
move_right(int n)
{
 int i;
 for( i = 0; i < n; i++ )
 if( x_offs + 1 < cur_line->len ) {
     if( *(cur_line->str + x_offs + 0) == '\t' )
	  move_tab_forw(4);//????
     else //printf("TAB!!!   ");
     printf("\033[%dC", 1);
     fflush(stdout);
     x_offs++;
  }
  return 0;
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
 x = (x_offs + 1) / TABS;
 moveyx(13, x);
 write(1, "\t", 1);
 */
 write(1, "\033[8D", strlen("\033[8D") );
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
 printf("cur_line->prev%p\n", cur_line->prev);
 printf("cur_line->next%p\n", cur_line->next);
 getchar();
 */

 if( cur_line == NULL)
    return 0;
 else

 if( cur_line->prev == NULL && cur_line->next == NULL ) {
    free(cur_line->str);
    free(cur_line);
    cur_line = NULL;
 print_cur();
    //return 0;
 }
 else

 if( cur_line->next == NULL && cur_line->prev != NULL) {
    tmp = cur_line; 
    cur_line = cur_line->prev;
    cur_line->next = NULL;
    free( tmp->str );
    free( tmp );
    scroll_down();
 print_cur();
    //move_up(1);
 }
 else

 if( cur_line->prev == NULL && cur_line->next != NULL ) {

    tmp = cur_line; 
    cur_line = cur_line->next;
    cur_line->prev = NULL;
    //tmp->prev->next = cur_line;
    free( tmp->str );
    free( tmp );
    //move_down(1);
 }
 
 else {
 tmp = cur_line; 
 cur_line = cur_line->next;
 cur_line->prev = tmp->prev;
 tmp->prev->next = cur_line;
 free( tmp->str );
 free( tmp );
 }

 print_cur();

 /*
 if( cur_line->next == NULL )
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
 pr_line = lines_head;
 for( i = 0; i < s && pr_line->next != NULL; i++ )
    pr_line = pr_line->next; 
 for( i = 0; pr_line->next != NULL && i < n; i++ ) {
    printf("%s%s\n", CLL, pr_line->str);
    pr_line = pr_line->next;
    }
}


print_top_line()
{
 int i;
 line_t* pr_line;
 pr_line = cur_line;
 for( i = 0; i < wincent - wintop; i++ ) {
    if (pr_line->prev == NULL) return;
    pr_line = pr_line->prev; 
 }
 moveyx(wintop,1);
 printf("%s", pr_line->str);
 moveyx(wincent,1);
}

print_bottom_line()
{
 int i;
 line_t* pr_line;
 pr_line = cur_line;
 for( i = 0; i < winbot - wincent; i++ ) {
    if (pr_line->next == NULL) return;
    pr_line = pr_line->next; 
 }
 moveyx(winbot,1);
 printf("%s", pr_line->str);
 moveyx(wincent,1);
}

print_lines_cur(int s, int n)
{
 int i;
 line_t* pr_line;
 pr_line = cur_line;
 for( i = 0; i < s && pr_line->next != NULL; i++ )
    pr_line = pr_line->next; 
 for( i = 0; pr_line->next != NULL && i < n; i++ ) {
    printf("\033[2K%s\n", pr_line->str);
    pr_line = pr_line->next;
 }
}

int
print_cur()
{
 int i;
 line_t* pr_line;
 pr_line = cur_line;
 moveyx(wincent, 1);
 clear_down();
 if( pr_line == NULL)
    return 0;
 for( i = wincent; /*pr_line->next != NULL &&*/ i <= winbot - 1; i++ ) {
    printf("\033[2K%s\n",  pr_line->str );
    if ( pr_line->next == NULL ) { moveyx(wincent, 1); return; }
    pr_line = pr_line->next;
 }
    printf("\033[2K%s",  pr_line->str );
 moveyx(wincent, 1);
 return 0;
}

char* 
find_str()
{
 int i;
 char* res = NULL;

 moveyx(wincent, 1);
 search_pos = cur_line;
 while(res == NULL && search_pos != NULL) {
    res = strstr(search_pos->str, search_buf);
    if ( res != NULL )
       break;
    search_pos = search_pos->next;
 }
  if(res == NULL) {
  moveyx(25,1);
  printf(CLL"no matching");
  moveyx(wincent, 1);
  return res;
 }
 cur_line = search_pos;
 goto_curline();
 moveyx(25,1);
 printf(CLL"%s", search_pos->str);
 moveyx(wincent, 1);
 return res;
}

int
goto_curline()
{
 int i, n;
 line_t* ln;
 ln = cur_line;
 for( n = wincent; n > wintop && ln->prev != NULL; ) {
    ln = ln->prev;
    n--;
 }
 moveyx(n+0, 1);
 clear_all();
 for( i = 0; i < winbot - n - 0  && ln->next != NULL; i++) {
    printf("%s\n", ln->str);
    ln = ln->next;
    //getchar();
 }
    printf("%s", ln->str);
 moveyx(wincent, 1);
}
/************ buffer ***************/
int
copy_lines(line_t *start, line_t *end, line_t **buf_head, line_t **buf_tail)
{

 line_t *source = start;
 line_t *dest = NULL;

 if( start == NULL || end == NULL ) return 1;

 dest = make_line_after(dest);
 dest->str = malloc( strlen( source->str + 1 ) );
 strcpy( dest->str, source->str );
 dest->len = source->len;
 source = source->next;
 *buf_head = dest;
 while( source != end->next ) {
    dest = make_line_after(dest);
    dest->str = malloc( strlen( source->str + 1 ) );
    strcpy( dest->str, source->str );
    dest->len = source->len;
    source = source->next;
 }
 *buf_tail = dest;
 message(CLL"copy_lines... DONE");
 return 0;
}

int
insert_lines(line_t *place, line_t *buf_head, line_t *buf_tail) /* after */
{
 if( !place || !buf_head || !buf_tail ) return 1;
 int i;
 line_t *tmp;
 line_t *lines;
 tmp = place->next;
 buf_head->prev = place;
 place->next = buf_head;
 tmp->prev = buf_tail;
 buf_tail->next = tmp;
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
cmd_yank()
{
 line_t *tmp, *lines, *start, *end;
 int match = 0;
 int valid_start = 0;
 int valid_end = 0;

 start = copy_start_pos;
 end = copy_end_pos;
 lines = lines_head;

 while( lines ) {
    if( start == lines )
       valid_start = 1;
    if( end == lines )
       valid_end = 1;
    lines = lines->next;

 }

 if( valid_start != 1 || valid_end != 1 )
    return 1;


 while( start != NULL && start != end ) {
    start = start->next;
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
       start = start->next;
    if( start == end ) {
       match = 1;
    }
    else
    return 1;
 }

 start = copy_start_pos;
 end = copy_end_pos;

 free_lines( copy_buffer_head);

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
 message(CLL"insert line done");
 goto_curline();
 return 0;
}

int free_lines(line_t *l)
{
 line_t *next;
 while( l ) {
    next = l->next;
    free( l->str );
    free( l );
    l = next;
 }
 return 0;
}
/************* end buffer functions **********************/


int main( int argn, char *argv[])
{
//initscr();

 if( argn != 2 )
    return 0;


 x_offs = 0;

 setvbuf(stdout, NULL, _IONBF, 0);
 setvbuf(stdin,  NULL, _IONBF, 0);

 int fd;
 char ch;
 FILE* file;
 int file_len;
 char buf[MAX_LINE];

 copy_buffer_head = NULL;
 copy_buffer_tail = NULL;
 /*
 copy_buffer_head = malloc(sizeof(line_t*));
 copy_buffer_tail = malloc(sizeof(line_t*));
 *copy_buffer_head = malloc(sizeof(line_t));
 (*copy_buffer_head)->str = malloc(sizeof(char) * 100);
 *copy_buffer_tail = malloc(sizeof(line_t));
 (*copy_buffer_tail)->str = malloc(sizeof(char) * 100);
 */

 fd = open(argv[1], O_RDWR);
 if ( fd < 0 ) {
    perror("open");
    return 1;
 }
 file = fdopen(fd, "r+");

 file_len = lseek(fd, 0, SEEK_END);
 lseek(fd, 0, SEEK_SET);

 if( file_len )
    read_buffer(file, &lines_head, &lines_tail);
 cur_line = lines_head;

 setnocanon();
 setnoecho();

 getsize();

 wintop = 1 + 1;
 winbot = rows - 1;
 wincent = ( winbot - wintop ) / 2 + wintop;

 /*
 initscr();
 move(1,1);
 moveyx(5, 4);
 save_cur();
 printf("y_cur: %d, x_cur: %d\n", y_cur, x_cur);
 getchar();
 endwin();
 setecho();
 exit(1);
 */

 printf("\033[%d;%dr", wintop, winbot);//set window
 printf("CLL""wintop: %d, winbot: %d\n", wintop, winbot);
 //getchar();


 set_tabs(cols, TABS);
 printf("\033[;H");
 clear_all();
 printf("\033[%d;H", wincent);
 print_cur();

 printf("\033[%d;H", wincent);

 while( 1 ) {
    ch = getchar();

    switch( ch ) {
       case K_KMOD:
	  key_mode = (key_mode) ? 0 : 1;
	  moveyx(winbot+1, 1);
	  printf("key_mode: %d", key_mode);
	  moveyx(wincent, 1);
	  break;
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
       case K_MKLINE:
	  if (key_mode == 1) {
	     cur_line = make_line_before(cur_line);
	     init_line(cur_line);
	  }
	  else {
	     cur_line = make_line_after(cur_line);
	     init_line(cur_line);
	  }
	  goto_curline();
          //make_line();
	  x_offs = 0;
	  break;
       case K_FIND1: 
 moveyx(rows, 1);
 getstring(search_buf, MAX_LINE);
 //moveyx(wincent, 1);
          find_str();
	  break;
       case K_REFIND: 
	  if(search_pos)
	  cur_line = cur_line->next;
	  //search_pos = search_pos->next;
          find_str();
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
       case K_INS_MOD1:
	  insert_string2();
	  break;
       case K_INS_SPACE:
          insert_space();
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

       default:
	  break;
    }
 }


 exit:
 puts("exiting\n");
 setcanon();
 setecho();
// endwin();
 return 0;
}

