#include "undo.h"
#include "med.h"
#include <stdlib.h>
#include <string.h>

extern int gl_deb;

extern char msg[MAX_LINE];

extern buffer_t* file;

undo_t undo, redo;

extern func_t* pfunc;


int
fill_undo_undo()
{
 free_lines(redo.head, redo.tail);
 redo.head = NULL;
 redo.tail = NULL;
 
 switch (undo.action) {
 case NOTHING:
 	return 0;
 case DELETE:
	redo.action = INSERT;
 case REPLACE:
	if (undo.action == REPLACE) 
		redo.action = REPLACE;
		
	copy_lines((undo.delete_begin) ? undo.delete_begin->forw : get_head(), 
		(undo.delete_end) ? undo.delete_end->backw : get_tail(), 
		&redo.head, &redo.tail);
		
 	redo.insert = undo.delete_begin;
 	
	redo.delete_begin = undo.delete_begin;
	redo.delete_end = undo.delete_end;
	
	break;
 case INSERT:
	redo.action = DELETE;
	
	redo.delete_begin = undo.insert;
	redo.delete_end = undo.insert ? undo.insert->forw : NULL;
	break;
 default:
	break;
 }
 redo.c_line = cur_line_number();
 redo.offset = offs;
 redo.x_curs =file->x_curs;
 
 return 0;
}

int
do_undo()
{
 fill_undo_undo();
 line_t *tmp;
 undo_t tmp1;
 
 if (!undo.action)
 	return 1;
 
/*
 if (undo.delete_begin == ( get_head()))
 if (undo.delete_end == ( get_tail()))
 	cl = 0;
*/
 
 switch (undo.action) {
 case NOTHING:
 	return 0;
 case DELETE:
 case REPLACE:
	if (undo.delete_begin && undo.delete_end)
		tmp = undo.delete_begin;
	
	if (!undo.delete_begin)
		tmp = undo.delete_end;
		
	if (!undo.delete_end)
		tmp = undo.delete_begin;
		
	undo.delete_begin = 
		(!undo.delete_begin) ? get_head() : undo.delete_begin->forw;
		
	undo.delete_end = 
		(!undo.delete_end) ? get_tail() : undo.delete_end->backw;
		
	 delete_lines(undo.delete_begin, undo.delete_end);
	cl = tmp;
	if (undo.action == DELETE)
		break;
 case INSERT:
	if (!undo.insert)
		if (!get_head())
			cl = undo.head;
		else
			insert_lines_before(get_head(), undo.head, undo.tail);
	else
		insert_lines(undo.insert, undo.head, undo.tail);
	undo.head = NULL;
	undo.tail = NULL;
	break;
 default:
	return 0;
	break;
 }
	
 set_cur_line(undo.c_line);
 offs = undo.offset;
 file->x_curs = undo.x_curs;
/*
 undo.action = NOTHING;
*/
/*
 undo.head = NULL;
 undo.tail = NULL;
*/

 memcpy(&tmp1, &undo, sizeof(undo_t));
 memcpy(&undo, &redo, sizeof(undo_t));
 memcpy(&redo, &tmp1, sizeof(undo_t));
 redraw_screen();
 return 0;
}


int
fill_undo(int action, line_t *start_copy, line_t *end_copy, line_t *insert_pos,
					line_t *delete_begin, line_t *delete_end)
{
 	
 free_lines(undo.head, undo.tail);
 undo.head = NULL;
 undo.tail = NULL;
 
 undo.action = action;
 copy_lines(start_copy, end_copy, &undo.head, &undo.tail);
 
 undo.insert = insert_pos;
 undo.delete_begin = delete_begin;
 undo.delete_end = delete_end;
 undo.c_line = cur_line_number();
// undo.c_line = 1;
 undo.offset = offs;
 undo.x_curs = file->x_curs;
// undo.fcl = fcl;
 
 return 0;
}