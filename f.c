int i; for (i= 0   ; i < 10; ii  ++)
aaaaa     ;;;;;;;     bbbbbb
ddddd    aaaaaa;;;;;  bbbbbbbb
sdlkfksdjfj
aaaaa11111
1
2
3
//#define NO_SHOW_BAR
#define DEB_SCR_SIZE 0
#include <ncurses.h>
#include <stdio.h>
#iaaaaaaaaaaa123nclude <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#i11111222222333333nclude <signal.h>
#define DEBUG_EDIT_INSERT 1
#include <assert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#if 0
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <errno.h>
#endif
#include <wchar.h>

{
	#include "init.h"
	#include "med.h"
	#include "move_x.h"
	#include "edit.h"
	#include "colors.h"
}
//#include "find.h"
#include "command.h"
	;

#include "keys.h"
#include "undo.h"
#include "script.h"
#include "debug.h"
 
#define ESC 27

extern uint32_t cmd_mask;

#if 0
#define \
askchar() \
 int c;\
 c = getchar();\
 if (c == ESC)\
 	c = BCSP;\
 if (c == BCSP)\
 	c = ESC;\
 	
#endif
char
askchar() {
 int c = getchar();
/*
 if (c == ESC) c = BCSP;
 else
 if (c == BCSP) c = ESC;
*/
 return c;
}

#define moveyx(y, x) ( printf("\033[%d;%dH", (y), (x)) )
#define movex(x) ( printf("\033[;%dH", (x)) )
#define movey(y) ( printf("\033[%d;H", (y)) )
#define clreol() (printf("\033[K") )
#define CLR "\033[K"
#define CLL "\033[2K"
#define clear_all() ( printf("\033[2J") );
#define clear_down() ( printf("\033[0J") );
#define clear_up() ( printf("\033[1J") );

#define \
color(col)\
(printf("\x1B[%dm", col));

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

int set_show_buffers = 1;
int gl_deb;
int fpr;
int global;
int need_goto;
line_t *copy_buffer_head = NULL;
line_t *copy_buffer_tail = NULL;

char com_key;
 
int _key_mode = 0;
int _switch_key_mode = 0;
int trm_cols, trm_rows;
int _wintop, _winbot, _wincur;
int dwin_top;
int dwin_bot;
int dwin_size = DEB_SCR_SIZE;
int trm_width;
int _h_scroll = 15;
int vert_jmp = 12;
int autoindent = 1;

int (*pfind)() = &simple_find;

int readonly;
int create_f;
int options;

buffer_t* last_file;

int msg_prior_level;
buffer_t* file;
int tab_len;
char msg[MAX_LINE];
char search_buf[MAX_LINE];
char s_buf[MAX_LINE];
char r_buf[MAX_LINE];
char invoke_buf[MAX_LINE];
char compile_and_run_buf[MAX_LINE];// compile and run
char enframe_top[MAX_LINE] = "/*";
char enframe_bot[MAX_LINE] = "*/";
char seq[256];
int key_pressed[256];
int key_stat[256];
//int key_pressed[256]={0};
FILE *erlog;


int
main( int argn, char *argv[])
{

 if (argn == 1)
 {
	printf("med [-rcmh] [file...]\n"); 
	printf("med -h for help\n"); 
	printf("med -m for commands' help\n"); 
	return 1;
 }

 if (LOG == 1)
	erlog = fopen("error.txt", "w");
 else
	erlog = fopen("/dev/null", "w");
 fprintf(erlog, "*********************************\n");
 fflush(erlog);
 
// init();
 
 int i;
 
 int (*pfunc)() = NULL;
 cmd_t* cmd = NULL;

 char *stdbuf = malloc(2048);
 
// setvbuf(stdout, NULL, _IONBF, 0);
 setvbuf(stdout, stdbuf, _IONBF, 2048);
// setvbuf(stdin,  stdbuf, _IONBF, 256);

 
 make_commands();
 make_help();
 
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
		case 'm':
			help_comm(0, NULL);
			return 1;
		case 'n':
			help_comm2(0, NULL);
			return 1;
		case 'h':
			printf("med [-rcmh] [file...]\n\n"); 
			printf("-r\tOpen file in Read-Only mode.\n");
			printf("-c\tCreate file if does not exist.\n");
			printf("-m\tDisplay list of commands.\n");
			printf("-h\tDisplay this help.\n");
			return 1;
		default:
			break;
		}
 }

 tab_len = 4;

 int key_valid = 1;
 for(i = 0; i < 256; i++) {
 	key_pressed[i] = 0;
 	key_stat[i] = 0;
 }
 read_key_pressed();

 file = NULL;

 FILE* f;
 char buf[MAX_LINE];
 struct winsize ws;

 copy_buffer_head = NULL;
 copy_buffer_tail = NULL;

 if (options == 1)
	i = 2;
 else
	i = 1;
 for( ; i < argn; i++)
 {
 	if (open_file(argv[i]) == -1)
 	;
// 		perror(argv[i]);
 	
 }
 

 create_f = 0;

 last_file = file;

 if(!file) {
// 	printf("exiting: no buffers to edit\n");
	return 1;
 }
 
 signal (SIGABRT, &debug_exit);
 signal (SIGTERM, &sig_handler);
 signal (SIGSEGV, &sigsegv_handler);
 atexit( &sig_handler );
// atexit( &debug_exit );

   
 while(file->prev)
	file = file->prev;

 init2();
 
 set_raw_mode(ON);

 while( 1 ) {
loop:
	ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
	if (ws.ws_row != trm_rows || ws.ws_col != trm_cols)
	{
		trm_rows = ws.ws_row;
		trm_cols = ws.ws_col;
		resize();
		redraw_screen();
	}
	
	show_bar();
	
	if (set_show_buffers == 1)
	{
//		gmessage("set_show_buf..");
		show_buffers();
	}
		
	need_goto = 0;
	
	com_key = getchar();
/*
	while(com_key != 14)
	{
		com_key = getchar();
		printf("%d\n", com_key);
	}
*/
	
 
	message("");

	cmd = NULL;
	pfunc = NULL;
	cmd = command_char(com_key);
	if (cmd)
	pfunc = cmd->noargsfunc;
	

	 if (file->cur_line == NULL)
	 if (pfunc == del_char 
	 	|| pfunc == case_word
		|| pfunc == case_backward
		|| pfunc == case_char
		|| pfunc == move_word_back
		|| pfunc == insert_indent
		|| pfunc == insert_string
		|| pfunc == insert_string_after
		|| pfunc == cmd_edit_line_begin
		|| pfunc == cmd_edit_line_end
		|| pfunc == concat_lines
		|| pfunc == change_char
		|| pfunc == insert_space
		|| pfunc == insert_char
		|| pfunc == insert_char_next
		|| pfunc == move_down_half_screen
		|| pfunc == move_up_half_screen
		|| pfunc == move_down_one
		|| pfunc == move_up_one
		|| pfunc == move_up_block
		|| pfunc == move_down_block
		|| pfunc == move_first_line
		|| pfunc == move_last_line
		|| pfunc == move_first_col
		|| pfunc == move_last_col
		|| pfunc == move_forward_one
		|| pfunc == move_forward_hs
		|| pfunc == move_backward_one
		|| pfunc == move_backward_hs
		|| pfunc == move_bow
		|| pfunc == move_eow
		|| pfunc == del_word
		|| pfunc == cmd_del_word
		|| pfunc == cmd_change_word
		|| pfunc == cmd_simple_find
		|| pfunc == cmd_find_name
		|| pfunc == cmd_find_beginning
		|| pfunc == refind
		|| pfunc == alt_refind
		|| pfunc == cmd_replace_marks
		|| pfunc == cmd_replace_whole
		|| pfunc == cmd_rename_marks
		|| pfunc == cmd_rename_whole
		|| pfunc == cmd_repeat_repl_marks
		|| pfunc == cmd_repeat_rename_marks
		|| pfunc == cmd_repeat_repl_whole
		|| pfunc == cmd_repeat_rename_whole
		|| pfunc == go_mark_next
		|| pfunc == go_mark_prev
		|| pfunc == go_mark_last
		|| pfunc == yank
		|| pfunc == cmd_del_line
		|| pfunc == enframe_cmd
		|| pfunc == enframe_alt_ind_cmd
		|| pfunc == enframe_alt_noind_cmd
	/*
		|| pfunc == 
		|| pfunc == 
		|| pfunc == 
		|| pfunc == 
		|| pfunc == 
	*/
		)
	 {
		message("Buffer is empty!");
		continue;;
	 }
	
	 if ( pfunc == insert_indent || pfunc == cmd_replace_marks
			|| pfunc == cmd_rename_marks || pfunc == cmd_repeat_repl_marks
			|| pfunc == cmd_repeat_rename_marks || pfunc == yank
			|| pfunc == cmd_del_block 
			|| pfunc == cmd_change_color 
			|| pfunc == enframe_cmd
			|| pfunc == enframe_alt_ind_cmd
			|| pfunc == enframe_alt_noind_cmd
		)
	 if (auto_set_mark()) continue;
	
	/*  undo */
	 if (pfunc == insert_char || pfunc == insert_char_next || 
		 pfunc == change_char || pfunc == insert_space ||
		 pfunc == del_char || pfunc == cmd_del_endline ||
		 pfunc == cmd_del_word || pfunc == cmd_change_word ||
		 pfunc == cmd_del_long_word || pfunc == cmd_change_long_word ||
		 pfunc == case_word || pfunc == case_backward ||
		 pfunc == case_char ||
	 	 pfunc == insert_string ||
	 	 pfunc == insert_string_after
		 )
		fill_undo(REPLACE, file->cur_line, file->cur_line, file->cur_line->backw,
						file->cur_line->backw, file->cur_line->forw);
	
	
	 if (file->cur_line)
	 if (pfunc == cmd_edit_new_line_after)
		fill_undo(DELETE, NULL, NULL, NULL, file->cur_line, file->cur_line->forw);
	
	 if (file->cur_line)
	 if (pfunc == cmd_edit_new_line_before) // bug?
		fill_undo(DELETE, NULL, NULL, NULL, file->cur_line->backw, file->cur_line);
	
	 if (pfunc == cmd_del_line)
		fill_undo(INSERT, cl, cl, cl->backw, NULL, NULL);  
		
	 if (pfunc == cmd_del_block)
		fill_undo(INSERT, file->copy_start_pos, file->copy_end_pos, 
				file->copy_start_pos->backw, NULL, NULL);
		
	 if (pfunc == concat_lines)
	 if (cl->forw)
		fill_undo(REPLACE, cl, cl->forw, 
				cl->backw, cl->backw, cl->forw->forw);
		
	 if (pfunc == cmd_paste ||
	 	pfunc == cmd_make_line_after)
		fill_undo(DELETE, NULL, NULL, NULL, cl, (!cl) ? NULL : cl->forw);  
		
	 if (pfunc == cmd_paste_before ||
	 	pfunc == cmd_make_line_before)
		fill_undo(DELETE, NULL, NULL, NULL, (!cl) ? NULL : cl->backw, cl);  
		
	 if (pfunc == new_func_templ)
		fill_undo(DELETE, NULL, NULL, NULL, cl, (!cl) ? NULL : cl->forw);  
	
/*
	 if (!strcmp(cmd->name, "enframe") ||
	 	!strcmp(cmd->name, "enframe_alt")
	 	)
*/
	 if (pfunc == enframe_cmd ||
	 	pfunc == enframe_alt_ind_cmd ||
	 	pfunc == enframe_alt_noind_cmd)
		fill_undo(REPLACE, 
			file->copy_start_pos, file->copy_end_pos,
			file->copy_start_pos->backw,
			file->copy_start_pos->backw, file->copy_end_pos->forw  
			);
	 	
	 if (pfunc)
		pfunc();


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
save_cur() 
{
 if (file->top == MAX_STACK) {
 	gmessage("Stack is full!");
	return 1;
 } 
 file->top++;
 getpos(&file->y_cur[file->top], &file->x_cur[file->top]);
 return 0;
}
int
rest_cur() 
{
 if (file->top == -1) {
 	gmessage("Stack is empty!");
	return 1;
 }
 moveyx(file->y_cur[file->top], file->x_cur[file->top]);
 file->top--;
 return 0;
}


int
getsize () 
{
 struct winsize ws;
 ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
 trm_rows = ws.ws_row;
 trm_cols = ws.ws_col;
 return 0;
}

int
resize()
{
 trm_width = trm_cols;
 
 _wintop = 1 + ((set_show_buffers) ? 2 : 1);
 dwin_bot = trm_rows - 2;
 dwin_top = dwin_bot - dwin_size + 1;
 _winbot = (dwin_size) ? dwin_top - 1 : trm_rows - 2; 
 _wincur = ( _winbot - _wintop ) / 2 + _wintop;
 
 save_cur(); 
 set_scroll_win(_wintop, _winbot);
 rest_cur(); 
 
 vert_jmp = (_winbot - _wintop) / 2;

 return 0;
}

int
do_nothing()
{
 int i;
}

int
dprint(char* format, ...)
{
 if (!dwin_size)
 	return 1;
 	
 va_list ap;
 
 if (file->y_dwin < dwin_top)
 	file->y_dwin = dwin_top;
 	
 if (file->y_dwin > dwin_bot)
 	file->y_dwin = dwin_bot;
 	
 if (file->x_dwin == 0)
 	file->x_dwin = 2;
 	
 save_cur(); 
 set_scroll_win(dwin_top, dwin_bot);
 va_start(ap, format);
 color(RED);
 moveyx(file->y_dwin, file->x_dwin);
 vfprintf(stderr, format, ap);
 color(NRM);
 va_end(ap);
 getpos(&file->y_dwin, &file->x_dwin);
 rest_cur();
 set_scroll_win(_wintop, _winbot);
 return 0;
}

int
set_raw_mode(int act)
{
 static struct termios term, restore;
 
 if (act == ON) {
	 tcgetattr(0, &term);
	 tcgetattr(0, &restore);
	 term.c_iflag &= ~IXON;
	 term.c_lflag &= ~(ICANON|ECHO);
	 term.c_lflag |= ISIG;
	// term.c_iflag &= ~(INLCR|IGNCR|ICRNL);
	 tcsetattr(0, TCSANOW, &term);
 }
 if (act == OFF)
 	tcsetattr(0, TCSANOW, &restore);
 
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
 del(&cur_str, offs+1, 1);
 print_cur_line();
 file->cur_line->len = strlen(cur_str);
 if(offs == file->cur_line->len)
	move_backward(1);
 return 0;
}


int
case_word()
{
 int i;
 for(i = offs; isalpha( cur_str[i] ); i++) {
 if('a' <= cur_str[i] && cur_str[i] <= 'z')
   cur_str[i] = toupper(cur_str[i]);
 else
   cur_str[i] = tolower(cur_str[i]);
 }
 print_cur_line();
 return 0;
}


int
case_backward()
{
 if(offs == 0) return 0;

 int i;
 for(i = offs - 1; (isalnum(cur_str[i]) || cur_str[i] == '_') && i >= 0; i--)
   cur_str[i] = toupper(cur_str[i]);
 print_cur_line();
 return 0;
}


int
case_char()
{
 if (isalpha( cur_str[offs] )) {
 if('a' <= cur_str[offs] && cur_str[offs] <= 'z')
   cur_str[offs] = toupper(cur_str[offs]);
 else
   cur_str[offs] = tolower(cur_str[offs]);
 }
 print_cur_line();
 return 0;
}


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
 ch = mget_char();
 if (ch == 0)
 	return 1;
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
simple_print(int n, char* str)
{
 int i;
 
 for (i = 0; i < n; i++)
 {
	if (isprint(str[i]))
		putchar(str[i]);
	else 
	if (str[i] == '\t')
	{
		putchar('\\');
		putchar('t');
	}
 }
 return 0;
}


int
mget_char()
{
 int c = getchar();
 
/*
 if (c == 27)
 	return 1;
*/
 if ((c == '\t' || !iscntrl(c)) && isascii(c))
 	return c;
 else
 	return 0;
 	
 return 0;
}


int
readstring(char* buf, int nmax) {
 int i, tmp, res, ret;
 char ch;
 int y, x;

/*
 setnocanon();
 setnoecho();
*/

 getpos(&y, &x);
// save_cur();
 
 i = 0;
 do {
//	res = read(0, &ch, 1);
//	ch = getchar();
	ch = askchar();

	switch (ch) {
	case BCSP:
		ch = DEL;

	case DEL:
		if( i ) {
			*(buf+i) = ch;
			write(1, buf+i, 1);
			write(1, " ", 1);
			write(1, buf+i, 1);
			if (*(buf+i-1) == '\t')
			{
				write(1, buf+i, 1);
				write(1, " ", 1);
				write(1, buf+i, 1);
			}
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
		moveyx(y, x);
		simple_print(i, buf);
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
/*
 *(buf+i) = 0;
 save_cur();
 printf("%s"CLR, buf);
// rest_cur();
*/
 } while ( ch != 0 && i < nmax);
 dpriv(nmax, d); 
 buf[nmax-1] = 0;
 moveyx(y, x);
 return ret; 
}


int
getstring(char* buf, char* str) {
 int ret;
 save_cur();
 moveyx(trm_rows-1, 1);
// moveyx(10, 10);
 printf(CLL"%s", str);
 ret = readstring(buf, MAX_LINE);
 rest_cur();
 return ret;
}


int
insert_string()
{
 int i, res, ret;
 int k;
 char ch;
 char *tmp;
 int tmp_len = 0;
 int tmp_offs;

/*
 setnocanon();
 setnoecho();
*/

 do {
//	res = read(0, &ch, 1);
	ch = askchar();

	switch (ch) {
	case BCSP:
		ch = DEL;

	case DEL:
		if( offs ) {
			move_backward(1);
			del(&cur_str, offs + 1, 1);
			file->cur_line->len--;
		}
		break;

	case 0:
	case 27:
		ret = ch;
		ch = 0;
		break;

	case 10:
		file->cur_line->len = strlen(cur_str);
		tmp = cur_str + offs;
		tmp_len = strlen(tmp);
		file->cur_line = make_line_after(file->cur_line);
		k = init_line(file->cur_line);
		
		cur_str = realloc(cur_str,
			strlen(cur_str) + tmp_len + 1 );
		strcat(cur_str, tmp);
		file->cur_line->backw->str =
			realloc( file->cur_line->backw->str, file->cur_line->backw->len - tmp_len+1 );
		file->cur_line->backw->str[file->cur_line->backw->len - tmp_len] = 0;
		tmp_offs = offs;
		reset_x();
//		fpr = 1;
		for (; k > 0; k--)
			get_next_move_x();
//		offs = i;
		place_cursor();

/*
		move_forward(tmp_offs+1);
		if(cur_str[offs] != '\0')
		{
			message("force_forward");
			offs += 1;
			file->x_curs = get_next_move_x(cur_str[i]);
			place_cursor();
		}
*/		

/*
		fprintf(erlog, "case 10: offset: %d, x_curs: %d\n", 
				offs, file->x_curs);
		fflush(erlog);
*/

//		place_cursor();
	
		redraw_screen();
		/*
		sprintf(msg, "case 10: offset: %d, tmp_offs: %d, x_curs: %d\n", 
					offs, tmp_offs, file->x_curs);
		message(msg);
		*/
		break;

 	case 'u'-0x60:
 		case_backward();
 		break;

	default:
 		if (!isprint(ch) && ch != '\t')
			break;
		insert_ch(&cur_str, offs, ch);
		if(ch == '\t')
			file->x_curs = nexttab1();
		else
			file->x_curs++;
		offs++;
		file->cur_line->len++;
		break;
		}
	 if(file->x_curs <= trm_width) {
//		place_cursor();
	 }
	 else {
		file->cur_line->x -= _h_scroll;
//		print_cur_line();
		file->x_curs -= _h_scroll;
//		place_cursor();
	 }
//sleep(1);
	print_cur_line();
	place_cursor();

	show_bar();

 } while ( ch != 0 );
 file->cur_line->len = strlen( cur_str);
 return ret; 
}


int
insert_string_after()
{
 int ret;
 if (*cur_str == '\0') {
//	ret = insert_string();
	return 0;
 }
 offs++;
 file->x_curs++;
 place_cursor();
 insert_string();
 offs++;
 file->x_curs--;
 place_cursor();
 offs--;
 
#if 0
 save_cur();
// move_forward(1);
 if (cur_str[offs] == '\t')
	file->x_curs = nexttab1();
 else
	file->x_curs++;
 offs++;
// moveyx(file->y_cur, file->x_curs);
 rest_cur();
 ret = insert_string();
 offs--;
 file->x_curs--;
// moveyx(file->y_cur, file->x_curs);
// rest_cur();
 return ret;
#endif
 return 0;
}


int
cmd_edit_new_line_before()
{
 file->cur_line = make_line_before(file->cur_line);
 init_line(file->cur_line);
 reset_x();
 redraw_screen();
 insert_string();
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
 if (!cl->forw) return 0;
 
 char *buf;
 line_t *tmp;
 
 buf = malloc( strlen(cur_str) + strlen(cl->forw->str) + 1 );
 strcpy(buf, cur_str);
 strcat(buf, cl->forw->str);
 free(cur_str);
 cur_str = buf;
 
// destroy_line(file->cur_line->forw);
 delete_lines(cl->forw, cl->forw);
 
 redraw_screen();
 
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
 free_line(tmp);

 return 0;
}

int
new_func_templ()
{
 line_t *l = cl;
/*
 l = make_line_after_str(l, "");
*/
 l = make_line_after_str(l, "");
 l = make_line_after_str(l, "int");
 l = make_line_after_str(l, "func()");
 cl = l;
 l = make_line_after_str(l, "{");
 l = make_line_after_str(l, " int i;");
 l = make_line_after_str(l, " ");
 l = make_line_after_str(l, " return 0;");
 l = make_line_after_str(l, "}");
/*
 l = make_line_after_str(l, "");
*/
 l = make_line_after_str(l, "");
 redraw_screen();
 return 0;
}

line_t*
make_line_after_str(line_t *prev, char* str)
{
 line_t* l;
 l = make_line_after(prev);
 init_line_str(l, str);
 return l;
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
 reset_x();
 redraw_screen();
}


int
cmd_make_line_after()
{
 file->cur_line = make_line_after(file->cur_line);
 init_line(file->cur_line);
 reset_x();
 redraw_screen();
}


int 
make_line()
{
 int i;
 line_t *prev, *next;

 if( file->cur_line == NULL ) {
	file->cur_line = malloc( sizeof(line_t) );
	file->cur_line->backw = NULL;
	cur_str = malloc(1);
	*(cur_str) = '\0';
	return 0;
 }

 if( file->cur_line->backw == NULL) {
	next = file->cur_line;
	file->cur_line = malloc( sizeof(line_t) );
	next->backw = file->cur_line;
	file->cur_line->backw = NULL;
	file->cur_line->forw = next;
	cur_str = calloc(1,1);
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
 cur_str = calloc(1,1);
 file->cur_line->len = 0;
 
 redraw_screen();
 moveyx(_wincur, 1);

 return 0;
}


int
change_char()
{
 char ch = mget_char();
 if (ch == 0)
 	return 1;

 cur_str[offs] = ch;
 print_cur_line();
}


int
insert_space()
{
 insert_ch( &cur_str, offs, ' ');
 file->cur_line->len++;
 print_cur_line();
 return 0;
}


int
insert_char()
{
 char ch = mget_char();
 if (ch == 0)
 	return 1;

 insert_ch( &cur_str, offs, ch);
 file->cur_line->len++;
 print_cur_line();
 position_cursor();
 return 0;
}


int
insert_char_next()
{
 char ch = mget_char();
 if (ch == 0)
 	return 1;
// save_cur();
 /*
 moveyx(file->y_cur, file->x_cur + 1);
 offs++;
 */
 move_forward(1);
 insert_ch( &cur_str, offs, ch);
 file->cur_line->len++;
 print_cur_line();
 position_cursor();
 move_forward(1);
 return 0;
}


int 
n_read_line(FILE* file, char* buf, int n)
{
 static int cnt;
printf("n_read_line: %d\n", cnt++);
 int i;
 char c;

 c = fgetc(file);
 for( i = 0; c != '\n' && c != EOF && c != '\0' && c != '\r' && i < n; i++ ) {
	buf[i] = c;
	c = fgetc(file);
 }
 buf[i] = '\0';

 if (c == '\r')
	c = fgetc(file);
/*
//???????????
 if (c != '\n')
	ungetc(c, file);
*/
 if (i == n)
 	return EOF;
 return c;
}


line_t*
read_file(FILE* file)
{
 char buf[MAX_LINE] = {0};
 line_t *lines = NULL;

 while( n_read_line(file, buf, MAX_LINE) != EOF ) {
	lines = make_line_after(lines);
/*
	lines->len = strlen( buf );
	lines->str = malloc( lines->len + 1 );
	strcpy( lines->str, buf);
*/
	lines->str = strdup(buf);
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
 
#ifdef READ
 readonly = 1;
#endif

 if (readonly)
	create_f = 0;

 if (create_f)
 	fd = open(name, O_RDWR | O_CREAT, 0666);
 else 	
 if (readonly)
 	fd = open(name, O_RDONLY);
 else
 	fd = open(name, O_RDWR);

/*
 snprintf(msg, MAX_LINE, "open %s: %s", name, strerror(errno));
 gmessage(msg);
*/
 	
 if ( fd < 0 ) {
	perror(name);
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

 offs = 0;
 file->x_curs = 1;
 file->fd = fd;
 file->top = -1;

 strcpy(file->filename, name);
 if (readonly)
	f = fdopen(fd, "r");
 else
	f = fdopen(fd, "r+");
 file->f = f;

 make_backup_copy(name);

 file_len = lseek(fd, 0, SEEK_END);
 lseek(fd, 0, SEEK_SET);

 if( file_len )
	file->cur_line = read_file(f);//BUG
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
message1(char* s, int prior)
{
 if (prior > msg_prior_level)
	return 1;
	
 if (_wintop < 2)
	print_line_x(1, 1, s);
 else
	print_line_x(1, _wintop - 1, s);
	
 msg_prior_level = prior;
 return 0;
}

int
dmessage(char *s)
{
 message1(s, -1);
 return 0;
}

int
message(char* s)
{
 message1(s, 0);
/*
 save_cur();
 moveyx(1, 1);
 printf(CLL"%s", msg);
 rest_cur();
 return 0;
*/
}


int
fmessage(char* format, ...)
{
 char s[MAX_LINE] = "";
 va_list ap;
 
 va_start(ap, format);
 vsnprintf(s, MAX_LINE, format, ap);
 va_end(ap);
 message(s);
 return 0;
}


int
gmessage(char* s)
{
 message(s);
 getchar();
 return 0;
}


int 
write_buf(char* name)
{
 line_t *line = get_head();
 FILE *f = fopen(name, "w");

 if (!f) return 1;
   
 while( line->forw ) 
 {
	fprintf(f, "%s\n", line->str);
	line = line->forw;
 }
 fprintf(f, "%s", line->str);
 
 fclose(f);
 return 0;
}


int
save_file() {
 if(!file) return 1;
 
 if (readonly) {
	message("readonly");
	return 1;
 }
 if (write_buf(file->filename) == 0)
	message("writing... done");
 else
	message("writing... failed");
 return 0;
}


int
save_all() {
 if(!file) return 1;
 
 if (readonly) {
	message("readonly");
	return 1;
 }
 
 buffer_t *tmp = file;

 while(file->prev)
	file = file->prev;

while(file) {
	write_buf(file->filename);
//	sprintf(msg, "writing %s...", file->filename);gmessage(msg);
	file = file->next;
}
 file = tmp;
 message("writing all... done(probably)");
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
//	reset_x();
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
//	reset_x();
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
 } while (!(isalpha(cur_str[0]) || cur_str[0] == '_'
			|| cur_str[0] == '{'));

 redraw_screen();
 return 0;
}


int
move_down_block()
{
 do {
	if (file->cur_line->forw == NULL) return 0;
	file->cur_line = file->cur_line->forw; 
 } while (!(isalpha(cur_str[0]) || cur_str[0] == '_'
			|| cur_str[0] == '}'));

 redraw_screen();
 return 0;
}


int
move_first_line()
{
 while( file->cur_line->backw != NULL )
	file->cur_line = file->cur_line->backw;
 redraw_screen();
 return 0;
}


int
move_last_line()
{
 while( file->cur_line->forw != NULL )
	file->cur_line = file->cur_line->forw;
 redraw_screen();
 return 0;
}


int
del_word()
{

 int i, to;

 to = ( isalnum( *(cur_str + offs) ) ) ? get_eow() : get_bow();

 del( &cur_str, offs + 1, to - (offs + 0) );
 file->cur_line->len = strlen(cur_str);
 print_cur_line();

return 0;
}


int
del_long_word()
{

 int i, to;

 to = ( isalnum(cur_str[offs]) || cur_str[offs] == '_') ? get_long_eow() : get_bow();

 del( &cur_str, offs + 1, to - (offs + 0) );
 file->cur_line->len = strlen(cur_str);
 print_cur_line();

return 0;
}


int
cmd_del_word()
{
 del_word();
 if( *(cur_str + offs) == 0)
	move_backward(1);
 return 0;
}


int
cmd_del_long_word()
{
 del_long_word();
 if( *(cur_str + offs) == 0)
	move_backward(1);
 return 0;
}


int
del_endline()
{
 del( &cur_str, offs + 1 + 1, strlen(cur_str) );
 print_cur_line();
 return 0;
}


int
cmd_del_endline()
{
 del_endline();
 if( *(cur_str + offs) == 0)
	move_backward(1);
 return 0;
}


int
cmd_change_word()
{
 del_word();
 insert_string();
 if( *(cur_str + offs) == 0)
	move_backward(1);
 return 0;
}

int
cmd_change_long_word()
{
 del_long_word();
 insert_string();
 if( *(cur_str + offs) == 0)
	move_backward(1);
 return 0;
}


int
print_top_line()
{
 int i;
 line_t* pr_line;
 pr_line = file->cur_line;
 for( i = 0; i < _wincur - _wintop; i++ ) 
 {
	if (pr_line->backw == NULL) return;
	pr_line = pr_line->backw; 
 }
 output_line(pr_line, _wintop);
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
 output_line(pr_line, _winbot);
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
 while(cur <= trm_width && str[i] != 0) 
 {
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
 int i = 0, j = 0;
 int tmp = o;
 int prev_o;
 char* p = str;
 int ch_len; //mbrlen
 int negative;
 int positive;
 
// getpos(&y, &x);
 save_cur();
 moveyx(line, 1);
 printf(CLL);

 while ( o <= 0)
 {
	if (str[i] == '\t')
		o = nexttab3( o - (tmp-1), tmp - 1);
	else
		o++;
	i++;
	
 }
 prev_o = o;
 p = str+i;


 while (o <= trm_width && str[i] != 0) 
 {
	if (str[i] == '\t') 
	{
	   prev_o = o;
	   o = nexttab3( o - (tmp-1), tmp - 1);
	   
	   while(*p == '\t')
		   p++;
		   
	   if (j)
	   {
   			moveyx(line, prev_o - j);
   			
	   		write(1, p, j);
/*
	   		sprintf(msg, "o:%d prev_o:%d", o, prev_o);
	   		message(msg);
*/
	   		j = 0;
	   		p = str + i + 1;
	   		prev_o = o;
	   }
	} 
	else 
 	{
	   j++;
	   o++;
	}
	i++;
 }
/*
 fprintf(erlog, "\no:%d prev_o:%d j:%d p-str:%d\n", o, prev_o, j, p-str); 
*/
 if (j)
 {
 	 moveyx(line, o - j);
 	 write(1, p, j);
 } 
/*
 sprintf(msg, "o:%d prev_o:%d", o, prev_o);
 message(msg);
*/
 rest_cur();
 return 0;
}


int
print_line_x_color(int o, int line, char *str, int col)
{
 assert(str);
 int i = 0;
// int y, x;
 int tmp;
// getpos(&y, &x);
 save_cur();
 moveyx(line, 1);
 printf(CLL);
 color(col);


/*
 moveyx(X_MODE_, 1);
 printf(CLL);
*/


 tmp = o;
 while(o <= trm_width && str[i] != 0) {
	if (o > 0 )
	   moveyx(line, o);


/*
	else
	   moveyx(X_MODE_, trm_cols + o);
*/


	if (str[i] == '\t')
		o = nexttab3( o - (tmp-1), tmp - 1);
	else {
		if (o > 0 )
		{
//	 		write(1, (str+i), 1);
	 		putchar(str[i]);
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
// moveyx(y, x);
 rest_cur();
/*
 sprintf(msg, "y: %d - x: %d", y, x);
 gmessage(msg);
*/
 printf("\x1B[0m"); /* escape codes */
}

int
output_line(line_t* l, int line_n)
{
 print_line_x(l->x, line_n, l->str);
/*
 print_line_x_color(l->x, line_n, l->str, l->color);
*/
 return 0;
}

int
print_color_line(line_t* l, int pos)
{
 output_line(l, pos);
 return 0;
}


int
print_cur_line()
{

/*
 output_line(file->cur_line->x, _wincur, cur_str,
			file->cur_line->color);
*/

 output_line(file->cur_line, _wincur);
 return 0;
}


int
redraw_screen()
{
 int i, n;
 line_t* ln;

 moveyx(dwin_top, 1);
 clear_up();

 
 msg_prior_level = 0;

 if (!file->cur_line)
 return 1;
// printf("redraw_screen:debug\n"); exit(1);

// debug(); 
 ln = file->cur_line;
// calculate_all_lengths();

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

		
//	print_line_x_color(ln->x, n+i, ln->str, ln->color);
	
	output_line(ln, n+i); /* seg fault*/
	ln = ln->forw;
 }
 output_line(ln, n+i);
// output_line(ln->x, n+i, ln->str);
// output_line(ln, n+i);
 place_cursor();
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
//gmessage("bubug");
 reset_x();
// message(search_pos->str);//bug!!!
 return 0;
}


int
cmd_simple_find()
{
 if (getstring(s_buf, "Find:") == 27)
 	return 27;
// if (s_buf[0])
// move_first_line();
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
 int o = offs;
 char *pos;

 pfind = &find_name;

 if( len )
 for ( ; line != NULL; line = line->forw, o = -1)
 for (pos = line->str + o + 1; (pos = strstr (pos, s_buf)); pos += len) {

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
 if (getstring(s_buf, "Find:") == 27)
 	return 1;
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
 reset_x();
 message(search_pos->str);
 return res;
}


int
cmd_find_beginning()
{
 if (getstring(s_buf, "Find:") == 27)
 	return 1;
 if (s_buf[0])
	move_first_line();

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

 for (pos = line->str; (pos = strstr( pos, s_buf ));) {
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
 if (getstring(s_buf, "Search:") == 27)
 	return 1;
 if (getstring(r_buf, "Replace:") == 27)
 	return 1;
/*
priv(s_buf, s); getchar();
priv(r_buf, s); getchar();
*/

 search_and_replace (get_head(), get_tail(), 0);
 redraw_screen();
 return 0;
}


int
cmd_replace_global()
{
 buffer_t *tmp = file; 
 message("GLOBAL replace");
 if (getstring(s_buf, "Search:") == 27)
 	return 1;
 if (getstring(r_buf, "Replace:") == 27)
 	return 1;
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
cmd_rename_global()
{
 buffer_t *tmp = file; 
 message("GLOBAL replace name");
 if (getstring(s_buf, "Search:") == 27)
 	return 1;
 if (getstring(r_buf, "Replace:") == 27)
 	return 1;
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
 if (getstring(s_buf, "Search:") == 27)
 	return 1;
 if (getstring(r_buf, "Replace:") == 27)
 	return 1;
 	
 search_and_replace (file->copy_start_pos, file->copy_end_pos, 0);
 redraw_screen();
 return 0;
}


int
cmd_rename_whole()
{
 if (getstring(s_buf, "Search:") == 27)
 	return 1;
 if (getstring(r_buf, "Replace:") == 27)
 	return 1;

 search_and_replace (get_head(), get_tail(), 1);
 redraw_screen();
 return 0;
}


int
cmd_rename_marks()
{
 if (getstring(s_buf, "Search:") == 27)
 	return 1;
 if (getstring(r_buf, "Replace:") == 27)
 	return 1;
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
cmd_repeat_rename_whole()
{
 search_and_replace (get_head(), get_tail(), 1);
 redraw_screen();
 return 0;
}


int
cmd_repeat_rename_marks()
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
 dprint("insert_lines: %p, %p, %p\n", place, buf_head, buf_tail);
 assert( place && buf_head && buf_tail ); 
 
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
 dprint("insert_lines_before: %p, %p, %p\n", place, buf_head, buf_tail);
 assert( place && buf_head && buf_tail ); //failed when undo comment sometimes
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
yank()
{
 if (copy_buffer_head)
	 free_lines( copy_buffer_head, NULL);
 
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
 redraw_screen();
 return 0;
}


int
delete_lines(line_t *l, line_t *m)
{
 line_t *prev, *next;

 if(l) prev = l->backw;
 
 if(m) next = m->forw;

 if(prev) prev->forw = next;
 
 if(next) next->backw = prev;
 
 free_lines(l, m);

 return 0;
}


int
del_lines(line_t *l, line_t *m)
{
 assert(l != NULL && m != NULL);
 
 if(!l || !m) return 1;
 
 yank();

 cl = (m->forw) ? m->forw : l->backw;
 
 delete_lines(l, m);
 
 redraw_screen();
 return 0;
}


int
cmd_del_block()
{
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
free_line(line_t *l)
{
 if (!l) return 0;
 
 free( l->str );
 free( l );
 return 0;
}

int 
free_lines(line_t *l, line_t *m)
{
 assert(l);
 
 if(!l)	return 0;

 line_t *next;
 while( l != NULL && l != m) {
	next = l->forw;
	free_line(l);
	l = next;
 }
 free_line(l);
 return 0;
}


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
 line_t *l = file->cur_line;

 if (!l) return l;

 while( l->backw != NULL )
	l = l->backw;
 return l;
}


line_t*
get_tail(){
 line_t *l = file->cur_line;

 if (!l) return l;

 while( l->forw != NULL )
	l = l->forw;
 return l;
}


int 
show_bar()
{
#ifndef NO_SHOW_BAR
 if(!file->cur_line) return 1;

 int line_no = cur_line_number();

 int y, x;
 int i;
 static char buf[200];
 getpos(&y, &x);
 moveyx(trm_rows-1, 1);

 printf(CLL);
 printf("cursor: %d, %d\tx_curs: %d\t", x, y, file->x_curs);
 printf("%1$d %2$x %2$c", offs, *(cur_str + offs) );
 printf("   x: %d", file->cur_line->x);

/* bits */
 char bits[9] = {0};
 int tmp_mask = cmd_mask;
 
 for (i = 0; i < 8; i++)
 {
	 bits[i] = (tmp_mask & 1) + '0';
	 tmp_mask = tmp_mask >> 1;
 }
 moveyx(trm_rows-1, 50);
 printf("cmd_mask: %s", bits);
 if (global) printf("   GLOBAL");

 moveyx(trm_rows, 1);
 printf(CLL);
 moveyx(trm_rows, 67);
 sprintf(buf, "_wintop:%d", _wintop);
 printf("%s", buf);
 moveyx(trm_rows, 50);
 sprintf(msg, "%x, %d", com_key, key_pressed[com_key] + key_stat[com_key]);
 printf( "    %s", msg);//msg - ?
 moveyx(trm_rows, 40);
 printf("%s", file->filename);
 moveyx(trm_rows, 20);
 printf("alt: ");
 if( _key_mode == 0 )
 	printf("OFF");
 else
 	printf("ON");
 moveyx(trm_rows, 1);
 printf("line: %d", line_no);
 moveyx(y, x);
#endif
 return 0;
}


int
show_buffers() {
 int i = 0, j;
 buffer_t *tmp;
 char buf[MAX_LINE]={0};
 
 tmp = file;
 for (;tmp->prev; tmp = tmp->prev);
 for (j = 0; tmp; tmp = tmp->next, j++)
 {
	if (tmp == file) 
 		i += sprintf(buf + i, "[%d]%s ", j + 1, tmp->filename);
 	else
 		i += sprintf(buf + i, "(%d)%s ", j + 1, tmp->filename);
 }
// message(buf);
 print_line_x(1, 1, buf);
 return 0;
}

int
show_hide_buffers()
{
 if (set_show_buffers)
 {
	_wintop--;
	message("");
	set_show_buffers = 0;
 }
 else
 {
// 	gmessage("else(set_sh.. == 0");
	_wintop++;
/*
	sprintf(msg, "_wintop:%d", _wintop);
	gmessage(msg);
*/
	show_buffers();
	set_show_buffers = 1;
 }
 resize();
 redraw_screen();
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
 if( (line = goto_line( n ) )) {
	file->cur_line = line;
	redraw_screen();
 }
 reset_x();
 return 0;
}


void 
sigsegv_handler(int i)
{
// sig_handler();
 fprintf(stderr, "SIGSEGV\n");
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
debug_exit()
{
 set_raw_mode(OFF);
}

void
sig_handler()
{
 int i, y, x;
 write_key_pressed();
 freecommands();
 freecommands_str();
 
 getpos(&y, &x);
 set_scroll_win(1, trm_rows);
 _wintop = 1;
 _winbot = trm_rows;
// moveyx(y, x);

/*
 for(i = 0; i < trm_rows - y - 2; i++)
 	scroll_up();
*/
 	
 clear_down();
 set_raw_mode(OFF);
 fclose(erlog);
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
change_color_mode(int col)
{
 int i;
 line_t *m1, *m2;

 color(col);
 
// redraw_screen();
 return 0;
}

int
cmd_change_color_mode()
{
	char ch_col_ch = getchar();
	switch (ch_col_ch) {
	case '0':
		change_color_mode(0);
		break;
	case '1':
		change_color_mode(31);
		break;
	case '2':
		change_color_mode(32);
		break;
	case '3':
		change_color_mode(33);
		break;
	case '4':
		change_color_mode(34);
		break;
	case '5':
		change_color_mode(35);
		break;
	case '6':
		change_color_mode(36);
		break;
	case '7':
		change_color_mode(37);
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
 {
 	open_file(argv[i]);
 }

	create_f = 0;

 redraw_screen();
 return 0;
}


int
cmd_create(int argn, char** argv)
{
 if (argn == 1)
	return 0;

 create_f = 1;
 
 int i;
 buffer_t *tmp;

 last_file = file;

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
cmd_set_autoindent()
{
 autoindent = (autoindent) ? 0 : 1; 
 sprintf(msg, "autoindent: %d", autoindent);
 message(msg);
 return 0;
}


int
set_autoindent(int argc, char** argv)
{
 if (argc != 1) return 1;
 cmd_set_autoindent();
}


int
indent_cur_line()
{
 if (file->cur_line->backw == NULL) return 1;

 int i;

 for (i = 0; isspace(file->cur_line->backw->str); i++);
 cur_str = malloc(i + 1);
 cur_str[i] = '\0';
 strncpy(cur_str, file->cur_line->backw->str, i);
// move_forwart(i);
// cmd_edit_line_end()

 return 0;
}


int
cur_len() {
 return strlen(cur_str);
}


int
do_command(int once) {
 char buf[MAX_LINE]={0};

 do {
	 if (getstring(buf, "Command:") == 27)
	 	continue;
	 	
	 if (strcmp(buf, "return") == 0 || strcmp(buf, "r") == 0)
	 	break;
	 
	 if (buf[0])
	 	strcpy(invoke_buf, buf);
	 	
	 command(invoke_buf);
 } while (!once);
 	
 return 0;
}

int
invoke_command()
{
 return do_command(1);
}


int
command_mode()
{
 return do_command(0);
}


#if 0
int
exec_shell(char* comm) {
 clear_all();
 moveyx(1, 1);
 set_scroll_win(1, trm_rows);
 setecho();
 system(comm);
 printf("\n[PRESS ANY KEY]");
 getchar();
 set_scroll_win(_wintop, _winbot);
 redraw_screen();
 setnoecho();
 resize();
 return 0;
}
#endif


int
do_shell_command (int once, char* str) 
{
 char buf[MAX_LINE]={0};
 int need_init=1;

 do {
	 if (!str)
	 {
		 if (getstring(buf, "Execute:") == 27)
		 {
		 	if (once) return 0;
			else continue;
		 }
	 }
	 else
	 	strcpy(buf, str);
	 	
 	 if (need_init) {
		 clear_all();
		 moveyx(1, 1);
		 set_scroll_win(1, trm_rows);
		 //setecho();
		 set_raw_mode(OFF);
		 need_init = 0;
	 }
	 	
	 if (strcmp(buf, "return") == 0 || strcmp(buf, "r") == 0)
	 	break;
	 
	 if (buf[0])
	 	strcpy(invoke_buf, buf);
	 	
	 system(invoke_buf);
 } while (!once);
 
 if (once)
 {
 	 printf("\n[PRESS ANY KEY]");
	 getchar();
 }
 set_scroll_win(_wintop, _winbot);
 redraw_screen();
 set_raw_mode(OFF);
 resize();
 return 0;
}

int
invoke_shell_command()
{
 return do_shell_command(1, NULL);
}

int
shell_command_mode()
{
 return do_shell_command(0, NULL);
}


int
get_compile_and_run_string()
{
 if (getstring(compile_and_run_buf, "Execute commands:") == 27)
	return 1;
}


int
make_run() {
 do_shell_command (1, compile_and_run_buf);
 return 0;
}

int
read_sequence()
{
 int i;
 char c;
 char term = com_key;
 
 save_cur();
 moveyx(trm_rows-1, 1);
 printf(CLL"%s", "Enter sequence:");
 
 for (i = 1; i < 255; i++)
 {
	c = getchar();
	
	if (c == term)
	{
		seq[i] = 0;
		break;
	}
	
	if (iscntrl(c))
	{
		printf("^%c", c + 64);
		seq[i] = c;
	}
	else
	if (isprint(c))
	{
		putchar(c);
		seq[i] = c;
	}
//seq[i] = c;
 }
 seq[0] = i - 1;
 rest_cur();
 return 0;
}

int
send_sequence()
{
 int i;
 
 uint32_t tmp = cmd_mask;
 cmd_mask = 0;
  
 for (i = seq[0]; i; i--)
	ungetc(seq[i], stdin);
 
 cmd_mask = tmp; 
 return 0;
}

int
isalnum_(char c) {
 if (isalnum(c) || c == '_')
	return 1;
 else
	return 0;
}

int
isspace_(char c) {
 if (isspace(c) && c != '\n')
 	return 1;
 else
 	return 0;
}

int
read_enfr_str()
{
 getstring(enframe_top, "Top frame:");
 getstring(enframe_bot, "Bottom frame:");
 return 0;
}

int
enframe (char* top, char* bot)
{
 line_t* l;
 
 l = make_line_before(file->copy_start_pos);
 l->str = malloc(strlen(top) + 1);
 l->x = 1;
 l->color = l->forw->color;
 strcpy(l->str, top);
 
 l = make_line_after(file->copy_end_pos);
 l->str = malloc(strlen(bot) + 1);
 l->x = 1;
 l->color = l->backw->color;
 strcpy(l->str, bot);
 
 redraw_screen();
 return 0;
}

int
enframe_cmd()
{
 enframe(enframe_top, enframe_bot);
 return 0;
}

int
enframe_ind (char* top, char* bot)
{
 line_t* l1;
 line_t* l2;
 
 l1 = make_line_before(file->copy_start_pos);
// l1->str = malloc(
 init_line(l1);
 l1->str = realloc(l1->str, strlen(l1->str) + strlen(top) + 1);
 
 l2 = make_line_after(file->copy_end_pos);
 l2->str = strdup(l1->str);
 l2->str = realloc(l2->str, strlen(l1->str) + strlen(bot) + 1);
 strcpy (l2->str, l1->str);
 l2->x = l1->x;
 l2->color = l1->color;
 
 strcat(l1->str, top);
 strcat(l2->str, bot);
 
 redraw_screen();
 return 0;
}

int
enframe_alt_ind_cmd()
{
 char top[MAX_LINE];
 char bot[MAX_LINE];
           
 getstring(top, "Enframe top:");
 getstring(bot, "Enframe bot:");
 enframe_ind(top, bot);
 return 0;
}

int
enframe_alt_noind_cmd()
{
 char top[MAX_LINE];
 char bot[MAX_LINE];
           
 getstring(top, "Enframe top:");
 getstring(bot, "Enframe bot:");
 enframe(top, bot);
 return 0;
}

int
set_tab(int argn , char** argv)
{
 int i = 0;
 if(argn == 1) {
 	sprintf(msg, "tab_len: %d", tab_len);
 	message(msg);
 }
 if(argn != 2) return 0;

 i = atoi(argv[1]); 
 if (i > 80) i = 80;
 if (i > 0)
 tab_len = i;
 redraw_screen();
 sprintf(msg, "tab_len: %d",  tab_len);
 message(msg);
 return 0;
}

int
set_debug_screen(int argn , char** argv)
{
 int i = 0;
 if(argn == 1) {
 	sprintf(msg, "dwin_size: %d", dwin_size);
 	message(msg);
 }
 if(argn != 2) return 0;

 i = atoi(argv[1]); 
 
/*
 	i = _wintop - dwin_bot;
*/
 
 if (dwin_bot - i > _wintop)
 if (i >= 0)
 dwin_size = i;
 resize();
 redraw_screen();
 sprintf(msg, "dwin_size: %d",  dwin_size);
 message(msg);
 return 0;
}


int
clear_debug_screen()
{
 int i;
 
 save_cur();
 
 for (i = dwin_top; i <= dwin_bot; i++)
 {
 	moveyx(i, 1);
 	clreol();
 }
 
 rest_cur();
 
 return 0;
}

int
inspect_key()
{
 int i;
 char c;
// int (*pfunc)() = NULL;
 cmd_t* cmd = NULL;
 
 message("enter key");
 c = getchar();
 cmd = command_char(c);
 if (!cmd)
	 snprintf(msg, MAX_LINE, "key not found");
 else
 	snprintf(msg, MAX_LINE, "%s", cmd->name);
 message(msg);
 
 return 0;
}


/*
#define K_TEST 'T'
*/

int
test() {
 dpriv(file->x_curs, d);
 print_cur_line();
 return 0;
}

int
test1() {
 dprint("\n");
 return 0;
}


int
script() {
dmessage("script");
 return 0;

#ifdef COMPILE_SCRIPT
 clear_all();
 line_t* head =  file->cur_line;
 script_t* scr;
 
 while(head->backw)
 	head = head->backw;
 
 scr = compile(head);
// printf("compile completed\n");
 
 if (scr)
 {
	 exec_script(scr);
 }
 else
 	printf("scr == NULL\n");
 	
 gmessage("debug script"); 	
 getchar(); 	
 redraw_screen();
 
#endif
 return 0;
}

//

