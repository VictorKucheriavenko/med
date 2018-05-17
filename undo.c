#include "undo.h"
#include "med.h"

extern char msg[MAX_LINE];

extern buffer_t* file;

undo_t undo;

int
do_undo()
{
 line_t *tmp;
 switch (undo.action)
 {
 case DELETE:
	/*fill_undo(...);*/
	delete(undo.delete_begin, undo.delete_end);
	break;
 case INSERT:
	/*fill_undo(...);*/
	insert_lines(undo.insert, undo.head, undo.tail);
	break;
 case REPLACE:
	/*fill_undo(...);*/
	delete(undo.delete_begin, undo.delete_end);
	insert_lines(undo.insert, undo.head, undo.tail);
	break;
 default:
	return 0;
	break;
 }
sprintf(msg, "%s", get_head()->str);
//gmessage(msg);
sprintf(msg, "%s", get_tail()->str);
//gmessage(msg);
 set_cur_line(undo.c_line);
 file->offs = undo.offs;
 file->x_curs = undo.x_curs;
 redraw_screen();
 return 0;
}


int
fill_undo(int action, line_t *start_copy, line_t *end_copy, line_t *insert_pos,
					line_t *delete_begin, line_t *delete_end)
{
// free_lines(undo.head, undo.tail);//seg fault
 undo.action = action;
 copy_lines(start_copy, end_copy, &undo.head, &undo.tail);
 undo.insert = insert_pos;
 undo.delete_begin = delete_begin;
 undo.delete_end = delete_end;
 undo.c_line = cur_line_number();
 undo.offs = file->offs;
 undo.x_curs = file->x_curs;
 
 return 0;
}