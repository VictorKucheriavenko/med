#include "med.h"

#define DELETE 01
#define INSERT 02
#define REPLACE 04

int do_undo();
int
fill_undo(int, line_t*, line_t*, line_t*,
			line_t*, line_t*);
typedef struct {
	int action;
	line_t *delete_begin, *delete_end;
	line_t *head, *tail;
	line_t *insert;
	int c_line;
	int offs;
	int x_curs;
	
} undo_t;