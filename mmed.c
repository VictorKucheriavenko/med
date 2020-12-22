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
//#include "find.h"
#include "med.h"
#include "keys.h"
#include <errno.h>

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




/***** global variables *****/
line_t *copy_buffer_head = NULL;
line_t *copy_buffer_tail = NULL;

int _key_mode = 0;
int _switch_key_mode = 0;
int _cols, _rows;
int _wintop, _winbot, _wincur;
int _width;
int _h_scroll = 10;

int readonly;
int create_f;
int options;

buffer_t* last_file;

int
main( int argn, char *argv[])
{
 int i;

 setvbuf(stdout, NULL, _IONBF, 0);
 setvbuf(stdin,  NULL, _IONBF, 0);

 if (argn == 1)
	return 0;

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
 atexit( &exit_handler );

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

 moveyx(_wincur, 1);
 redraw_screen();
/*
printf("debug %d\n", __LINE__);
getchar();
*/
 make_commands();

 while( 1 ) {
loop:
	com_key = getchar();

	message("");
/*
switch
*/
	switch( com_key ) {
	case K_CMD:
		invoke_command();
		break;
	case K_TEST:
		break;
	case K_F_BUF_CLOSE:
		close_file();
		redraw_screen();
		break;
	case K_F_BUF_PREV:
		if (_key_mode == 0)
			go_mark_prev();
		else
			file_buffer_prev();
		break;
	case K_F_BUF_NEXT:
		if (_key_mode == 0)
			go_mark_next();
		else
			file_buffer_next();
		break;
	case K_F_BUF_GO_1:
		if (_key_mode == 0)
			go_mark_number(1);
		else
			file_buffer_go(1);
		break;
	case K_F_BUF_GO_2:
		if (_key_mode == 0)
			go_mark_number(2);
		else
			file_buffer_go(2);
		break;
	case K_F_BUF_GO_3:
		if (_key_mode == 0)
			go_mark_number(3);
		else
			file_buffer_go(3);
		break;
	case K_F_BUF_GO_4:
		if (_key_mode == 0)
			go_mark_number(4);
		else
			file_buffer_go(4);
		break;
	case K_F_BUF_GO_5:
		if (_key_mode == 0)
			go_mark_number(5);
		else
			file_buffer_go(5);
		break;
	case K_F_BUF_GO_6:
		if (_key_mode == 0)
			go_mark_number(6);
		else
			file_buffer_go(6);
		break;
	case K_F_BUF_GO_7:
		if (_key_mode == 0)
			go_mark_number(7);
		else
			file_buffer_go(7);
		break;
	case K_F_BUF_GO_8:
		if (_key_mode == 0)
			go_mark_number(8);
		else
			file_buffer_go(8);
		break;
	case K_F_BUF_GO_9:
		if (_key_mode == 0)
			go_mark_number(9);
		else
			file_buffer_go(9);
		break;
	case K_F_BUF_GO_0:
		if (_key_mode == 0)
			go_mark_last();
		else
			file_buffer_go_last();
		break;
	case K_CHNG_COL:
		ch_col_ch = getchar();
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
		break;
	case K_REDRAW:
		redraw_screen();
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
	case K_CUR_UP:
		cur_up();
		break;
	case K_CUR_DOWN:
		cur_down();
		break;
	case K_UP:
		move_up(1);
		break;
	case K_UP1:
		move_up((_winbot - _wintop) / 2);
		break;
	case K_UP2:
		move_first();
		break;
	case K_DOWN:
		move_down(1);
		break;
	case K_DOWN1:
		move_down((_winbot - _wintop) / 2);
		break;
	case K_DOWN2:
		move_last();
		break;
	case K_LEFT:
		move_backward(1);
		break;
	case K_LEFT1:
		move_backward(10);
		break;
	case K_LEFT2:
		move_first_col();
		break;
	case K_RIGHT:
		move_forward(1);
		break;
	case K_RIGHT1:
		move_forward(10);
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
	case K_GOWORDBACK:
		move_word_back();
		break;
	case K_DELLINE:
		del_line();
		break;
/*
	case K_DELLINE:
		cmd_del_line();
		break;
*/
	case K_MKLINE:
		if (_key_mode == 1) {
			file->cur_line = make_line_before(file->cur_line);
			init_line(file->cur_line);
			moveyx(file->y_cur, 1);
		}
		else {
			file->cur_line = make_line_after(file->cur_line);
			init_line(file->cur_line);
			moveyx(file->y_cur, 1);
		}
		redraw_screen();
		reset();
		break;
	case K_FIND1: 
		getstring(search_buf);
		if(_key_mode == 0)
			find_str();
		else
		if(_key_mode == 1)
	 		find_beginning();
		break;
	case K_FIND2: 
		getstring(search_buf);
		find_name();
		break;
	case K_REFIND: 
		if (file->cur_line->forw)
		if(file->search_pos)
		file->cur_line = file->cur_line->forw;
		if(_key_mode == 0)
			find_str();
		else
		if(_key_mode == 1)
			find_beginning();
		redraw_screen();
		break;
	case K_REFIND2:
		find_name();
		break;
	case K_SRCH_RPL: 
		getstring(s_buf);
		getstring(r_buf);

		if(_key_mode == 1)
			search_and_replace (get_head(), get_tail());
		if(_key_mode == 0) {
			set_mark2();
			i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);
			if( !i ) 
			search_and_replace(file->copy_start_pos, file->copy_end_pos);
		}
		break;
	case K_REPEAT_RPL: 
		if(_key_mode == 1)
			search_and_replace(get_head(), get_tail());
		if(_key_mode == 0) {
			set_mark2();
			i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);
			if (!i) search_and_replace(file->copy_start_pos, file->copy_end_pos);
		}
		break;
	case K_SRCH_RPL2: 
		getstring(s_buf);
		getstring(r_buf);

		if(_key_mode == 1)
			search_and_replace_name(get_head(), get_tail());
		if(_key_mode == 0) {
			set_mark2();
			i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);
			if(!i) search_and_replace_name(file->copy_start_pos, file->copy_end_pos);
		}
		break;
	case K_REPEAT_RPL2:
		if(_key_mode == 1)
			search_and_replace_name(get_head(), get_tail());
		if(_key_mode == 0) {
			set_mark2();
			i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);
			if (!i) search_and_replace_name(file->copy_start_pos, file->copy_end_pos);
		}
		break;
	case K_WRITE_ALL:
		save_all();
		break;
	case K_WRITE: 
		save_file();
		break;
	case K_QUIT:
		clear_all();
		key_pressed[com_key]++;
		sprintf(msg, "%x, %d", com_key, key_pressed[com_key] + key_stat[com_key]);
		show_bar();
		while(1) {
			close_file();
		}
		quit();
		break;
	case K_DEL:
		//del_char_right();
		del_char();
		break;
	case K_DEL_WORD:
		del_word();
		if( *(file->cur_line->str + file->offs) == 0)
			move_backward(1);
		break;
	case K_CHANGE_WORD:
		del_word();
		insert_string();
		if( *(file->cur_line->str + file->offs) == 0)
			move_backward(1);
		break;
	case K_CASE_WORD:
		case_word();
		break;
	case K_CASE_WORD_BACKW:
		case_backward();
		break;
	case K_CASE_CHAR:
		case_char();
		break;
	case K_INS_MOD:
		if( _key_mode == 1 )
	 	insert_string_after();
		if( _key_mode == 0 )
	 	insert_string();
		break;
	case K_INS_MOD1:
		moveyx(file->y_cur, 1);
		setecho();
		if (_key_mode == 1) {
			file->cur_line = make_line_before(file->cur_line);
		}
		else {
			file->cur_line = make_line_after(file->cur_line);
		}
		init_line(file->cur_line);
		redraw_screen();
		//moveyx(file->y_cur, 1);
		reset();
		insert_string();
		redraw_screen();
		break;

	case K_INS_PREPEND:
		move_first_col();
	 	insert_string();
		break;
	case K_INS_APPEND:
		move_last_col();
	 	insert_string_after();
		break;

	case K_INS_INDENT:
		insert_indent();
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
	case K_CHANGE_CHAR:
		c = getchar();
		change_char(c);
		break;

	case K_SET_MARK:
		c = getchar();
		if (c >= '0' && c <= '9')
			set_mark(c - '0');
		break;
	case K_GO_MARK_NUMB:
		c = getchar();
		if (c >= '0' && c <= '9')
			go_mark_number(c - '0');
		break;
	case K_SET_COPY_START:
		set_mark1();
		break;
	case K_COPY:
		yank();
		break;
	case K_PASTE:
		if(!_key_mode)
		cmd_paste();
		else
		cmd_paste_before();
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
		key_valid = 0;
		break;
	}//end switch

	if( key_valid ) {
		key_pressed[com_key]++;
	}
	sprintf(msg, "%x, %d", com_key, key_pressed[com_key] + key_stat[com_key]);
	key_valid = 1;

	debug_bar();
	show_bar();
	if(_switch_key_mode == 1) {
	_key_mode = (_key_mode) ? 0 : 1;
	_switch_key_mode = 0;
	}
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
invoke_command() {
 char buf[MAX_LINE];

 getstring(buf);
 command(buf);
 return 0;
}

int
save_cur() {
 getpos(&file->y_cur, &file->x_cur);
 return 0;
}

int
rest_cur() {
 moveyx(file->y_cur, file->x_cur);
}

int
getsize () {
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
 if( !file->cur_line )
   return 0;

 if( file->cur_line->len == 0 )
	del_line();
 del(&file->cur_line->str, file->offs+1, 1);
 print_cur_line();
 file->cur_line->len = strlen(file->cur_line->str);
 if(file->offs == file->cur_line->len)
	move_backward(1);
 return 0;
}

int
del_char_right()
{
 char buf[MAX_LINE];
 int len = strlen( file->cur_line->str );
//*(file->cur_line->
 memmove(file->cur_line->str + file->offs, file->cur_line->str + file->offs + 1, len + 1);
 printf( CLR"%s", file->cur_line->str + file->offs);
 moveyx(13, file->offs+1);
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
 redraw_line();
 return 0;
}

int
case_backward()
{
 if(file->offs == 0) return 0;

 int i;
 for(i = file->offs - 1; (isalpha(file->cur_line->str[i]) || file->cur_line->str[i] == '_') && i >= 0; i--) {
// if(islower(file->cur_line->str[i]))
   file->cur_line->str[i] = toupper(file->cur_line->str[i]);
// else
//   file->cur_line->str[i] = tolower(file->cur_line->str[i]);
 }
 redraw_line();
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
 redraw_line();
 return 0;
}

int move_word_back()
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
 if(!file->cur_line) return 1;

 char ch;
 ch = getchar();
 line_t *line;
 set_mark2();
 int  i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);
 if(i) return 1; 

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
		for(; (isalpha(*(buf+i)) || *(buf+i) == '_') && i >= 0; i--)
			*(buf+i) = toupper(*(buf+i));
		i = tmp;
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
getstring(char* buf) {
 save_cur();
 moveyx(_rows-1, 1);
 printf(CLL);
 readstring(buf, MAX_LINE);
 rest_cur();
}


int
insert_string()
{
 if(!file->cur_line) return 1;

 int res, ret;
 char ch;
 char *tmp;
 int tmp_len = 0;

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
	  file->cur_line->x = 1;
  	  file->cur_line->str = malloc( tmp_len + 1 );
	  strcpy(file->cur_line->str, tmp);
	  file->cur_line->backw->str =
		realloc( file->cur_line->backw->str, file->cur_line->backw->len - tmp_len+1 );
		file->cur_line->backw->str[file->cur_line->backw->len - tmp_len] = 0;
	  file->offs = 0;
	  file->x_curs = 1;
	  moveyx(_wincur, file->x_curs);

	  redraw_screen();
	  break;

 	  case 'u'-0x60:
 	  	case_backward();
 	  break;

	   default:
	  insert_ch(&file->cur_line->str, file->offs, ch);
	  if(ch == '\t') {
		file->x_curs = nexttab1();
		sprintf(msg, "nexttab():%d, file->x_curs:%d", nexttab(), file->x_curs );
		message(msg);
	  }
	  else {
	  file->x_curs++;
	  }
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
	redraw_line();
	moveyx(_wincur, file->x_curs);

	show_bar();
	debug_bar();

 } while ( ch != 0 );
 file->cur_line->len = strlen( file->cur_line->str);
 return ret; 
}

int
insert_string_after()
{
 if(!file->cur_line) return 1;

 int ret;
 save_cur();
 file->offs++;
 file->x_curs++;
 moveyx(file->y_cur, file->x_curs);
 ret = insert_string();
 file->offs--;
 file->x_curs--;
 moveyx(file->y_cur, file->x_curs);
 return ret;
}

int
concat_lines()
{
 if(!file->cur_line->forw)
   return 0;

 int i;
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
init_line( line_t *ln )
{
 ln->str = malloc(1);
 *(ln->str) = '\0';
 ln->len = 0;
 ln->x = 1;
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
 readstring(buf, MAX_LINE);
 insert_str( &file->cur_line->str, file->offs + 1, buf);
 file->cur_line->len = strlen( file->cur_line->str);
 moveyx(file->y_cur, 1);
 printf(CLL"%s", file->cur_line->str  );
 rest_cur();

 tcsetattr(0, TCSANOW, &restore);
 return 0;
}

int
change_char(char c)
{
 if(!file->cur_line) return 1;

 file->cur_line->str[file->offs] = c;
 redraw_line();
}


int
insert_space()
{
 if(!file->cur_line) return 1;
 insert_ch( &file->cur_line->str, file->offs, ' ');
 file->cur_line->len++;
 print_cur_line();
 return 0;
}

int
insert_char(char ch)
{
 if( !file->cur_line )  return 1;

 insert_ch( &file->cur_line->str, file->offs, ch);
 file->cur_line->len++;
 redraw_line();
 position_cursor();
 return 0;
}


int
insert_char_next(char ch)
{
 if(!file->cur_line) return 1;
 save_cur();
 moveyx(file->y_cur, file->x_cur + 1);
 file->offs++;
 insert_char(ch);
 move_forward(1);
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
//	exit(1);
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
 gmessage("line == NULL");
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
 int len = strlen(fname);
 char *fname2 = malloc(len+1+4);
 strcpy(fname2, fname);
 strcat(fname2, ".mbk");
 fd1 = open(fname, O_RDONLY);
 fd2 = open(fname2, O_WRONLY | O_CREAT | O_TRUNC, 0666);
 while(ret != 0) {
   ret = read(fd1, buf, BUFSIZE);
   write(fd2, buf, ret);
 }
 close(fd1);
 close(fd2);
}

int
remove_backup(char* fname) {
 char buf[BUFSIZE];
 int len = strlen(fname);
 char *fname2 = malloc(len+1+4);
 strcpy(fname2, fname);
 strcat(fname2, ".mbk");
 remove(fname2);
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

 if (file->cur_line == NULL)
	return 0;

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
move_up(int n)
{
 int i;

 if( file->cur_line == NULL )
	return 0;

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
move_first()
{
 if(!file->cur_line) return 1;

 while( file->cur_line->backw != NULL )
	file->cur_line = file->cur_line->backw;
 redraw_screen();
 return 0;
}

int
move_last()
{
 if(!file->cur_line) return 1;

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
 if(!file->cur_line) return 1;

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
if ( !file->cur_line )
	return 0;

 move_forward(file->cur_line->len);
 return 0;
}


int
move_forward1(int n)
{
 if (!file->cur_line) return 1;

 int i;

 for( i = 0; i < n; i++ )
 if ( file->offs + 1 < file->cur_line->len ) { 
	 if( *(file->cur_line->str + file->offs + 0) == '\t' ) {
		file->x_curs = nexttab1();
	 }
	 else
		file->x_curs++;

	 if(file->x_curs <= _width) {
		moveyx(_wincur, file->x_curs);
	 }
	 else {
		file->cur_line->x -= _h_scroll;
		print_cur_line();
		file->x_curs -= _h_scroll;
		moveyx(_wincur, file->x_curs);
	 }
	 file->offs++;
 }
  return 0;
}

int
move_forward(int n)
{
 if (!file->cur_line) return 1;

 int i;
 int need_reprint = 0;

 for( i = 0; i < n; i++ )
 if ( file->offs + 1 < file->cur_line->len ) { 
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
 int i;
 i =  (file->x_curs + (file->cur_line->x - 1) - 1) / _tabs * _tabs + _tabs + 1
		- (file->cur_line->x - 1);
 return i;
}


/*
word
*/

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
 if(!file->cur_line) return 1;

 int i = 0;
 i = get_bow();
 move_first_col();
 move_forward(i);
}

int
move_eow()
{
 if(!file->cur_line) return 1;

 int i = 0;
 i = get_eow();
 move_first_col();
 move_forward(i);
}

int
del_word()
{
if( !file->cur_line )
	return 0;

 int i, to;

 to = ( isalnum( *(file->cur_line->str + file->offs) ) ) ? get_eow() : get_bow();


 del( &file->cur_line->str, file->offs + 1, to - (file->offs + 0) );
 file->cur_line->len = strlen(file->cur_line->str);
 print_cur_line();

/*
if( *(file->cur_line->str + file->offs) == 0)
  move_backward(1);
*/

 char buf[200];
 sprintf(buf, "file->offs:%d", file->offs);
 message(buf);

return 0;
}



int
del_line()
{
	line_t *prev;
	line_t* tmp;

 if( file->cur_line == NULL)
	return 1;
 else

 if( file->cur_line->backw == NULL && file->cur_line->forw == NULL ) {
	free(file->cur_line->str);
	free(file->cur_line);
	file->cur_line = NULL;
	redraw_screen();
	return 0;
 }
 else

 if( file->cur_line->forw == NULL && file->cur_line->backw != NULL) {
	tmp = file->cur_line; 
	file->cur_line = file->cur_line->backw;
	file->cur_line->forw = NULL;
	free( tmp->str );
	free( tmp );
	scroll_down();
	redraw_screen();
	//move_up(1);
 }
 else

 if( file->cur_line->backw == NULL && file->cur_line->forw != NULL ) {

	tmp = file->cur_line; 
	file->cur_line = file->cur_line->forw;
	file->cur_line->backw = NULL;
	//tmp->backw->forw = file->cur_line;
	free( tmp->str );
	free( tmp );
	//move_down(1);
 }
 
 else {
 tmp = file->cur_line; 
 file->cur_line = file->cur_line->forw;
 file->cur_line->backw = tmp->backw;
 tmp->backw->forw = file->cur_line;
 free( tmp->str );
 free( tmp );
 }
 redraw_screen();
 return 0;
}

int
del_line2(line_t *line)  /*file->cur_line*/
{
 if(!line)
 return 1;

 line_t *next, *prev, *tmp;
 prev = line->backw;
 next = line->forw;
 free(line->str);
 free(line);
 if(prev)
 prev->forw = next;
 if(next)
 next->backw = prev;
 return 0;
}

cmd_del_line()
{
 line_t *tmp;
 if(file->cur_line->forw)
   tmp = file->cur_line->forw;
 else
 if(file->cur_line->backw)
   tmp = file->cur_line->backw;
 else
 tmp = NULL;
 del_line2(file->cur_line);
 file->cur_line = tmp;
 redraw_screen();
 return 0;
}

int del_lines2()
{
 line_t *next, *prev, tmp;
}


/*
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
*/


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
 print_line_x_color(pr_line->x, _winbot, pr_line->str, pr_line->color);
}

/*
print_lines_cur(int s, int n)
{
 int i;
 line_t* pr_line;
 pr_line = file->cur_line;
 for( i = 0; i < s && pr_line->forw != NULL; i++ )
	pr_line = pr_line->forw; 
 for( i = 0; pr_line->forw != NULL && i < n; i++ ) {
	printf("\033[2K%s\n", pr_line->str);
	pr_line = pr_line->forw;
 }
}
*/


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
 getpos(&y, &x);
 moveyx(line, 1);
 printf(CLL);
/*
 moveyx(X_MODE_, 1);
 printf(CLL);
*/
 tmp = o;
 while(o <= _width && str[i] != 0) {
	if( !( o <= 0 ) )	
	   moveyx(line, o);
/*
	else
	   moveyx(X_MODE_, _cols + o);
*/
	if (str[i] == '\t')
	   o = nexttab3( o - (tmp-1), tmp - 1);
	else {
	   if( !( o <= 0 ) )	
	   write(1, (str+i), 1);
	   o++;
	}
	i++;
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
 printf("\x1B[%dm", col);
/*
 moveyx(X_MODE_, 1);
 printf(CLL);
*/
 tmp = o;
 while(o <= _width && str[i] != 0) {
	if( !( o <= 0 ) )	
	   moveyx(line, o);
/*
	else
	   moveyx(X_MODE_, _cols + o);
*/
	if (str[i] == '\t')
	   o = nexttab3( o - (tmp-1), tmp - 1);
	else {
	   if( !( o <= 0 ) )	
	   write(1, (str+i), 1);
	   o++;
	}
	i++;
 }
 moveyx(y, x);
 printf("\x1B[0m");
}


int
print_cur_line()
{
 print_line_x_color(file->cur_line->x, _wincur, file->cur_line->str,
			file->cur_line->color);
 return 0;
}

int
redraw_screen()
{

 int i, n;
 line_t* ln;
 clear_all();

 if(!file->cur_line)
 return 1;

 ln = file->cur_line;
 save_cur();
 calculate_all_lengths();

 if(!ln)
  return 1;

 for( n = _wincur; n > _wintop && ln->backw != NULL; ) {
	ln = ln->backw;
	n--;
 }
 for( i = 0; i < _winbot - n - 0  && ln->forw != NULL; i++) {

/* colored markers */
	if (ln == file->copy_start_pos) ln->color = 31;
	if (ln == file->copy_end_pos) ln->color = 32;
	if (ln == file->copy_start_pos && ln == file->copy_end_pos)
	ln->color = 33;
	if (ln != file->copy_start_pos && ln != file->copy_end_pos)
	ln->color = 0;
/********		*/

	print_line_x_color(ln->x, n+i, ln->str, ln->color);
	ln = ln->forw;
 }
 print_line_x_color(ln->x, n+i, ln->str, ln->color);
 rest_cur(); /* bug */
}

int
redraw_line()
{
 print_line_x_color(file->cur_line->x, _wincur, file->cur_line->str, file->cur_line->color);
 return 0;
}



char* 
find_str()
{
 int i;
 char* res = NULL;

 moveyx(_wincur, 1);
 file->search_pos = file->cur_line;
 while(res == NULL && file->search_pos != NULL) {
//	res = strcasestr(file->search_pos->str, search_buf);
//	res = strstr(file->search_pos->str, search_buf);
	res = find_ss(search_buf, file->search_pos->str);

	if ( res != NULL )
	   break;
	file->search_pos = file->search_pos->forw;
 }
  if(res == NULL) {
  message("no matching");
  return res;
 }
 file->cur_line = file->search_pos;
 redraw_screen();
 reset();
 message(file->search_pos->str);
 return res;
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
find_beginning()
{
 int i;
 int res = -1;

 moveyx(_wincur, 1);
 file->search_pos = file->cur_line;
 while(res != 0 && file->search_pos != 0) {
	res = strncmp(file->search_pos->str, search_buf, strlen(search_buf));
	if ( res == 0 )
	   break;
	file->search_pos = file->search_pos->forw;
 }
  if(res != 0) {
  message("no matching");
  return res;
 }
 file->cur_line = file->search_pos;
 redraw_screen();
 reset();
 message(file->search_pos->str);
 return res;
}


int
search_and_replace(line_t* start_srch, line_t* end_srch)
{
 if (file->cur_line == NULL)
	return 0;

 int i;
 int len, s_len, r_len, o;
 char *pos;
 line_t *line = start_srch;
 static char buf[MAX_LINE];

 s_len = strlen(s_buf);
 r_len = strlen(r_buf);

 if( s_len )
 for( ;line != end_srch->forw; line = line->forw )
 for (pos = line->str; pos = strstr( pos, s_buf ); pos += r_len) {
	o = pos - line->str;
	strncpy(buf, line->str, o);
	buf[o] = 0;
	strcat(buf, r_buf);
	strcat(buf, pos + s_len);
	line->len = strlen(buf);
	line->str = realloc(line->str, line->len + 1);
	strcpy(line->str, buf);

	pos = line->str + o;
   }

 redraw_screen();
 return 0;
}

int
search_and_replace_name(line_t* start_srch, line_t* end_srch)
{
 if (file->cur_line == NULL)
	return 0;

 int i;
 int len, s_len, r_len, o;
 char *pos;
 line_t *line = start_srch;
 static char buf[MAX_LINE];

 s_len = strlen(s_buf);
 r_len = strlen(r_buf);

 if( s_len )
 for( ;line != end_srch->forw; line = line->forw )
 for (pos = line->str; pos = strstr( pos, s_buf ); pos += r_len) {
	o = pos - line->str;

	if ((isalnum (pos[-1]) && pos != line->str) || isalnum (pos[s_len])
		|| pos[-1] == '_' || pos[s_len] == '_')
		continue;

	strncpy(buf, line->str, o);
	buf[o] = 0;
	strcat(buf, r_buf);
	strcat(buf, pos + s_len);
	line->len = strlen(buf);
	line->str = realloc(line->str, line->len + 1);
	strcpy(line->str, buf);

	pos = line->str + o;
   }

 redraw_screen();
 return 0;
}

int
search_and_replace_name2(line_t* start_srch, line_t* end_srch)
{
 if (file->cur_line == NULL)
	return 0;

 int i;
 int len, s_len, r_len, o;
 line_t *line = start_srch;
 char *pos = line->str;
 static char buf[MAX_LINE];

 s_len = strlen(s_buf);
 r_len = strlen(r_buf);

 if( s_len )
 for (o = 0; line != end_srch->forw; line = line->forw, o = 0)
 while(pos = strstr (line->str + o, s_buf)) {
	o = pos - line->str + r_len;

	if ((isalnum (pos[-1]) && pos != line->str) || isalnum (pos[s_len])
		|| pos[-1] == '_' || pos[s_len] == '_')
		continue;

	strncpy(buf, line->str, o);
	buf[o] = 0;
	strcat(buf, r_buf);
	strcat(buf, pos + s_len);
	line->len = strlen(buf);
	line->str = realloc(line->str, line->len + 1);
	strcpy(line->str, buf);

	pos = line->str + o;
 }

 redraw_screen();
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
 dest->x = source->x;
 source = source->forw;
 *buf_head = dest;
 while( source != end->forw ) {
	dest = make_line_after(dest);
	dest->str = malloc( strlen( source->str ) + 1 );
	strcpy( dest->str, source->str );
	dest->len = source->len;
	dest->x = source->x;
	source = source->forw;
 }
 *buf_tail = dest;
 message("copy_lines... DONE");
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
set_mark(int n)
{
 if(n < 0 || n > 9)
	return 1;

 file->marks[n] = file->cur_line;
 return 0;
}

int
go_mark_number(int n) {
 if (n < 0 || n > 13) return 1;

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


/*
int 
go_mark_next()
{
 go_mark_number(++(file->cur_mark));
 return 0;
}


int 
go_mark_prev()
{
 go_mark_number(--(file->cur_mark));
 return 0;
}
*/

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
 set_mark2();
 int  i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);
 if(i) return 1; 

 free_lines( copy_buffer_head, NULL);

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

int
cmd_del_lines()
{
 set_mark2();
 int i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);
 if(i)
   return 1;

 line_t *l = file->copy_start_pos;
 line_t *m = file->copy_end_pos;
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
 file->cur_line = prev;
 else
 file->cur_line = next;
 redraw_screen();
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


/************* end buffer functions **********************/
line_t*
get_head(){
 int i;
 line_t *line = file->cur_line;

 if (!line) return line;

 while( line->backw != NULL )
	line = line->backw;
 return line;
}

line_t*
get_tail(){
 int i;
 line_t *line = file->cur_line;

 if (!line) return line;

 while( line->forw != NULL )
	line = line->forw;
 return line;
}

int 
debug_bar()
{
 if(!file->cur_line) return 1;

 int y, x;
 getpos(&y, &x);
 save_cur();
 moveyx(_rows-1, 1);

 printf(CLL);
// printf("cur_mark: %d\t\t", file->cur_mark);
// printf(CLL"len:%d\\%d\t", strlen(file->cur_line->str), file->cur_line->len);
 printf("y:%d x:%d\tx_curs: %d\t", y, x, file->x_curs);
 printf("%1$d %2$x %2$c", file->offs, *(file->cur_line->str + file->offs) );
// printf("  :%1$d %2$x %2$c", file->offs + 1, *(file->cur_line->str + file->offs + 1) );
 printf("   x: %d", file->cur_line->x);

 rest_cur();
 return 0;
}

int
show_bar()
{
 if(!file->cur_line) return 1;

 int line_no = cur_line_number();
 save_cur();
 moveyx(_rows, 1);
 printf(CLL);
 moveyx(_rows, 55);
 printf( "    %s", msg);
 moveyx(_rows, 40);
 printf("%s", file->filename);
 moveyx(_rows, 20);
 printf("_key_mode: %d", _key_mode);
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

void 
exit_handler()
{
 sig_handler();
}

int
quit()
{
 exit(EXIT_SUCCESS);
 return 0;
}

void
sig_handler()
{
 write_key_pressed();
 freecommands();
 set_scroll_win(1, _rows);
 setecho();
 setcanon();
 printf("sig_handler()\n");
 clear_all();
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
file_buffer_go_last(int n) {
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
	key_pressed[i] += buf[i];
 i = fwrite(key_pressed, sizeof(int), 256, f);
 rewind(f);
 i = fread(buf, sizeof(int), 256, f);

 fclose(f);
 return 0;
}


int
change_color(int col)
{
 int i;
 line_t *m1, *m2;

 set_mark2();
 i = arrange_markers(&file->copy_start_pos, &file->copy_end_pos);

 if ( i ) return 0;

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