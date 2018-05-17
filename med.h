#ifndef _MEDH_
#define _MEDH_

#include <stdio.h>

#define INIT_NUM 1000
#define MAX_LINE 2048
#define MAX_BUF 2048
#define BUFSIZE 2048
#define NCOLS 80
#define NROWS 25
#define X_MODE_ 23 

 
#define ESC 27
#define BCSP 0x7f
#define DEL 0x08

typedef struct line {
	char* str;
	int len;
	int x;
	int color;
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
	int offs;
	int x_curs;
	int x_cur;
	int y_cur;
	char filename[MAX_LINE];
	int fd;
	FILE* f;
	
	struct file_buffer *next;
	struct file_buffer *prev;
} buffer_t;

typedef int (*func_t)();


#if 0
#endif /* if 0 */

#include "function.h"

#endif