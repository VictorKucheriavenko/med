#ifndef _MEDH_
#define _MEDH_

#define LOG 10
//#define COMPILE_SCRIPT
//#define NO_SHOW_BAR

#include <stdio.h>
#include "macros.h"
#include "debug.h"

#define INIT_NUM 1000
#define MAX_LINE 2048
#define MAX_BUF 2048
#define MAX_STACK 256
#define MAX_SEQ 2048
#define BUFSIZE 2048
#define NCOLS 80
#define NROWS 25
#define X_MODE_ 23 

#define ON 1
#define OFF 0
#define INIT 3

//#define BAK_CUR_FOLDER
 
#define ESC 27
#define BCSP 0x7f
#define DEL 0x08

#define \
debug()\
 printf("debug:%s:%d\n", __func__, __LINE__);\

#define \
prvar(var, f)\
 fprintf(stderr, #var":%"#f"\n", (var));

#define cl file->cur_line
#define cur_str file->cur_line->str
#define offs file->offset

typedef struct line {
	char* str;
	int len;
	int x;
	int color;
	int no;
	struct line *forw;
	struct line *backw;
} line_t;

typedef struct file_buffer {
	line_t* cur_line;
	line_t *copy_start_pos; //from...
	line_t *copy_end_pos;
	line_t *use_mark1;
	line_t *use_mark2;
	line_t* marks[13];
	line_t* last_mark;
	line_t* cur_mark;
	int offset;
	int x_curs;
	int y_curs;
	int x_cur[MAX_STACK];
	int y_cur[MAX_STACK];
	int top;
	char filename[MAX_LINE];
/*
	int fd;
	FILE* f;
*/
	
	int x_dwin;
	int y_dwin;
	
	struct file_buffer *next;
	struct file_buffer *prev;
} buffer_t;

typedef int (*func_t)();


#include "function.h"

#endif

