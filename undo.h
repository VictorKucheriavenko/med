#include "med.h"

#define NOTHING 0
#define DELETE 1
#define INSERT 2
#define REPLACE 3

int do_undo();
int fill_undo(int action, line_t *start_copy, line_t *end_copy, line_t *insert_pos,
					line_t *delete_begin, line_t *delete_end);
int fill_redo(int action, line_t *start_copy, line_t *end_copy, line_t *insert_pos,
					line_t *delete_begin, line_t *delete_end);
			
typedef struct {
	int action;
	line_t *delete_begin, *delete_end;
	line_t *head, *tail;
	line_t *insert;
	int c_line;
	int offset;
	int x_curs;
} undo_t;