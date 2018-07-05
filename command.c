/**** command.c ****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "med.h"
#include "command.h"
#include "keys.h"
#include "undo.h"

#define ON 1
#define OFF 0

Cmd *head;

extern buffer_t* last_file;
extern int create_f;
extern int options;
extern int _tabs;
extern char msg[MAX_LINE];
extern buffer_t* file;
extern Cmd* cmd_head;
extern Cmd_str* cmd_str_head;
extern int cmd_mask;


char** split(char* str);

/****** function declarations *****/
Cmd* gethead();
int set_tab(int argn, char** argv);
int cmd_open(int argn, char** argv);

//typedef int (*func_t)();

int 
make_commands() {

 newcom_str("tab", &set_tab);
 newcom_str("open o", &cmd_open);
 newcom_str("rename ren", &cmd_rename);
 newcom_str("jump j", &set_jump);
 newcom_str("ind indent", &set_autoindent);

/* start edit*/

 newcom(K_TEST, 0, OFF, NULL);
 newcom(K_CMD, 0, OFF, invoke_command);
 newcom(K_F_BUF_CLOSE, 0, OFF, close_file);
 newcom(K_F_BUF_REOPEN, 0, OFF, reopen_file);
 newcom(K_F_BUF_PREV, NUM_ROW_FLAG, OFF, go_mark_prev);
 newcom(K_F_BUF_PREV, NUM_ROW_FLAG, ON, file_buffer_prev);
 newcom(K_F_BUF_NEXT, NUM_ROW_FLAG, OFF, go_mark_next);
 newcom(K_F_BUF_NEXT, NUM_ROW_FLAG, ON,  file_buffer_next);
 newcom(K_F_BUF_GO_1, NUM_ROW_FLAG, OFF,  go_mark_1);
 newcom(K_F_BUF_GO_2, NUM_ROW_FLAG, OFF,  go_mark_2);
 newcom(K_F_BUF_GO_3, NUM_ROW_FLAG, OFF,  go_mark_3);
 newcom(K_F_BUF_GO_4, NUM_ROW_FLAG, OFF,  go_mark_4);
 newcom(K_F_BUF_GO_5, NUM_ROW_FLAG, OFF,  go_mark_5);
 newcom(K_F_BUF_GO_6, NUM_ROW_FLAG, OFF,  go_mark_6);
 newcom(K_F_BUF_GO_7, NUM_ROW_FLAG, OFF,  go_mark_7);
 newcom(K_F_BUF_GO_8, NUM_ROW_FLAG, OFF,  go_mark_8);
 newcom(K_F_BUF_GO_9, NUM_ROW_FLAG, OFF,  go_mark_9);
 newcom(K_F_BUF_GO_0, NUM_ROW_FLAG, OFF,  go_mark_last);
 newcom(K_F_BUF_GO_1, NUM_ROW_FLAG, ON,  file_buffer_go_1);
 newcom(K_F_BUF_GO_2, NUM_ROW_FLAG, ON,  file_buffer_go_2);
 newcom(K_F_BUF_GO_3, NUM_ROW_FLAG, ON,  file_buffer_go_3);
 newcom(K_F_BUF_GO_4, NUM_ROW_FLAG, ON,  file_buffer_go_4);
 newcom(K_F_BUF_GO_5, NUM_ROW_FLAG, ON,  file_buffer_go_5);
 newcom(K_F_BUF_GO_6, NUM_ROW_FLAG, ON,  file_buffer_go_6);
 newcom(K_F_BUF_GO_7, NUM_ROW_FLAG, ON,  file_buffer_go_7);
 newcom(K_F_BUF_GO_8, NUM_ROW_FLAG, ON,  file_buffer_go_8);
 newcom(K_F_BUF_GO_9, NUM_ROW_FLAG, ON,  file_buffer_go_9);
 newcom(K_F_BUF_GO_0, NUM_ROW_FLAG, ON,  file_buffer_go_last);
 newcom(K_CHNG_COL, 0, OFF, cmd_change_color);
 newcom(K_REDRAW, 0, OFF, redraw_screen);
 newcom(K_KDEAD, 0, OFF, cmd_dead_key_pressed);
 newcom(K_KMOD, 0, OFF, cmd_switch_alt_mode);
 newcom(K_SW_ROW, 0, OFF, cmd_switch_num_row_mode);
 newcom(K_SW_SRCH, 0, OFF, cmd_switch_search_mode);
 newcom(K_INDENT, 0, OFF, cmd_set_autoindent);
 newcom(K_CUR_UP, 0, OFF, cur_up);
 newcom(K_CUR_DOWN, 0, OFF, cur_down);
 newcom(K_M_UP, 0, OFF, move_up_one);
 newcom(K_M_DOWN, 0, OFF, move_down_one);
 newcom(K_M_UP_SCR, 0, OFF, move_up_half_screen);
 newcom(K_M_UP_START, 0, OFF, move_first);
 newcom(K_M_DOWN_SCR, 0, OFF, move_down_half_screen);
 newcom(K_M_DOWN_END, 0, OFF, move_last);
 newcom(K_M_LEFT, 0, OFF, move_backward_one);
 newcom(K_M_LEFT1, 0, OFF, move_backward_hs);
 newcom(K_M_LEFT2, 0, OFF, move_first_col);
 newcom(K_M_RIGHT, 0, OFF, move_forward_one);
 newcom(K_M_RIGHT1, 0, OFF, move_forward_hs);
 newcom(K_M_RIGHT2, 0, OFF, move_last_col);
 newcom(K_GOWORD, ALT_FLAG, OFF,  move_bow);
 newcom(K_GOWORD, ALT_FLAG, ON,  move_eow);
 newcom(K_GOWORD1, ALT_FLAG, ON,  move_bow);
 newcom(K_GOWORD1, ALT_FLAG, OFF,  move_eow);
 newcom(K_GOWORDBACK, 0, OFF, move_word_back);
 newcom(K_DELLINE, 0, OFF, cmd_del_line);
 newcom(K_DEL_BLOCK, 0, OFF, cmd_del_block);
 newcom(K_MKLINE, ALT_FLAG, ON, cmd_make_line_before);
 newcom(K_MKLINE, ALT_FLAG, OFF, cmd_make_line_after);
 newcom(K_FIND1, SRCH_FLAG, OFF, cmd_simple_find);
 newcom(K_FIND1, SRCH_FLAG, ON, cmd_find_beginning);
 newcom(K_FIND2, 0, OFF, cmd_find_name);
 newcom(K_REFIND, ALT_FLAG, OFF,  refind);
 newcom(K_REFIND, ALT_FLAG, ON,  alt_refind);
 newcom(K_SH_BUFF, 0, ON,  show_buffers);
 newcom(K_UP_BLOCK, 0, ON,  move_up_block);
 newcom(K_DOWN_BLOCK, 0, ON,  move_down_block);

// PARTY
/*
 newcom( K_REFIND2, ALT_FLAG, OFF, 
  refind, "", NULL);
*/

 newcom(K_SRCH_RPL, ALT_FLAG, OFF, cmd_replace_marks);
 newcom(K_SRCH_RPL, ALT_FLAG, ON, cmd_replace_whole);
 newcom(K_SRCH_RPL2, ALT_FLAG, OFF, cmd_replace_marks2);
 newcom(K_SRCH_RPL2, ALT_FLAG, ON, cmd_replace_whole2);
 newcom(K_REPEAT_RPL, ALT_FLAG, OFF, cmd_repeat_repl_marks);
 newcom(K_REPEAT_RPL, ALT_FLAG, ON, cmd_repeat_repl_whole);
 newcom(K_REPEAT_RPL2, ALT_FLAG, OFF, cmd_repeat_repl_marks2);
 newcom(K_REPEAT_RPL2, ALT_FLAG, ON, cmd_repeat_repl_whole2);
 newcom(K_GLOBAL, 0, OFF, cmd_replace_global);
 newcom(K_GLOBAL2, 0, OFF, cmd_replace_global2);
 newcom(K_WRITE_ALL, 0, OFF, save_all);
 newcom(K_WRITE, 0, OFF,  save_file);
 newcom(K_QUIT, 0, OFF, cmd_quit); 
 newcom(K_DEL, 0, OFF, del_char);
 newcom(K_DEL_WORD, 0, OFF, cmd_del_word);
 newcom(K_DEL_ENDLINE, 0, OFF, cmd_del_endline);
 newcom(K_CHANGE_WORD, 0, OFF, cmd_change_word);
 newcom(K_CASE_WORD, 0, OFF, case_word);
 newcom(K_CASE_WORD_BACKW, 0, OFF, case_backward);
 newcom(K_CASE_CHAR, 0, OFF, case_char);
 newcom(K_INS_MOD, ALT_FLAG, OFF, insert_string);
 newcom(K_INS_MOD, ALT_FLAG, ON, insert_string_after);
 newcom(K_INS_MOD1, ALT_FLAG, OFF, cmd_edit_new_line_after);
 newcom(K_INS_MOD1, ALT_FLAG, ON, cmd_edit_new_line_before); /* bug!!! */
 newcom(K_INS_PREPEND, 0, OFF, cmd_edit_line_begin);
 newcom(K_INS_APPEND, 0, OFF, cmd_edit_line_end);
 newcom(K_INS_INDENT, 0, OFF, insert_indent);
 newcom(K_INS_SPACE, 0, OFF, insert_space);
 newcom(K_INS_CHAR, ALT_FLAG, OFF, insert_char);
 newcom(K_INS_CHAR, ALT_FLAG, ON, insert_char_next);
 newcom(K_CHANGE_CHAR, 0, OFF, change_char);
 newcom(K_SET_MARK, 0, OFF, cmd_set_mark);
 newcom(K_GO_MARK_NUMB, 0, OFF, cmd_go_mark_number);
 newcom(K_SET_COPY_START, 0, OFF, set_mark1);
 newcom(K_COPY, 0, OFF, cmd_yank);
 newcom(K_PASTE, ALT_FLAG, OFF, cmd_paste);
 newcom(K_PASTE, ALT_FLAG, ON, cmd_paste_before);
 newcom(K_CONCAT, 0, OFF, concat_lines);
 newcom(K_GO_LINE, 0, OFF, cmd_goto_line);
 newcom(K_UNDO, 0, OFF, do_undo);


//end edit


 return 0;
}

int
getargn(char** argv) {
 int argn;
 
 if( !argv ) return -1;

 for( argn = 0; *(argv+argn); argn++ )
		;	
 return argn;
}

int
command(char* str) {
 Cmd_str *cmd;
 int argn;
 char** argv;
 int i;
 
 
 argv = split(str);
 argn = getargn(argv);
 
 if (!(*argv)) return 1;
 
	for ( cmd = cmd_str_head; cmd != NULL; cmd = cmd->next)
		for(i = 0; *(cmd->pstr+i) != NULL; i++) 
			if( strcmp( *argv, *(cmd->pstr+i) ) == 0 && *argv) {
				cmd->func(argn, argv);
				return 0;
			}

 return 0;
}

func_t 
command_char(char ch) {
 int i = 0;
 int ret = 1;
 Cmd *cmd;
 Cmd *head = gethead();
 int (*pfunc)() = NULL;

 for ( cmd = head; cmd != NULL; cmd = cmd->next)
		if ( cmd->ch == ch)
		if ( ((((cmd_mask & cmd->flag) == cmd->flag) && (cmd->status == ON)) 
			|| (((cmd_mask & cmd->flag) == 0) && (cmd->status == OFF))) )
			pfunc = cmd->noargsfunc;

 if (file->cur_line == NULL)
 if (pfunc == del_char || pfunc == case_word
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
	|| pfunc == move_first
	|| pfunc == move_last
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
	|| pfunc == cmd_replace_marks2
	|| pfunc == cmd_replace_whole2
	|| pfunc == cmd_repeat_repl_marks
	|| pfunc == cmd_repeat_repl_marks2
	|| pfunc == cmd_repeat_repl_whole
	|| pfunc == cmd_repeat_repl_whole2
	|| pfunc == go_mark_next
	|| pfunc == go_mark_prev
	|| pfunc == go_mark_last
	|| pfunc == cmd_yank
	|| pfunc == cmd_del_line
/*
	|| pfunc == 
	|| pfunc == 
	|| pfunc == 
	|| pfunc == 
	|| pfunc == 
	|| pfunc == 
*/
	)
 {
	message("Buffer is empty!");
	return;
 }

 if ( pfunc == insert_indent || pfunc == cmd_replace_marks
	|| pfunc == cmd_replace_marks2 || pfunc == cmd_repeat_repl_marks
	|| pfunc == cmd_repeat_repl_marks2 || pfunc == cmd_yank
	|| pfunc == cmd_del_block || pfunc == cmd_change_color )
	i = auto_set_mark();
 if (i) return;

/*  undo */
 if (pfunc == insert_char || pfunc == insert_char_next || 
	pfunc == change_char || pfunc == insert_space)
	fill_undo(REPLACE, file->cur_line, file->cur_line, file->cur_line->backw,
					file->cur_line->backw, file->cur_line->forw/*, file->cur_line->backw*/);

 if (pfunc == insert_string || pfunc == insert_string_after)
	fill_undo(REPLACE, file->cur_line, file->cur_line, file->cur_line->backw,
						file->cur_line->backw, file->cur_line->forw);

 if (file->cur_line)
 if (pfunc == cmd_edit_new_line_after)
	fill_undo(DELETE, NULL, NULL, NULL, file->cur_line, file->cur_line->forw);

 if (file->cur_line)
 if (pfunc == cmd_edit_new_line_before) // bug!!!!!!
	fill_undo(DELETE, NULL, NULL, NULL, file->cur_line->backw, file->cur_line);

/*
 if (pfunc == cmd_del_line)
	fill_undo(INSERT, file->copy_start_pos, file->copy_end_pos,
					file->copy_start_pos->backw, NULL, NULL);  
*/
 if (pfunc)
 pfunc();
 return pfunc;
}


Cmd*
gethead() {
 return cmd_head;
 return 0;
}

int
newcom(char ch, int mask, int status, int (*noargsfunc)() /* , 
		char* str, int (*func)(int argn, char** argv) */ )
{
 Cmd *cmd;
 int i;
 int n = 0;

 if (cmd_head == NULL) {
 	cmd_head = malloc(sizeof(Cmd));
	cmd_head->ch = ch;

	cmd_head->flag = mask;
	cmd_head->status = status;

//	cmd_head->pstr = split(str); 
 	cmd_head->next = NULL;
// 	cmd_head->func = func;
 	cmd_head->noargsfunc = noargsfunc;
	return 0;
 }

 for ( cmd = cmd_head; cmd->next != NULL; cmd = cmd->next);

 cmd->next = malloc(sizeof(Cmd));
 cmd = cmd->next;
// cmd->pch = pch;
	cmd->ch = ch;

	cmd->flag = mask;
	cmd->status = status;

// cmd->pstr = split(str);
 cmd->next = NULL;
// cmd->func = func;
 cmd->noargsfunc = noargsfunc;

 return 0;
}


int
newcom_str(char* str, int (*func)(int argn, char** argv))
{
 Cmd_str *cmd;
 int i;
 int n = 0;

 if (cmd_str_head == NULL) {
 	cmd_str_head = malloc(sizeof(Cmd_str));

	cmd_str_head->pstr = split(str); 
 	cmd_str_head->next = NULL;
 	cmd_str_head->func = func;
	return 0;
 }

 for ( cmd = cmd_str_head; cmd->next != NULL; cmd = cmd->next);

 cmd->next = malloc(sizeof(Cmd_str));
 cmd = cmd->next;

 cmd->pstr = split(str);
 cmd->next = NULL;
 cmd->func = func;

 return 0;
}

int 
freecommands()
{
 Cmd *cmd, *tmp;
 int i, j;
 if(!head) return 0;
 for(cmd = head, j = 0, tmp = head; tmp != NULL; cmd = tmp, j++) {
 	tmp = cmd->next;
	free(cmd);
 }
 return 0;
}

int 
freecommands_str()
{
 Cmd_str *cmd, *tmp;
 int i, j;
 if(!head) return 0;
 for(cmd = tmp = cmd_str_head, j = 0; tmp != NULL; cmd = tmp, j++) {
 	tmp = cmd->next;
	for(i = 0; (cmd->pstr[i]); i++) {
		free( (cmd->pstr[i]) );
	}
	free(cmd);
 }
 return 0;
}


int
set_tab(int argn , char** argv)
{
 int i = 0;
 if(argn == 1) {
 	sprintf(msg, "_tabs: %d", _tabs);
 	message(msg);
 }
 if(argn != 2) return 0;

 i = atoi(argv[1]); 
 if (i > 80) i = 80;
 if (i)
 _tabs = i;
 redraw_screen();
 sprintf(msg, "_tabs: %d",  _tabs);
 message(msg);
 return 0;
}