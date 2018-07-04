#define LOG 0
int make_commands();

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
#include <sys/stat.h>
#include "colors.h"
//#include "find.h"
#include "med.h"
#include "command.h"
#include "keys.h"
#include <errno.h>
#include <wchar.h>
#include "undo.h"


#define moveyx(y, x) ( printf("\033[%d;%dH", (y), (x)) )
#define movex(x) ( printf("\033[;%dH", (x)) )
#define movey(y) ( printf("\033[%d;H", (y)) )
#define clreol() (printf("\033[K") )
#define CLR "\033[K"
#define CLL "\033[2K"
#define clear_all() ( printf("\033[2J") );
#define clear_down() ( printf("\033[0J") );


#define scroll_down() \
		( printf("\033[%d;H\033M\033[%d;%dH", \
		(_wintop), (_wincur), (file->x_curs) ) )

#define scroll_up() \
		( printf("\033[%d;H\033D\033[%d;%dH", \
		(_winbot), (_wincur), (file->x_curs) ) )

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

/**** global variables *****/

int fpr;
Cmd *cmd_head;
Cmd_str* cmd_str_head;
int cmd_mask;
int global;
int need_goto;
line_t *copy_buffer_head = NULL;
line_t *copy_buffer_tail = NULL;

int _key_mode = 0;
int _switch_key_mode = 0;
int _cols, _rows;
int _wintop, _winbot, _wincur;
int _width;
int _h_scroll = 15;
int vert_jmp = 12;
int autoindent = 1;

int (*pfind)() = &simple_find;

int readonly;
int create_f;
int options;

buffer_t* last_file;

buffer_t* file;
int _tabs;
char msg[MAX_LINE];
char search_buf[MAX_LINE];
char s_buf[MAX_LINE];
char r_buf[MAX_LINE];
int key_pressed[256];
int key_stat[256];
//int key_pressed[256]={0};
FILE *erlog;


int
main( int argn, char *argv[])
{
 if (argn == 1)
	return 0;

 if (LOG == 1)
	erlog = fopen("error.txt", "w");
 else
	erlog = fopen("/dev/null", "w");
 fprintf(erlog, "*********************************\n");
 fflush(erlog);

 int i;
 char c;
 int (*pfunc)() = NULL;

 setvbuf(stdout, NULL, _IONBF, 0);
 setvbuf(stdin,  NULL, _IONBF, 0);

/* options */

 if (argv[1][0] != '-')
	options = 0;
 else {
	options = 1;
	for (i = 0; argv[1][i] != '\0'; i++)
		switch (argv[1][i]) {
		case 'r':
			readonly = 1;
			break;
		case 'c':
			create_f = 1;
			break;
		default:
			break;
		}
 }

//cmd_mask = 1;
cmd_mask = 0;

 _tabs = 4;

 int key_valid = 1;
 for(i = 0; i < 256; i++) {
 	key_pressed[i] = 0;
 	key_stat[i] = 0;
 }
 read_key_pressed();

 file = NULL;

 char com_key;
 char ch_col_ch;
 FILE* f;
 char buf[MAX_LINE];
 struct termios term, restore;

 copy_buffer_head = NULL;
 copy_buffer_tail = NULL;

 signal (SIGABRT, &sig_handler);
 signal (SIGTERM, &sig_handler);
 signal (SIGSEGV, &sigsegv_handler);
 atexit( &sig_handler );

 if (options == 1)
	i = 2;
 else
	i = 1;
 for( ; i < argn; i++)
 	open_file(argv[i]);

 create_f = 0;

 last_file = file;

 if(!file) {
 	printf("exiting: no buffers to edit\n");
	return 1;
 }
   
 while(file->prev)
	file = file->prev;

 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_iflag &= ~IXON;
 term.c_lflag &= ~(ICANON|ECHO);
 tcsetattr(0, TCSANOW, &term);

 getsize();
 _width = _cols;
// _width = _cols-60;

 _wintop = 1 + 1;
 _winbot = _rows - 3;
 _wincur = ( _winbot - _wintop ) / 2 + _wintop;

 set_scroll_win(_wintop, _winbot);

 vert_jmp = (_winbot - _wintop) / 2;

 moveyx(_wincur, 1);
 redraw_screen();
 make_commands();

 while( 1 ) {
loop:
	show_bar();
	need_goto = 0;
	com_key = getchar();
 
	message("");

	pfunc =	command_char(com_key);


	if( key_valid ) {
		key_pressed[com_key]++;
	}
	sprintf(msg, "%x[%c], %d(%d/%d)", com_key, com_key, key_pressed[com_key] + key_stat[com_key], key_stat[com_key], key_pressed[com_key]);
	key_valid = 1;

	if (need_goto == 1) goto loop;

	global = 0;
	if (_switch_key_mode == 1) {
		cmd_mask = ~cmd_mask;
		_switch_key_mode = 0;
		show_bar();
	}
	goto loop;
 }

/*
 exit:
 puts("exiting\n");
 tcsetattr(0, TCSANOW, &restore);
 freecommands();
 return 0;
*/
}


int
switch_mode (int flag) 
{
 cmd_mask = (cmd_mask & flag) == flag ? cmd_mask & ~flag : cmd_mask | flag;

 sprintf(msg, "switch... cmd_mask: %d", cmd_mask);
 message(msg);
 show_bar();
 return 0;
}


int
cmd_switch_alt_mode() 
{
 switch_mode (ALT_FLAG);
 show_bar();
 return 0;
}


int
cmd_switch_num_row_mode() 
{
 switch_mode (NUM_ROW_FLAG);
 show_bar();
 return 0;
}


int
cmd_switch_search_mode() 
{
 switch_mode (SRCH_FLAG);
 show_bar();
 return 0;
}

int
switch_global()
{
 global = 1;
 need_goto = 1;
 return 0;
}


int
invoke_command() {
 char buf[MAX_LINE];

 getstring(buf, "Command:");
 command(buf);
 return 0;
}


int
save_cur() 
{
 getpos(&file->y_cur, &file->x_cur);
 return 0;
}


int
rest_cur() 
{
 moveyx(file->y_cur, file->x_cur);
}


int
getsize () 
{
 struct winsize ws;
 ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
 _rows = ws.ws_row;
 _cols = ws.ws_col;
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
/* appeared */
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
/*
 if( file->cur_line->len == 0 ) {
	del_lines(file->cur_line, file->cur_line);
	return 0;
 }
*/
 del(&file->cur_line->str, file->offs+1, 1);
 print_cur_line();
 file->cur_line->len = strlen(file->cur_line->str);
 if(file->offs == file->cur_line->len)
	move_backward(1);
 return 0;
}


int
case_word()
{
 int i;
 for(i = file->offs; isalpha( file->cur_line->str[i] ); i++) {
 if('a' <= file->cur_line->str[i] && file->cur_line->str[i] <= 'z')
   file->cur_line->str[i] = toupper(file->cur_line->str[i]);
 else
   file->cur_line->str[i] = tolower(file->cur_line->str[i]);
 }
 print_cur_line();
 return 0;
}


int
case_backward()
{
 if(file->offs == 0) return 0;

 int i;
 for(i = file->offs - 1; (isalnum(file->cur_line->str[i]) || file->cur_line->str[i] == '_') && i >= 0; i--)
   file->cur_line->str[i] = toupper(file->cur_line->str[i]);
 print_cur_line();
 return 0;
}


int
case_char()
{
 if (isalpha( file->cur_line->str[file->offs] )) {
 if('a' <= file->cur_line->str[file->offs] && file->cur_line->str[file->offs] <= 'z')
   file->cur_line->str[file->offs] = toupper(file->cur_line->str[file->offs]);
 else
   file->cur_line->str[file->offs] = tolower(file->cur_line->str[file->offs]);
 }
 print_cur_line();
 return 0;
}


int
move_word_back()
{
 int tmp = 0;
 move_backward(1);  
 tmp = file->offs;
 if( isalnum(file->cur_line->str[file->offs]) )
	 while( isalnum(file->cur_line->str[tmp]) )
	   tmp--;
 else
	 while( !isalnum(file->cur_line->str[tmp]) )
	   tmp--;
 tmp++;
 move_first_col();
 move_forward(tmp+0);
 return 0; 
}


#if 0
char*
insert_str (char **pstr1, int n, char *str2)
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
 free(buf);
 return NULL;
}
#endif


char*
insert_ch (char **pstr, int n, char c)
{
 char *buf;
 int i;

 if ( n > strlen(*pstr) ) return NULL;

 if (!isprint(c) && c != '\t') return NULL;

 buf = malloc( strlen(*pstr) + 2 );
 strncpy(buf, *pstr, n);
 buf[n] = c;
 buf[n+1] = '\0';

 strcat (buf, *pstr+n);
 *pstr = realloc( *pstr, strlen(buf) + 1 );
 strcpy( *pstr, buf );
 free(buf);
 return NULL;
}

int
insert_indent()
{
 char ch;
 ch = getchar();
 line_t *line;

 for (line = file->copy_start_pos; line != file->copy_end_pos->forw; line = line->forw) {
 	insert_ch(&line->str, 0, ch);
	line->len++;
 }
 redraw_screen();
 position_cursor();
 return 0;
}


int
readstring(char* buf, int nmax) {
 int i, tmp, res, ret;
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
			if (*(buf+i-1) == '\t')
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
	case 'u'-0x60:
		tmp = i;
		i--;
		for(; (isalnum(*(buf+i)) || *(buf+i) == '_') && i >= 0; i--)
			*(buf+i) = toupper(*(buf+i));
		i = tmp;
		break;
	case '\t':
		*(buf+i) = ch;
		write(1, "\\t", 2);
		i++;
		break;
	default:
		*(buf+i) = ch;
		write(1, buf+i, 1);
		i++;
		break;
	}

 } while ( ch != 0 );
 return ret; 
}


int
getstring(char* buf, char* str) {
 save_cur();
 moveyx(_rows-1, 1);
 printf(CLL"%s", str);
 readstring(buf, MAX_LINE);
 rest_cur();
}


int
insert_string()
{
 int res, ret;
 char ch;
 char *tmp;
 int tmp_len = 0;
 int tmp_offs;

 setnocanon();
 setnoecho();

 do {
	res = read(0, &ch, 1);

	switch (ch) {
	case BCSP:
		ch = DEL;

	case DEL:
		if( file->offs ) {
			move_backward(1);
			del(&file->cur_line->str, file->offs + 1, 1);
			file->cur_line->len--;
		}
		break;

	case 0:
	case 27:
		ret = ch;
		ch = 0;
		break;

	case 10:
		file->cur_line->len = strlen(file->cur_line->str);
		tmp = file->cur_line->str + file->offs;
		tmp_len = strlen(tmp);
			file->cur_line = make_line_after(file->cur_line);
		file->offs = init_line(file->cur_line);
		
		file->cur_line->str = realloc(file->cur_line->str,
			strlen(file->cur_line->str) + tmp_len + 1 );
		strcat(file->cur_line->str, tmp);
		file->cur_line->backw->str =
			realloc( file->cur_line->backw->str, file->cur_line->backw->len - tmp_len+1 );
		file->cur_line->backw->str[file->cur_line->backw->len - tmp_len] = 0;
		tmp_offs = file->offs;
		file->offs = 0;
		file->x_curs = 1;
//		fpr = 1;
		move_forward(tmp_offs+1);
		if(file->cur_line->str[file->offs] != '\0')
		{
			force_forward(1);
			file->offs++;
		}

/*
		fprintf(erlog, "case 10: offs: %d, x_curs: %d\n", 
				file->offs, file->x_curs);
		fflush(erlog);
*/

//		moveyx(_wincur, file->x_curs);
	
		redraw_screen();
		/*
		sprintf(msg, "case 10: offs: %d, tmp_offs: %d, x_curs: %d\n", 
					file->offs, tmp_offs, file->x_curs);
		message(msg);
		*/
		break;

 	case 'u'-0x60:
 		case_backward();
 		break;

	default:
 		if (!isprint(ch) && ch != '\t')
			break;
		insert_ch(&file->cur_line->str, file->offs, ch);
		if(ch == '\t')
			file->x_curs = nexttab1();
		else
			file->x_curs++;
		file->offs++;
		file->cur_line->len++;
		break;
		}
	 if(file->x_curs <= _width) {
//		moveyx(_wincur, file->x_curs);
	 }
	 else {
		file->cur_line->x -= _h_scroll;
//		print_cur_line();
		file->x_curs -= _h_scroll;
//		moveyx(_wincur, file->x_curs);
	 }
//sleep(1);
	print_cur_line();
	moveyx(_wincur, file->x_curs);

	show_bar();

 } while ( ch != 0 );
 file->cur_line->len = strlen( file->cur_line->str);
 return ret; 
}


int
insert_string_after()
{
 int ret;
 if (*file->cur_line->str == '\0') {
	ret = insert_string();
	return 0;
 }
 save_cur();
// move_forward(1);
 if (file->cur_line->str[file->offs] == '\t')
	file->x_curs = nexttab1();
 else
	file->x_curs++;//TAB??
 file->offs++;
 moveyx(file->y_cur, file->x_curs);
 ret = insert_string();
 file->offs--;
 file->x_curs--;
 moveyx(file->y_cur, file->x_curs);
 return ret;
}


int
cmd_edit_new_line_before()
{
 file->cur_line = make_line_before(file->cur_line);
 init_line(file->cur_line);
 reset();
 redraw_screen();
 insert_string();
 redraw_screen();
 return 0;
}


int
cmd_edit_new_line_after()
{
 file->cur_line = make_line_after(file->cur_line);
 init_line(file->cur_line);
 reset();
 move_last_col();
 redraw_screen(); //need partial scroll
// insert_string();
 cmd_edit_line_end();
 redraw_screen();
 return 0;
}


int
cmd_edit_line_begin()
{
 move_first_col();
 insert_string();
 return;
}


int
cmd_edit_line_end()
{
 move_last_col();
 insert_string_after();
 return;
}


int
concat_lines()
{
 char *buf;
 line_t *tmp;
 
 buf = malloc( strlen(file->cur_line->str) + strlen(file->cur_line->forw->str) + 1 );
 strcpy(buf, file->cur_line->str);
 strcat(buf, file->cur_line->forw->str);
 free(file->cur_line->str);
 file->cur_line->str = buf;
 
 destroy_line(file->cur_line->forw);
 
 redraw_screen();
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
cmd_make_line_before()
{
 file->cur_line = make_line_before(file->cur_line);
 init_line(file->cur_line);
 reset();
 redraw_screen();
}


int
cmd_make_line_after()
{
 file->cur_line = make_line_after(file->cur_line);
 init_line(file->cur_line);
 reset();
 redraw_screen();
}


int
init_line( line_t *ln )
{
 int i;

 if (ln->backw && autoindent) {
	for (i = 0; isspace(ln->backw->str[i]); i++);
	ln->str = malloc(i + 1);
	ln->str[i] = '\0';
	strncpy(ln->str, ln->backw->str, i);
 }
 else
 {
	ln->str = malloc(1);
	*(ln->str) = '\0';
	ln->len = 0;
 }
 ln->x = 1;
 if (ln->backw)
	ln->color = ln->backw->color;
 else
	ln->color = 0;
 return strlen(ln->str);
}


int 
make_line()
{
 int i;
 line_t *prev, *next;

 if( file->cur_line == NULL ) {
	file->cur_line = malloc( sizeof(line_t) );
	file->cur_line->backw = NULL;
	file->cur_line->str = malloc(1);
	*(file->cur_line->str) = '\0';
	return 0;
 }

 if( file->cur_line->backw == NULL) {
	next = file->cur_line;
	file->cur_line = malloc( sizeof(line_t) );
	next->backw = file->cur_line;
	file->cur_line->backw = NULL;
	file->cur_line->forw = next;
	file->cur_line->str = calloc(1,1);
	file->cur_line->len = 0;
	redraw_screen();
	moveyx(_wincur, 1);
	return 0;
 }


 prev = file->cur_line->backw;
 next = file->cur_line;
 prev->forw = malloc( sizeof(line_t) );
 file->cur_line = prev->forw;
 next->backw = file->cur_line;

 file->cur_line->backw = prev;
 file->cur_line->forw = next;
 file->cur_line->str = calloc(1,1);
 file->cur_line->len = 0;
 
 redraw_screen();
 moveyx(_wincur, 1);

 return 0;
}


int
change_char()
{
 char c = getchar();

 file->cur_line->str[file->offs] = c;
 print_cur_line();
}


int
insert_space()
{
 insert_ch( &file->cur_line->str, file->offs, ' ');
 file->cur_line->len++;
 print_cur_line();
 return 0;
}


int
insert_char()
{
 char ch = getchar();

 insert_ch( &file->cur_line->str, file->offs, ch);
 file->cur_line->len++;
 print_cur_line();
 position_cursor();
 return 0;
}


int
insert_char_next()
{
 char ch = getchar();
 save_cur();
 moveyx(file->y_cur, file->x_cur + 1);
 file->offs++;
 insert_ch( &file->cur_line->str, file->offs, ch);
 file->cur_line->len++;
 print_cur_line();
 position_cursor();
 move_forward(1);
 return 0;
}


int 
read_line(FILE* file, char* buf)
{
 int fd, i;
 char c;

 c = fgetc(file);
 for( i = 0; c != '\n' && c != EOF && c != '\0' && c != '\r'; i++ ) {
	buf[i] = c;
	c = fgetc(file);
 }
 buf[i] = '\0';

 if (c == '\r')
	c = fgetc(file);
	if (c != '\n')
		ungetc(c, file);
 return c;
}


line_t*
read_file(FILE* file)
{
 char buf[MAX_LINE] = {0};
 line_t *lines = NULL;

 while( read_line(file, buf) != EOF ) {
	lines = make_line_after(lines);
	lines->len = strlen( buf );
	lines->str = malloc( lines->len + 1 );
	strcpy( lines->str, buf);
	lines->x = 1;
	lines->color = 0;
 }
	lines = make_line_after(lines);
	lines->len = strlen( buf );
	lines->str = malloc( lines->len + 1 );
	strcpy( lines->str, buf);
	lines->x = 1;
	lines->color = 0;

	while( lines->backw )
	   lines = lines->backw;

 return lines;
}


int
open_file(char *name) {
 int fd;
 int flags;
 FILE *f;
 int file_len;
 buffer_t *tmp_prev, *tmp_next;

 if (create_f == 1)
 	fd = open(name, O_RDWR | O_CREAT, 0666);
 else
 	fd = open(name, O_RDWR);

 if ( fd < 0 ) {
	perror("open");
	return 1;
 }


 if (file) {
	tmp_prev = file;
	tmp_next = file->next;
	file->next = malloc( sizeof(buffer_t));
	file = file->next;
	file->prev = tmp_prev; 
	file->next = tmp_next; 
	tmp_prev->next = file;
	if (tmp_next)
	tmp_next->prev = file;
 }
 else {
	file = malloc(sizeof(buffer_t));
	file->prev = NULL;
	file->next = NULL;
 }

 file->offs = 0;
 file->x_curs = 1;
 file->fd = fd;

 strcpy(file->filename, name);
 f = fdopen(fd, "r+");
 file->f = f;

 make_backup_copy(name);

 file_len = lseek(fd, 0, SEEK_END);
 lseek(fd, 0, SEEK_SET);

 if( file_len )
	file->cur_line = read_file(f);
 else
	file->cur_line = NULL;
 return 0;
}


int 
close_file() {
 buffer_t *tmp;

 if(!file)
	return 0;

 remove_backup(file->filename);
 if(!file->prev && !file->next) {
 	free_lines(get_head(), NULL);
 	free(file);
	quit();
	return 0;
 }

 tmp = (file->prev) ? file->prev : file->next;
 free_lines(get_head(), NULL);
 if(file->prev)
 file->prev->next = file->next;
 if(file->next)
 file->next->prev = file->prev;
 free(file);
 file = tmp;

 redraw_screen();
 return 0;
}


int
reopen_file () {
 rewind(file->f);
 free_lines(get_head(), NULL);
 file->cur_line = read_file(file->f);
 redraw_screen();
 message("reopen_file()");
 return 0;
}


int
message(char* s)
{
 print_line_x(1, 1, s);
 return 0;
}


int
gmessage(char* s)
{
 print_line_x(1, 1, s);
 getchar();
 return 0;
}


int 
write_buf(int fd)
{
 if (readonly) {
	message("readonly");
	return 1;
 }
 message("writing...");
 lseek(fd, 0, SEEK_SET);
 int i, res;
 char buf[MAX_LINE];
// char buf[500];
 line_t *line;

 FILE *f = fdopen(fd, "r+");
 if (!f) {
 perror("fdopen");
   return 1;
 }
 ftruncate(fd, 0);
 

 line = get_head();

 if (!line) {
 message("line == NULL");
//	fclose(f);
	return 0;
 }

 while( line->forw ) 
 {
	strcpy( buf, line->str);
	for(i = 0; buf[i] != '\0'; i++)
	   fputc(buf[i], f);
	fputc('\n', f);
	line = line->forw;
 }
 strcpy( buf, line->str);
 for(i = 0; buf[i] != '\0'; i++)
	fputc(buf[i], f);

 fflush(f);
// fclose(f);
 message("writing... done");
}


int
save_file() {
 if(file)
 write_buf(file->fd);
 return 0;
}


int
save_all() {
 if(!file) return 1;
 
 buffer_t *tmp = file;

 while(file->prev)
	file = file->prev;

while(file) {
	write_buf(file->fd);
//	sprintf(msg, "writing %s...", file->filename);gmessage(msg);
	file = file->next;
}
 file = tmp;
 return 0;
}


int
make_backup_copy(char* fname)
{
 int fd1, fd2;
 int ret = -1;
 char buf[BUFSIZE];
 char path[BUFSIZE]={0};
 int len = strlen(fname);
 
#ifndef BAK_CUR_FOLDER
 strcpy(path, getenv("HOME"));
 strcat(path, "/.med/");
 if (access(path, F_OK) == -1)
 mkdir (path, 0777);
#endif
  
 strcat(path, fname);
 strcat(path, ".mbk"); 
 
 fd1 = open(fname, O_RDONLY);
 fd2 = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
 if (fd2 < 0) {
	sprintf(msg, "make_backup_copy: failed to open %s", path); 
	message(msg);
 }
 while(ret != 0) {
   ret = read(fd1, buf, BUFSIZE);
   write(fd2, buf, ret);
 }
 close(fd1);
 close(fd2);
}


int
remove_backup(char* fname) {
 char path[BUFSIZE]={0};
 
#ifndef BAK_CUR_FOLDER
 strcpy(path, getenv("HOME"));
 strcat(path, "/.med/");
#endif
 
 strcat(path, fname);
 strcat(path, ".mbk");
 remove(path);
 return 0;
}


int
cur_up()
{
 if(_wincur != _wintop)
 _wincur--;
 redraw_screen();
 moveyx(_wincur, 1);
}


int
cur_down()
{
 if(_wincur != _winbot)
 _wincur++;
 redraw_screen();
 moveyx(_wincur, 1);
}

int
move_down(int n)
{
 int i;
 for( i = 0; i < n && file->cur_line->forw; i++)
	file->cur_line = file->cur_line->forw;
 return 0;
}


int
move_down_half_screen()
{
// move_down((_winbot - _wintop) / 2);
 move_down(vert_jmp);
 redraw_screen();
 position_cursor();
 return 0;
}


int
move_up(int n)
{
 int i;
 for( i = 0; i < n && file->cur_line->backw; i++)
	file->cur_line = file->cur_line->backw;
 return 0;
}


int
move_up_half_screen()
{
// move_up((_winbot - _wintop) / 2);
 move_up(vert_jmp);
 redraw_screen();
 position_cursor();
 return 0;
}


int
move_down_smooth(int n)
{
 int i;

 for( i = 0; i < n && file->cur_line->forw; i++) {
//	reset();
	scroll_up();
	file->cur_line = file->cur_line->forw;
	print_bottom_line();
	position_cursor();
 }
 return 0;
}


int
move_up_smooth(int n)
{
 int i;

 for( i = 0; i < n && file->cur_line->backw; i++) {
//	reset();
	scroll_down();
	file->cur_line = file->cur_line->backw;
	print_top_line();
	position_cursor();
 }
 return 0;
}


int
move_down_one()
{
 move_down_smooth(1);
 return 0;
}


int
move_up_one()
{
 move_up_smooth(1);
 return 0;
}


int
move_up_block()
{
 do {
	if (file->cur_line->backw == NULL) return 0;
	file->cur_line = file->cur_line->backw; 
 } while (!(isalpha(file->cur_line->str[0]) || file->cur_line->str[0] == '_'
			|| file->cur_line->str[0] == '{'));

 redraw_screen();
 return 0;
}


int
move_down_block()
{
 do {
	if (file->cur_line->forw == NULL) return 0;
	file->cur_line = file->cur_line->forw; 
 } while (!(isalpha(file->cur_line->str[0]) || file->cur_line->str[0] == '_'
			|| file->cur_line->str[0] == '}'));

 redraw_screen();
 return 0;
}


int
move_first()
{
 while( file->cur_line->backw != NULL )
	file->cur_line = file->cur_line->backw;
 redraw_screen();
 return 0;
}


int
move_last()
{
 while( file->cur_line->forw != NULL )
	file->cur_line = file->cur_line->forw;
 redraw_screen();
 return 0;
}


int
position_cursor()
{
 int tmp = file->x_curs;
 move_first_col();
 while(file->x_curs < tmp && file->offs + 1 < file->cur_line->len)
 move_forward(1);
}


int
reset()
{
 file->offs = 0;
 file->x_curs = 1;
 return 0;
}


int
move_first_col()
{
 file->offs = 0;
 file->x_curs = 1;
 file->cur_line->x = 1;
 moveyx(_wincur, 1);
 print_cur_line();
 return 0;
}


int
move_last_col()
{
// move_forward(file->cur_line->len);
 move_forward(cur_len());
 return 0;
}


int
move_forward(int n)
{
 int i;
 int need_reprint = 0;

 file->cur_line->len = cur_len();

 if(fpr == 1) {
		fprintf(erlog, "mv_forw: n: %d\n", n);
		fprintf(erlog, "mv_forw: cur_line->len: %d\n", file->cur_line->len);
		fflush(erlog);
 }

 for( i = 0; i < n; i++ )
 if ( file->offs + 1 < file->cur_line->len ) { 

 if(fpr == 1) {
		fprintf(erlog, "mv_forw: offs: %d, x_curs: %d char: %c\n", file->offs, file->x_curs,
		file->cur_line->str[file->offs]);
		fflush(erlog);
 }
	 if( *(file->cur_line->str + file->offs ) == '\t' )
		file->x_curs = nexttab1();
	 else
		file->x_curs++;

	 while (file->x_curs > _width) {
		 file->cur_line->x -= _h_scroll;
		 file->x_curs -= _h_scroll;
		 need_reprint = 1;
	 }
	 file->offs++;
 }

 if(need_reprint == 1)
   print_cur_line();
 moveyx(_wincur, file->x_curs);
 return 0;
}

int
force_forward(int n)
{
 int i;
 int need_reprint = 0;

 file->cur_line->len = cur_len();

 if(fpr == 1) {
		fprintf(erlog, "force_forw: n: %d\n", n);
		fprintf(erlog, "force_forw: cur_line->len: %d\n", file->cur_line->len);
		fflush(erlog);
 }

 for( i = 0; i < n; i++ )
/* if ( file->offs + 1 < file->cur_line->len ) */{ 

 if(fpr == 1) {
		fprintf(erlog, "force_forw: offs:%d, x_curs:%d char:%d\n", file->offs, file->x_curs,
		file->cur_line->str[file->offs]);
		fflush(erlog);
 }
	 if( *(file->cur_line->str + file->offs ) == '\t' )
		file->x_curs = nexttab1();
	 else
		file->x_curs++;

/*
	 while (file->x_curs > _width) {
		 file->cur_line->x -= _h_scroll;
		 file->x_curs -= _h_scroll;
		 need_reprint = 1;
	 }
	 file->offs++;
*/
 }

/*
 if(need_reprint == 1)
   print_cur_line();
*/
 moveyx(_wincur, file->x_curs);
 return 0;
}


int
move_forward_one()
{
 move_forward(1);
 return 0;
}


int
move_forward_hs()
{
 move_forward(10);
 return 0;
}


int
move_backward1(int n)
{
 int i, tmp;
 int need_reprint = 0;

 for( tmp = file->offs, i = 0; i < n && tmp != 0; tmp--, i++ )
	;
 file->offs = 0;
 file->x_curs = 1;

/* move forward */
 while( file->offs != tmp ) 
 if ( file->offs + 1 < file->cur_line->len ) { 
	 if( *(file->cur_line->str + file->offs + 0) == '\t' ) {
		file->x_curs = nexttab1();
	 }
	 else
		file->x_curs++;

	 while (file->x_curs > _width) {
//		 file->cur_line->x -= _h_scroll;
		 file->x_curs -= _h_scroll;
//		 need_reprint = 1;
	 }
	 file->offs++;
 }
gmessage("left");
message("\t\t");

	 while (file->x_curs < 1 ) {
gmessage("< 1");
message("\t\t");
		 file->cur_line->x += _h_scroll;
		 file->x_curs += _h_scroll;
		 need_reprint = 1;
	 }

// if(need_reprint == 1)
   print_cur_line();
 moveyx(_wincur, file->x_curs);
 return 0;
}


int
move_backward(int n)
{
 int i, tmp = file->offs;
 for( i = 0; i < n && file->offs != 0; i++ ) {
	move_first_col();
	move_forward(--tmp);
 }
 return 0;
}


int
move_backward_one()
{
 move_backward(1);
 return 0;
}


int
move_backward_hs()
{
 move_backward(10);
 return 0;
}


int
nexttab()
{
 int i;
 int y, x;
 getpos(&y, &x);
 i = (x - 1) / _tabs * _tabs + _tabs + 1;
 return i;
}


int
nexttab3(int x, int o)
{
 int i;
 i = (x - 1) / _tabs * _tabs + _tabs + 1;
 return i + o;
}


int
nexttab1()
{
 return (file->x_curs + (file->cur_line->x - 1) - 1) / _tabs * _tabs + _tabs + 1
		- (file->cur_line->x - 1);
}


int
get_bow()
{
 int i, tmp;
 char *str;

 str = file->cur_line->str;
 i = tmp = file->offs;
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

 str = file->cur_line->str;
 i = tmp = file->offs;
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
 int i = 0;
 i = get_bow();
 move_first_col();
 move_forward(i);
}


int
move_eow()
{
 int i = 0;
 i = get_eow();
 move_first_col();
 move_forward(i);
}


int
del_word()
{

 int i, to;

 to = ( isalnum( *(file->cur_line->str + file->offs) ) ) ? get_eow() : get_bow();

 del( &file->cur_line->str, file->offs + 1, to - (file->offs + 0) );
 file->cur_line->len = strlen(file->cur_line->str);
 print_cur_line();

return 0;
}


int
cmd_del_word()
{
 del_word();
 if( *(file->cur_line->str + file->offs) == 0)
	move_backward(1);
 return 0;
}


int
cmd_change_word()
{
 del_word();
 insert_string();
 if( *(file->cur_line->str + file->offs) == 0)
	move_backward(1);
 return 0;
}


int
print_top_line()
{
 int i;
 line_t* pr_line;
 pr_line = file->cur_line;
 for( i = 0; i < _wincur - _wintop; i++ ) {
	if (pr_line->backw == NULL) return;
	pr_line = pr_line->backw; 
 }
// print_color_line(pr_line, _wintop);
 print_line_x_color(pr_line->x, _wintop, pr_line->str, pr_line->color);
}


int
print_bottom_line()
{
 int i;
 line_t* pr_line;
 pr_line = file->cur_line;
 for( i = 0; i < _winbot - _wincur; i++ ) {
	if (pr_line->forw == NULL) return;
	pr_line = pr_line->forw; 
 }
// print_color_line(pr_line, _winbot);
 print_line_x_color(pr_line->x, _winbot, pr_line->str, pr_line->color);
}


int
print_line(int line, char *str)
{
 int i = 0;
 int y, x;
 getpos(&y, &x);
 moveyx(line, 1);
 write(1, "\033[2K", 4);
 int cur = 1;
 while(cur <= _width && str[i] != 0) {
	if (str[i] == '\t')
	   cur = nexttab();
	else {
/*
sprintf(msg, "%c %d %d", *(str+i), i, cur );
message(msg);
*/
	   write(1, (str+i), 1);
	   cur++;
	}
	moveyx(line, cur);
	i++;
 }
 moveyx(y, x);
}


int
print_line_x(int o, int line, char *str)
{
 int i = 0;
 int y, x;
 int tmp;
 int ch_len; //mbrlen
 getpos(&y, &x);
 moveyx(line, 1);
 printf(CLL);

/*
 moveyx(X_MODE_, 1);
 printf(CLL);
*/

 tmp = o;
 while(o <= _width && str[i] != 0) {
	   if (o > 0 )
	   moveyx(line, o);

/*
	else
	   moveyx(X_MODE_, _cols + o);
*/

	if (str[i] == '\t')
	   o = nexttab3( o - (tmp-1), tmp - 1);
 	else {
	   if (o > 0 )

	   ch_len = mbrlen(str+i, MB_CUR_MAX, NULL);
//	   write(1, (str+i), ch_len);
	   write(1, (str+i), 1);
	   o++;
	}
	i += ch_len;
 }
 moveyx(y, x);
}


int
print_line_x_color(int o, int line, char *str, int col)
{
 int i = 0;
 int y, x;
 int tmp;
 getpos(&y, &x);
 moveyx(line, 1);
 printf(CLL);
 printf("\x1B[%dm", col); /* escape codes */


/*
 moveyx(X_MODE_, 1);
 printf(CLL);
*/


 tmp = o;
 while(o <= _width && str[i] != 0) {
	if (o > 0 )
	   moveyx(line, o);


/*
	else
	   moveyx(X_MODE_, _cols + o);
*/


	if (str[i] == '\t')
		o = nexttab3( o - (tmp-1), tmp - 1);
	else {
		if (o > 0 )
		{
	 		write(1, (str+i), 1);
/*
			sprintf(msg, "%d - %c", *(str+i+1), *(str+i+1));
			gmessage(msg);
*/
		}
		o++;
/*
		sprintf(msg, "o: %d", o);
		gmessage(msg);
*/
	}
	i++;
/*
	sprintf(msg, "i: %d", i);
	gmessage(msg);
*/
 }
 moveyx(y, x);
/*
 sprintf(msg, "y: %d - x: %d", y, x);
 gmessage(msg);
*/
 printf("\x1B[0m"); /* escape codes */
}


int
print_color_line(line_t* l, int pos)
{
// print_line_x_color(l->x, pos, l->str, l->color);
 return 0;
}


int
print_cur_line()
{

 print_line_x_color(file->cur_line->x, _wincur, file->cur_line->str,
			file->cur_line->color);

// print_color_line(file->cur_line, _wincur);
 return 0;
}


int
redraw_screen()
{
 int i, n;
 line_t* ln;

 clear_all();

 if (!file->cur_line)
 return 1;

 ln = file->cur_line;
 calculate_all_lengths();

 for( n = _wincur; n > _wintop && ln->backw != NULL; ) {
	ln = ln->backw;
	n--;
 }
 for( i = 0; i < _winbot - n - 0  && ln->forw != NULL; i++) {

/* colored markers */
/*
	if (ln == file->copy_start_pos) ln->color = 31;
	if (ln == file->copy_end_pos) ln->color = 32;
	if (ln == file->copy_start_pos && ln == file->copy_end_pos)
	ln->color = 33;
	if (ln != file->copy_start_pos && ln != file->copy_end_pos)
	ln->color = 0;
*/
/********		*/

	print_line_x_color(ln->x, n+i, ln->str, ln->color);
//	print_color_line(ln, n+i); /* seg fault*/
sprintf(msg, "%d", n+i);
//getchar();
//gmessage(msg);
	ln = ln->forw;
/*
sprintf(msg, "%s", ln->str);
gmessage(msg);
*/
 }
 print_line_x_color(ln->x, n+i, ln->str, ln->color);
// print_color_line(ln, n+i);
 moveyx(_wincur, file->x_curs);
}

char* 
find_ss (char* s, char *t) {	//skip spaces
/* very old function */

if (s == NULL) 
{
	printf("find_s: first argument is NULL\n");
	return NULL;
}

if  (t == NULL) 
{
	printf("find_s: second argument is NULL\n");
	return NULL;
}

//print_ascii(s);
int i = 0;
int j=0;
int k=0;
int sp = 0;

	while( *(t+j) != '\0' )
	{
		i=0;
		k=j;
		while( 1 ) 
		{
			if( *(s+i) == ' ' || *(s+i) == '\t') 
			{
				i++;
				continue;
			}
			if( *(t+k) == ' ' || *(t+k) == '\t') 
			{
				k++;
				sp++;
				continue;
			}
			if( *(s+i+0) == '\0' ) {
					sprintf(msg, "%d, %d", j, sp);
				return (t+k+sp);///!!!! j
			}
/*
				if( *(t+k) == '\0' ) {
					return NULL;//???
				}
*/
//			if ( *(s+i) != *(t+j+sp) )
			if ( *(s+i) != *(t+j) )
				break;
			else
			{
//printf(" %c %c ", *(s+i), *(t+k) ); getchar();
				i++;
				k++;
			}
			if( *(s+i+0) == '\0' ) {
//				return (t+j+sp);///!!!! j
//				sprintf(msg, "%d, %d", j, sp);
				return (t+j);///!!!! j
			}

		}
		j++;
	}

return NULL;
}


int
simple_find()
{
 int i;
 char* res = NULL;
 line_t* search_pos;

 pfind = &simple_find;

 moveyx(_wincur, 1);
 if (file->cur_line->forw == NULL) return 1;
 search_pos = file->cur_line->forw;
 while(res == NULL && search_pos != NULL) {
//	res = strcasestr(search_pos->str, s_buf);
	res = strstr(search_pos->str, s_buf);
//	res = find_ss(s_buf, search_pos->str);

	if ( res != NULL )
		break;
	search_pos = search_pos->forw;
 }
	if (res == NULL) {
		message("no matching");
		return 1;
 }
 file->cur_line = search_pos;

 redraw_screen();
 reset();
 message(search_pos->str);
 return 0;
}


#if 0
/*
int
simple_find()
{
 int i;
 char* res = NULL;
 line_t* search_pos;

 pfind = &simple_find;

 moveyx(_wincur, 1);
 if(file->cur_line->forw == NULL) return 1;
 search_pos = file->cur_line;
 while(search_pos != NULL) {
//	res = strcasestr(search_pos->str, s_buf);
	res = strstr(search_pos->str + file->offs + 1, s_buf);
//	res = find_ss(s_buf, search_pos->str);

	if ( res == NULL )
		search_pos = search_pos->forw;
 }
	if (res == NULL) {
		message("no matching");
		return 1;
 }
 file->cur_line = search_pos;

 redraw_screen();
 reset();
	move_first_col();
	move_forward(res - file->cur_line->str+0);	

// message(search_pos->str);
//	gmessage(msg);
 sprintf(msg, "%d\n", res-search_pos->str);

 return 0;
}

*/


/*
int
simple_find()
{
 if (file->cur_line == NULL)
	return 0;

 int len = strlen(s_buf);
 line_t *line = file->cur_line;
 int o = file->offs;
 char *pos;

 pfind = &simple_find;

 if( len )
 for ( ; line != NULL; line = line->forw, o = -1)
 for (pos = line->str + o + 1; pos = find_ss (pos, s_buf); pos += len) {

/*
	if ((isalnum (pos[-1]) && pos != line->str) || isalnum (pos[len])
	|| (pos[-1] == '_' && pos != line->str) || pos[len] == '_')
		continue;
*/

	file->cur_line = line;

	redraw_screen();
	move_first_col();
	move_forward(pos - line->str);	
	message(msg);

	return 0;
 }
 return 0;
}
*/
#endif


int
cmd_simple_find()
{
 getstring(s_buf, "Find:");
// if (s_buf[0])
// move_first();
 simple_find();
 return 0;
}


int
find_name()
{
 if (file->cur_line == NULL)
	return 0;

 int len = strlen(s_buf);
 line_t *line = file->cur_line;
 int o = file->offs;
 char *pos;

 pfind = &find_name;

 if( len )
 for ( ; line != NULL; line = line->forw, o = -1)
 for (pos = line->str + o + 1; pos = strstr (pos, s_buf); pos += len) {

	if ((isalnum (pos[-1]) && pos != line->str) || isalnum (pos[len])
	|| (pos[-1] == '_' && pos != line->str) || pos[len] == '_')
		continue;

	file->cur_line = line;

	redraw_screen();
	move_first_col();
	move_forward(pos - line->str);	

	return 0;
 }
 return 0;
}


int
cmd_find_name()
{
 getstring(s_buf, "Find:");
 find_name();
 return 0;
}


int 
find_beginning()
{
 int i;
 int res = -1;
 line_t* search_pos;

 pfind = &find_beginning;

 moveyx(_wincur, 1);
 if (!file->cur_line->forw) return;
 search_pos = file->cur_line->forw;
 while(res != 0 && search_pos != 0) {
	res = strncmp(search_pos->str, s_buf, strlen(s_buf));
	if ( res == 0 )
	   break;
	search_pos = search_pos->forw;
 }
  if(res != 0) {
  message("no matching");
  return res;
 }
 file->cur_line = search_pos;

 redraw_screen();
 reset();
 message(search_pos->str);
 return res;
}


int
cmd_find_beginning()
{
 getstring(s_buf, "Find:");
 if (s_buf[0])
	move_first();

 find_beginning();
 return 0;
}


int
refind()
{
 if(s_buf[0] == '\0')
 	return;
 pfind();
 return 0;
}


int
alt_refind()
{
 if(s_buf[0] == '\0')
 	return;
 pfind = (pfind == &simple_find) ? &find_beginning : &simple_find;
 pfind();
 return 0;
}


int
search_and_replace(line_t* start_srch, line_t* end_srch, int name)
{
 int i;
 int len, s_len, r_len, o;
 char *pos;
 line_t *line = start_srch;
 static char buf[MAX_LINE];

 s_len = strlen(s_buf);
 r_len = strlen(r_buf);

 pos = line->str;
 if( s_len )
 for( ;line != end_srch->forw; line = line->forw )

 for (pos = line->str; pos = strstr( pos, s_buf );) {
	o = pos - line->str;

	if (name == 1)
	if ((isalnum (pos[-1]) && pos != line->str) || isalnum (pos[s_len])
		|| pos[-1] == '_' || pos[s_len] == '_') {
		pos += s_len;
		continue;
	}

	strncpy(buf, line->str, o);
	buf[o] = 0;
	strcat(buf, r_buf);
	strcat(buf, pos + s_len);
	line->len = strlen(buf);
	line->str = realloc(line->str, line->len + 1);
	strcpy(line->str, buf);

	pos = line->str + o;
	pos += r_len;
   }
 return 0;
}


int
cmd_replace_whole()
{
 getstring(s_buf, "Search:");
 getstring(r_buf, "Replace:");

 search_and_replace (get_head(), get_tail(), 0);
 redraw_screen();
 return 0;
}

int
cmd_replace_global()
{
 buffer_t *tmp = file; 
 message("GLOBAL replace");
 getstring(s_buf, "Search:");
 getstring(r_buf, "Replace:");
 for(file = file; file->prev; file = file->prev);
 for(;file; file = file->next) {
	if (file->cur_line == NULL) continue;
 	search_and_replace(get_head(), get_tail(), 0);
 }
 file = tmp;
 redraw_screen();
 return 0;
}

int
cmd_replace_global2()
{
 buffer_t *tmp = file; 
 message("GLOBAL replace name");
 getstring(s_buf, "Search:");
 getstring(r_buf, "Replace:");
 for(file = file; file->prev; file = file->prev);
 for(;file; file = file->next) {
	if (file->cur_line == NULL) continue;
 	search_and_replace(get_head(), get_tail(), 1);
 }
 file = tmp;
 redraw_screen();
 return 0;
}


int
cmd_replace_marks()
{
 getstring(s_buf, "Search:");
 getstring(r_buf, "Replace:");
 search_and_replace (file->copy_start_pos, file->copy_end_pos, 0);
 redraw_screen();
 return 0;
}


int
cmd_replace_whole2()
{
 getstring(s_buf, "Search:");
 getstring(r_buf, "Replace:");

 search_and_replace (get_head(), get_tail(), 1);
 redraw_screen();
 return 0;
}


int
cmd_replace_marks2()
{
 getstring(s_buf, "Search:");
 getstring(r_buf, "Replace:");
 search_and_replace (file->copy_start_pos, file->copy_end_pos, 1);
 redraw_screen();
 return 0;
}


int
cmd_repeat_repl_whole()
{
 search_and_replace (get_head(), get_tail(), 0);
 redraw_screen();
 return 0;
}


int
cmd_repeat_repl_marks()
{
 search_and_replace (file->copy_start_pos, file->copy_end_pos, 0);
 redraw_screen();
 return 0;
}


int
cmd_repeat_repl_whole2()
{
 search_and_replace (get_head(), get_tail(), 1);
 redraw_screen();
 return 0;
}


int
cmd_repeat_repl_marks2()
{
 search_and_replace (file->copy_start_pos, file->copy_end_pos, 1);
 redraw_screen();
 return 0;
}


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
 dest->x = source->x;
 dest->color = 0;
 source = source->forw;
 *buf_head = dest;
 while( source != end->forw ) {
	dest = make_line_after(dest);
	dest->str = malloc( strlen( source->str ) + 1 );
	strcpy( dest->str, source->str );
	dest->len = source->len;
	dest->x = source->x;
	dest->color = 0;
	source = source->forw;
 }
 *buf_tail = dest;
 return 0;
}


int
insert_lines(line_t *place, line_t *buf_head, line_t *buf_tail)
{
// if (!place) insert_lines_before(get_head(), buf_head, buf_tail);//bad recursion
 if( !place || !buf_head || !buf_tail ) return 1;

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
// if (!place) insert_lines(get_tail(), buf_head, buf_tail);//bad recursion
 if( !place || !buf_head || !buf_tail ) return 1;

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
set_mark1()
{
 file->use_mark1 = file->cur_line;
 return 0;
}


int
set_mark2()
{
 file->use_mark2 = file->cur_line;
 return 0;
}


int
auto_set_mark()
{
 int i;
 set_mark2();
 i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);
 return i;
}


int
set_mark(int n)
{
 if(n < 0 || n > 9)
	return 1;

 file->marks[n] = file->cur_line;
 return 0;
}


int
cmd_set_mark()
{
 char c = getchar();
 set_mark(c - '0');
 return 0;
}


int
go_mark_number(int n) {
 if (n < 0 || n > 9) return 1;

// file->cur_mark = n;


 line_t *line = get_head();

 while (line != file->marks[n] && line != NULL)
	line = line->forw;
 if (line == file->marks[n] && file->marks[n] != NULL ) {
	file->last_mark = file->cur_line;
	file->marks[10] = file->cur_line;
	file->cur_line = line;
	file->cur_mark = file->cur_line;
	file->marks[11] = file->cur_line;
	redraw_screen();
 }
 return 0;
}


int
cmd_go_mark_number()
{
 char c = getchar();
 go_mark_number(c - '0');
 return 0;
}


int
go_mark_1() {
 go_mark_number(1); 
 return 0;
}


int
go_mark_2() {
 go_mark_number(2); 
 return 0;
}


int
go_mark_3() {
 go_mark_number(3); 
 return 0;
}


int
go_mark_4() {
 go_mark_number(4); 
 return 0;
}


int
go_mark_5() {
 go_mark_number(5); 
 return 0;
}


int
go_mark_6() {
 go_mark_number(6); 
 return 0;
}


int
go_mark_7() {
 go_mark_number(7); 
 return 0;
}


int
go_mark_8() {
 go_mark_number(8); 
 return 0;
}


int
go_mark_9() {
 go_mark_number(9); 
 return 0;
}


int
go_mark_next() {

 line_t *line = file->cur_line;

 do
	line = line->forw;
 while ((line != file->marks[0] 
 		&& line != file->marks[1] 
 		&& line != file->marks[2] 
 		&& line != file->marks[3] 
 		&& line != file->marks[4] 
 		&& line != file->marks[5] 
 		&& line != file->marks[6] 
 		&& line != file->marks[7] 
 		&& line != file->marks[8] 
 		&& line != file->marks[9])
		&& line != NULL);

 if ((line == file->marks[0] 
 		|| line == file->marks[1] 
 		|| line == file->marks[2]
 		|| line == file->marks[3]
 		|| line == file->marks[4]
 		|| line == file->marks[5]
 		|| line == file->marks[6]
 		|| line == file->marks[7]
 		|| line == file->marks[8]
 		|| line == file->marks[9])
		&& line != NULL ) {
/*
	file->cur_mark = file->cur_line;
	file->marks[10] = file->cur_line;
*/
	file->cur_line = line;
	redraw_screen();
//	file->marks[11] = file->cur_line;
 }
 return 0;
}


int
go_mark_prev() {

 line_t *line = file->cur_line;

 do
	line = line->backw;
 while ((line != file->marks[0] 
 		&& line != file->marks[1] 
 		&& line != file->marks[2] 
 		&& line != file->marks[3] 
 		&& line != file->marks[4] 
 		&& line != file->marks[5] 
 		&& line != file->marks[6] 
 		&& line != file->marks[7] 
 		&& line != file->marks[8] 
 		&& line != file->marks[9])
		&& line != NULL);

 if ((line == file->marks[0] 
 		|| line == file->marks[1] 
 		|| line == file->marks[2]
 		|| line == file->marks[3]
 		|| line == file->marks[4]
 		|| line == file->marks[5]
 		|| line == file->marks[6]
 		|| line == file->marks[7]
 		|| line == file->marks[8]
 		|| line == file->marks[9])
		&& line != NULL ) {

//	file->cur_mark = file->cur_line;
//	file->marks[10] = file->cur_line;

	file->cur_line = line;
	redraw_screen();
//	file->marks[11] = file->cur_line;
 }
 return 0;
}


int
go_mark_last ()
{
// go_mark_number(10);
 line_t *line = get_head();

 while (line != file->last_mark && line != NULL)
	line = line->forw;
 if (line == file->last_mark && file->last_mark != NULL ) {
	file->last_mark = file->cur_line;
	file->cur_line = line;
	redraw_screen();
 }
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
	file->cur_line = line;
	redraw_screen();
 }
 return 0;
}


int
arrange_markers(line_t** m1, line_t** m2)
{
 file->copy_start_pos = file->use_mark1;
 file->copy_end_pos = file->use_mark2;

 m1 = &file->copy_start_pos;
 m2 = &file->copy_end_pos;

 line_t *tmp, *lines, *start, *end;
 int match = 0;
 int valid_start = 0;
 int valid_end = 0;

/*
 start = file->copy_start_pos;
 end = file->copy_end_pos; 
*/
 start = *m1;
 end = *m2; 

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
	tmp = *m1;
	*m1 = *m2;
	*m2 = tmp;

	start = *m1;
	end = *m2;

	while( start != NULL && start != end )
	   start = start->forw;
	if( start == end ) {
	   match = 1;
	}
	else
	return 1;
 }

/*
 *m1 = start;
 *m2 = end;
 start = *m1;
 end = *m2;
*/

 return 0;
}


int
cmd_yank()
{
 free_lines( copy_buffer_head, NULL);
//here
 copy_buffer_head = NULL;
 copy_buffer_tail = NULL;

 copy_lines( file->copy_start_pos, file->copy_end_pos, 
		&copy_buffer_head, &copy_buffer_tail );
 return 0;
}


int
cmd_paste()
{
 line_t *start_ins = NULL; 
 line_t *end_ins = NULL;
 copy_lines( copy_buffer_head, copy_buffer_tail, &start_ins, &end_ins );
 insert_lines( file->cur_line,  start_ins, end_ins);
 if (file->cur_line == NULL)
	file->cur_line = start_ins;
 message(CLL"insert line done");
 redraw_screen();
 return 0;
}


int
cmd_paste_before()
{
 line_t *start_ins = NULL; 
 line_t *end_ins = NULL;
 copy_lines( copy_buffer_head, copy_buffer_tail, &start_ins, &end_ins );
 insert_lines_before( file->cur_line,  start_ins, end_ins);
 if (file->cur_line == NULL)
	file->cur_line = start_ins;
 message(CLL"insert line done");
 redraw_screen();
 return 0;
}


/*
int
delete(line_t *l, line_t *m)
{
 line_t *prev, *next;

 if(l)
 prev = l->backw;
 if(m)
 next = m->forw;


 free_lines(l, m);//seg fault
 if(prev)
 prev->forw = next;
 if(next)
 next->backw = prev;

 return 0;
}
*/

int
delete(line_t *l, line_t *m)
{
 if (l == NULL)
	l = get_head();
 else
	l = l->forw;

 if (m == NULL)
	m = get_tail();
 else
	m = m->backw;

 line_t *prev, *next;

 prev = l->backw;
 next = m->forw;

 free_lines(l, m);//seg fault
 if(prev)
 prev->forw = next;
 if(next)
 next->backw = prev;

 return 0;
}

int
del_lines(line_t *l, line_t *m)
{
 if(!l || !m) return 1;
/*
 line_t *l = file->copy_start_pos;
 line_t *m = file->copy_end_pos;
*/
 line_t *prev, *next;

 if(l)
 prev = l->backw;
 if(m)
 next = m->forw;

 assert(l != NULL && m != NULL);

 cmd_yank();

 free_lines(l, m);
 if(prev)
 prev->forw = next;
 if(next)
 next->backw = prev;

 if(next)
 file->cur_line = next;
 else
 file->cur_line = prev;
 redraw_screen();
 return 0;
}


int
cmd_del_block()
{
 if (!file->cur_line)
	return 1;
 del_lines(file->copy_start_pos, file->copy_end_pos);
 return 0;
}


int
cmd_del_line()
{


 file->copy_start_pos = file->cur_line;
 file->copy_end_pos = file->cur_line;


// del_lines(file->cur_line, file->cur_line);
 del_lines(file->copy_start_pos, file->copy_end_pos);
 return 0;
}


int 
free_lines(line_t *l, line_t *m)
{
sprintf(msg, "%p, %p", l, m);

 if(!l)	return 0;

 line_t *next;
 while( l != NULL && l != m ) {
	next = l->forw;
	free( l->str );
	free( l );
	l = next;
 }
 return 0;
}

#if 0
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
#endif


int
cur_line_number()
{

 if( !file->cur_line )
   return 0;


 int i;
 line_t *line;

 line = get_head();
 for(i = 1; line != file->cur_line; i++) {
   line = line->forw;
 }
 return i;
}

int
set_cur_line(int n)
{

 if( !file->cur_line )
   return 0;


 int i;
 line_t *line;

 line = get_head();
 for(i = 1; i != n; i++) {
   line = line->forw;
 }
 file->cur_line = line;
 return i;
}


line_t*
get_head()
{
 line_t *line = file->cur_line;

 if (!line) return line;

 while( line->backw != NULL )
	line = line->backw;
 return line;
}


line_t*
get_tail(){
 line_t *line = file->cur_line;

 if (!line) return line;

 while( line->forw != NULL )
	line = line->forw;
 return line;
}


int 
show_bar()
{
 if(!file->cur_line) return 1;

 int line_no = cur_line_number();

 int y, x;
 int i;
 getpos(&y, &x);
 moveyx(_rows-1, 1);

 printf(CLL);
 printf("cursor: %d, %d\tx_curs: %d\t", x, y, file->x_curs);
 printf("%1$d %2$x %2$c", file->offs, *(file->cur_line->str + file->offs) );
 printf("   x: %d", file->cur_line->x);

/* bits */
 char bits[9] = {0};
 int tmp_mask = cmd_mask;
 
 for (i = 0; i < 8; i++)
 {
	 bits[i] = (tmp_mask & 1) + '0';
	 tmp_mask = tmp_mask >> 1;
 }
 moveyx(_rows-1, 50);
 printf("cmd_mask: %s", bits);
 if (global) printf("   GLOBAL");

 moveyx(_rows, 1);
 printf(CLL);
 moveyx(_rows, 55);
// sprintf(msg, "%x, %d", com_key, key_pressed[com_key] + key_stat[com_key]);
 printf( "    %s", msg);
 moveyx(_rows, 40);
 printf("%s", file->filename);
 moveyx(_rows, 20);
 printf("alt: ");
 if( _key_mode == 0 )
 	printf("OFF");
 else
 	printf("ON");
 moveyx(_rows, 1);
 printf("line: %d", line_no);
 moveyx(y, x);
 return 0;
}


int
show_buffers() {
 int i;
 buffer_t *tmp;
 char buf[MAX_LINE]={0};
 
 tmp = file;
 for (;tmp->prev; tmp=tmp->prev);
 for (;tmp; tmp = tmp->next) {
	strcat(buf, tmp->filename);
	strcat(buf, " ");
 }
 message(buf);
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
 if(!file->cur_line) return 1;

 char buf[MAX_LINE];
 int n;
 line_t *line;
 save_cur();
 moveyx(_winbot + 1, 1);
 printf(CLL);
 readstring(buf, MAX_LINE);
 rest_cur();
 n = atoi(buf);
 if( line = goto_line( n ) ) {
	file->cur_line = line;
	redraw_screen();
 }
 reset();
 return 0;
}


void 
sigsegv_handler(int i)
{
 sig_handler();
 printf("SIGSEGV\n");
 abort();
}


int
quit()
{
 exit(EXIT_SUCCESS);
 return 0;
}


int
cmd_quit()
{
 while(1) 
	close_file();
}


void
sig_handler()
{
 int i, y, x;
 write_key_pressed();
 freecommands();
 freecommands_str();
 getpos(&y, &x);
 set_scroll_win(1, _rows);
 _wintop = 1;
 _winbot = _rows;
 moveyx(y, x);
 for(i = 0; i < _rows - y + 2; i++)
 	scroll_up();
 setecho();
 setcanon();
 fclose(erlog);
 printf("sig_handler()\n");
}


int 
file_buffer_next() {
 if(file && file->next) {
	last_file = file;
	file = file->next;
	redraw_screen();
 }
 return 0;
}


int 
file_buffer_prev() {
 if(file && file->prev) {
	last_file = file;
	file = file->prev;
	redraw_screen();
 }
 return 0;
}


int 
file_buffer_go(int n) {
 int i;
 buffer_t *tmp = file;
 buffer_t *tmp_last_file = last_file;

 if(!file) return 1;

 last_file = file;

 while(file->prev)
	file = file->prev;
 for(i = 1; i < n && file->next; i++) {
	file = file->next;
 }
 if( i != n) {
	file = tmp;
	last_file = tmp_last_file;
 }
 redraw_screen();
 return 0;
}


int
file_buffer_go_1 () {
 file_buffer_go (1);
return 0;
}


int
file_buffer_go_2 () {
 file_buffer_go (2);
return 0;
}


int
file_buffer_go_3 () {
 file_buffer_go (3);
return 0;
}


int
file_buffer_go_4 () {
 file_buffer_go (4);
return 0;
}


int
file_buffer_go_5 () {
 file_buffer_go (5);
return 0;
}


int
file_buffer_go_6 () {
 file_buffer_go (6);
return 0;
}


int
file_buffer_go_7 () {
 file_buffer_go (7);
return 0;
}


int
file_buffer_go_8 () {
 file_buffer_go (8);
return 0;
}


int
file_buffer_go_9 () {
 file_buffer_go (9);
return 0;
}


int 
file_buffer_go_last (int n) {
 buffer_t *tmp;

 tmp = file;
 file = last_file;
 last_file = tmp;
 redraw_screen();
 return 0;
}


int
read_key_pressed() {
 int i;
 FILE *f;
 char path[MAX_BUF];
 strcpy(path, getenv("HOME"));
 strcat(path, "/.med/pressed.key");
 f = fopen(path, "r");
 if(f) {
	fread(key_stat, sizeof(int), 256, f);
	fclose(f);
 }
 return 0;
}


int
write_key_pressed() {
 int i;
 FILE *f;
 int buf[256] = {0};
 char path[MAX_BUF];

 strcpy(path, getenv("HOME"));
 strcat(path, "/.med/pressed.key");

 f = fopen(path, "r+");

 if(!f) 
	f = fopen(path, "w+");
 if(!f) {
	gmessage("failed to open \"pressed.key\"\n");
	return 1;
 }

 i = fread(buf, sizeof(int), 256, f);
 rewind(f);

 for (i = 0; i < 256; i++)
	buf[i] += key_pressed[i];
 i = fwrite(buf, sizeof(int), 256, f);
 rewind(f);
// i = fread(buf, sizeof(int), 256, f);

 fclose(f);
 return 0;
}


int
change_color(int col)
{
 int i;
 line_t *m1, *m2;

 m1 = file->copy_start_pos;
 m2 = file->copy_end_pos;
 
 while(m1 != m2) {
	m1->color = col;
	m1 = m1->forw;
 }
 m1->color = col;
 redraw_screen();
 return 0;
}


int
cmd_change_color()
{
	char ch_col_ch = getchar();
	switch (ch_col_ch) {
	case '0':
		change_color(0);
		break;
	case '1':
		change_color(31);
		break;
	case '2':
		change_color(32);
		break;
	case '3':
		change_color(33);
		break;
	case '4':
		change_color(34);
		break;
	case '5':
		change_color(35);
		break;
	case '6':
		change_color(36);
		break;
	case '7':
		change_color(37);
		break;
	default:
		break;
	}
 return 0;
}


int
cmd_dead_key_pressed()
{
 cmd_mask = ~cmd_mask;
 _switch_key_mode = (_switch_key_mode) ? 0 : 1;
 sprintf(msg, "%d", cmd_mask); message(msg);
 sprintf(msg, "_switch...: %d", _switch_key_mode); message(msg);
 need_goto = 1;
 return 0;
}

int
cmd_open(int argn, char** argv)
{
 if (argn == 1)
	return 0;

 int i;
 buffer_t *tmp;

 last_file = file;

 if (argv[1][0] != '-')
	options = 0;
 else {
	options = 1;
	for (i = 0; argv[1][i] != '\0'; i++)
		switch (argv[1][i]) {
		case 'c':
			create_f = 1;
			break;
		default:
			break;
		}
 }
 if (options == 1)
	i = 2;
 else
	i = 1;
 for( ; i < argn; i++)
 	open_file(argv[i]);

	create_f = 0;

 redraw_screen();
 return 0;
}


int
cmd_rename(int argn, char** argv)
{
 if (argn != 2)
	return 1;

 int ret;

 ret = rename(file->filename, argv[1]); 
 if(ret != 0) return 1;
 strcpy(file->filename, argv[1]);

 redraw_screen();
 return 0;
}

int
set_jump(int argn, char** argv)
{
 if (argn != 2)
	return 1;

 int i = atoi(argv[1]);

 if (i <= 0) i = vert_jmp;
 if (i > 100) i = 100;

 vert_jmp = i;
 return 0;
}

int
set_autoindent(int argc, char** argv)
{
 if (argc != 1) return 1;
 autoindent = (autoindent) ? 0 : 1; 
 sprintf(msg, "autoindent: %d", autoindent);
 message(msg);
 return 0;
}

int
indent_cur_line()
{
 if (file->cur_line->backw == NULL) return 1;

 int i;

 for (i = 0; isspace(file->cur_line->backw->str); i++);
 file->cur_line->str = malloc(i + 1);
 file->cur_line->str[i] = '\0';
 strncpy(file->cur_line->str, file->cur_line->backw->str, i);
// move_forwart(i);
// cmd_edit_line_end()

 return 0;
}

int
cur_len() {
 return strlen(file->cur_line->str);
}