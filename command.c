/**** command.c ****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
int help_comm(int argn, char** argv);

//typedef int (*func_t)();

int 
make_commands() {

 newcom_str("tab", &set_tab);
 newcom_str("open o", &cmd_open);
 newcom_str("rename ren", &cmd_rename);
 newcom_str("jump j", &set_jump);
 newcom_str("ind indent", &set_autoindent);
 newcom_str("help_comm h", &help_comm);


 newcom(K_TEST, 0, OFF, test, "test");
 newcom(K_QUIT, 0, OFF, cmd_quit, "quit"); 
 newcom(K_WRITE_BUF, 0, OFF,  save_file, "save_file");
 newcom(K_WRITE_ALL, 0, OFF, save_all, "save_all");
 newcom(K_CMD, 0, OFF, invoke_command, "inv_comm");
 newcom(K_F_BUF_CLOSE, 0, OFF, close_file, "close_file");
 newcom(K_F_BUF_REOPEN, 0, OFF, reopen_file, "reopen");
 newcom(K_F_BUF_PREV, NUM_ROW_FLAG, OFF, go_mark_prev, "prev_mark");
 newcom(K_F_BUF_PREV, NUM_ROW_FLAG, ON, file_buffer_prev, "prev_file");
 newcom(K_F_BUF_NEXT, NUM_ROW_FLAG, OFF, go_mark_next, "next_mark");
 newcom(K_F_BUF_NEXT, NUM_ROW_FLAG, ON,  file_buffer_next, "next_file");
 newcom(K_F_BUF_GO_1, NUM_ROW_FLAG, OFF,  go_mark_1, "mark_1");
 newcom(K_F_BUF_GO_2, NUM_ROW_FLAG, OFF,  go_mark_2, "mark_2");
 newcom(K_F_BUF_GO_3, NUM_ROW_FLAG, OFF,  go_mark_3, "mark_3");
 newcom(K_F_BUF_GO_4, NUM_ROW_FLAG, OFF,  go_mark_4, "mark_4");
 newcom(K_F_BUF_GO_5, NUM_ROW_FLAG, OFF,  go_mark_5, "mark_5");
 newcom(K_F_BUF_GO_6, NUM_ROW_FLAG, OFF,  go_mark_6, "mark_6");
 newcom(K_F_BUF_GO_7, NUM_ROW_FLAG, OFF,  go_mark_7, "mark_7");
 newcom(K_F_BUF_GO_8, NUM_ROW_FLAG, OFF,  go_mark_8, "mark_8");
 newcom(K_F_BUF_GO_9, NUM_ROW_FLAG, OFF,  go_mark_9, "mark_9");
 newcom(K_F_BUF_GO_0, NUM_ROW_FLAG, OFF,  go_mark_last, "last_mark");
 newcom(K_F_BUF_GO_1, NUM_ROW_FLAG, ON,  file_buffer_go_1, "file_1");
 newcom(K_F_BUF_GO_2, NUM_ROW_FLAG, ON,  file_buffer_go_2, "file_2");
 newcom(K_F_BUF_GO_3, NUM_ROW_FLAG, ON,  file_buffer_go_3, "file_3");
 newcom(K_F_BUF_GO_4, NUM_ROW_FLAG, ON,  file_buffer_go_4, "file_4");
 newcom(K_F_BUF_GO_5, NUM_ROW_FLAG, ON,  file_buffer_go_5, "file_5");
 newcom(K_F_BUF_GO_6, NUM_ROW_FLAG, ON,  file_buffer_go_6, "file_6");
 newcom(K_F_BUF_GO_7, NUM_ROW_FLAG, ON,  file_buffer_go_7, "file_7");
 newcom(K_F_BUF_GO_8, NUM_ROW_FLAG, ON,  file_buffer_go_8, "file_8");
 newcom(K_F_BUF_GO_9, NUM_ROW_FLAG, ON,  file_buffer_go_9, "file_9");
 newcom(K_F_BUF_GO_0, NUM_ROW_FLAG, ON,  file_buffer_go_last, "last_file");
 newcom(K_SW_INDENT, 0, ON, cmd_change_color, "chng_col");
 newcom(K_REDRAW, 0, OFF, redraw_screen, "redraw_scr");
 newcom(K_DEAD_TAB, 0, OFF, cmd_dead_key_pressed, "short_sw_flags");
 newcom(K_SW_ALT, 0, OFF, cmd_switch_alt_mode, "switch_alt");
 newcom(K_SW_ROW, 0, OFF, cmd_switch_num_row_mode, "switch_num");
 newcom(K_SW_SRCH, 0, OFF, cmd_switch_search_mode, "switch_srch");
 newcom(K_SW_INDENT, 0, OFF, cmd_set_autoindent, "set_autoind");
 newcom(K_CUR_UP, 0, OFF, cur_up, "cur_up");
 newcom(K_CUR_DN, 0, OFF, cur_down, "cur_down");
 newcom(K_MV_UP, 0, OFF, move_up_one, "mv_up");
 newcom(K_MV_DN, 0, OFF, move_down_one, "mv_dn");
 newcom(K_MV_UP_SCR, 0, OFF, move_up_half_screen, "mv_up_scr");
 newcom(K_MV_DN_SCR, 0, OFF, move_down_half_screen, "mv_dn_scr");
 newcom(K_MV_UP_START, 0, OFF, move_first_line, "move_first_line");
 newcom(K_MV_DN_END, 0, OFF, move_last_line, "mv_last_line");
 newcom(K_MV_L, 0, OFF, move_backward_one, "mv_l");
 newcom(K_MV_L_N, 0, OFF, move_backward_hs, "mv_l_n");
 newcom(K_MV_L_BEG, 0, OFF, move_first_col, "mv_l_beg");
 newcom(K_MV_R, 0, OFF, move_forward_one, "mv_r");
 newcom(K_MV_R_N, 0, OFF, move_forward_hs, "mv_r_n");
 newcom(K_MV_R_END, 0, OFF, move_last_col, "mv_r_end");
 /*
 // not needed
 newcom(K_MV_EOW, ALT_FLAG, OFF,  move_bow, "mv_bow");
 newcom(K_MV_EOW, ALT_FLAG, ON,  move_eow, "mv_eow");
 */
 newcom(K_MV_BOW, ALT_FLAG, OFF,  move_bow, "mv_bow");
 newcom(K_MV_BOW, ALT_FLAG, ON,  move_eow, "mv_eow");
 newcom(K_MV_WORDBACK, 0, OFF, move_word_back, "mv_word_back");
 newcom(K_DELLINE, 0, OFF, cmd_del_line, "del_line");
 newcom(K_DEL_BLOCK, 0, OFF, cmd_del_block, "del_block");
 newcom(K_MKLINE, ALT_FLAG, ON, cmd_make_line_before, "add_empty_prev");
 newcom(K_MKLINE, ALT_FLAG, OFF, cmd_make_line_after, "add_empty_next");
 newcom(K_FIND, SRCH_FLAG, OFF, cmd_simple_find, "find");
 newcom(K_FIND, SRCH_FLAG, ON, cmd_find_beginning, "find_begin");
 newcom(K_FIND_NAME, 0, OFF, cmd_find_name, "find_name");
 newcom(K_REFIND, ALT_FLAG, OFF,  refind, "rep_find");
 newcom(K_REFIND, ALT_FLAG, ON,  alt_refind, "alt_rep_find");
 newcom(K_SHOW_BUFF, 0, ON,  show_buffers, "show_files");
 newcom(K_MV_UP_BLOCK, 0, ON,  move_up_block, "mv_up_block");
 newcom(K_MV_DN_BLOCK, 0, ON,  move_down_block, "mv_dn_block");

// PARTY
/*
 newcom( K_REFIND_NAME, ALT_FLAG, OFF, 
  refind, "", NULL, "test");
*/

 newcom(K_REPLACE, ALT_FLAG, OFF, cmd_replace_marks, "replace_selected");
 newcom(K_REPLACE, ALT_FLAG, ON, cmd_replace_whole, "replace_whole");
 newcom(K_RENAME, ALT_FLAG, OFF, cmd_rename_marks, "rename_selected");
 newcom(K_RENAME, ALT_FLAG, ON, cmd_rename_whole, "rename_whole");
 newcom(K_REPEAT_RPL, ALT_FLAG, OFF, cmd_repeat_repl_marks, "rep_repl_sel");
 newcom(K_REPEAT_RPL, ALT_FLAG, ON, cmd_repeat_repl_whole, "rep_repl_whole");
 newcom(K_REPEAT_REN, ALT_FLAG, OFF, cmd_repeat_rename_marks, "rep_ren_sel");
 newcom(K_REPEAT_REN, ALT_FLAG, ON, cmd_repeat_rename_whole, "rep_ren_whole");
 newcom(K_GL_REPL, 0, OFF, cmd_replace_global, "global_replace");
 newcom(K_GL_REN, 0, OFF, cmd_rename_global, "global_rename");
 newcom(K_DEL, 0, OFF, del_char, "del_char");
 newcom(K_DEL_WORD, 0, OFF, cmd_del_word, "del_word");
 newcom(K_DEL_ENDLINE, 0, OFF, cmd_del_endline, "del_eol");
 newcom(K_CHANGE_WORD, 0, OFF, cmd_change_word, "change_word");
 newcom(K_CASE_WORD, 0, OFF, case_word, "case_word_forw");
 newcom(K_CASE_WORD_BACKW, 0, OFF, case_backward, "up_case_backw");
 newcom(K_CASE_CHAR, 0, OFF, case_char, "case_char");
 newcom(K_EDIT, ALT_FLAG, OFF, insert_string, "ins_edit");
 newcom(K_EDIT, ALT_FLAG, ON, insert_string_after, "ins_edit_after");
 newcom(K_EDIT_NL, ALT_FLAG, OFF, cmd_edit_new_line_after, "new_line_after");
 newcom(K_EDIT_NL, ALT_FLAG, ON, cmd_edit_new_line_before, "new_line_before"); /* bug!!! */
 newcom(K_EDIT_BEG, 0, OFF, cmd_edit_line_begin, "edit_beg");
 newcom(K_EDIT_END, 0, OFF, cmd_edit_line_end, "edit_end");
 newcom(K_INDENT, 0, OFF, insert_indent, "indent");
 newcom(K_SPACE, 0, OFF, insert_space, "space");
 newcom(K_INS_CHAR, ALT_FLAG, OFF, insert_char, "ins_char");
 newcom(K_INS_CHAR, ALT_FLAG, ON, insert_char_next, "ins_char_next");
 newcom(K_CHANGE_CHAR, 0, OFF, change_char, "change_char");
 newcom(K_SET_MARK, 0, OFF, cmd_set_mark, "set_mark");
 newcom(K_GO_MARK_NUMB, 0, OFF, cmd_go_mark_number, "go_mark_no");
 newcom(K_SELECT, 0, OFF, set_mark1, "select");
 newcom(K_COPY, 0, OFF, cmd_yank, "yank");
 newcom(K_PASTE, ALT_FLAG, OFF, cmd_paste, "paste");
 newcom(K_PASTE, ALT_FLAG, ON, cmd_paste_before, "paste_before");
 newcom(K_CONCAT, 0, OFF, concat_lines, "concat_lines");
 newcom(K_GOTO_LINE, 0, OFF, cmd_goto_line, "goto_line");
 newcom(K_UNDO, 0, OFF, do_undo, "undo");
 return 0;
}

int
make_help()
{
 assign_help("test", "test");
 assign_help("quit", "quit program"); 
 assign_help("save_file", "save changes in current file");
 assign_help("save_all", "save changes in all opened files");
 assign_help("inv_comm", "invoke command via command line");
 assign_help("close_file", "close current buffer");
 assign_help("reopen", "try to reopen same file");
 assign_help("prev_mark", "previous mark");
 assign_help("prev_file", "previous file buffer");
 assign_help("next_mark", "next mark");
 assign_help("next_file", "next file");
 assign_help("mark_1", "go to mark 1");
 assign_help("mark_2", "go to mark 2");
 assign_help("mark_3", "go to mark 3");
 assign_help("mark_4", "go to mark 4");
 assign_help("mark_5", "go to mark 5");
 assign_help("mark_6", "go to mark 6");
 assign_help("mark_7", "go to mark 7");
 assign_help("mark_8", "go to mark 8");
 assign_help("mark_9", "go to mark 9");
 assign_help("last_mark", "go to last mark");
 assign_help("file_1", "switch to buffer 1");
 assign_help("file_2", "switch to buffer 2");
 assign_help("file_3", "switch to buffer 3");
 assign_help("file_4", "switch to buffer 4");
 assign_help("file_5", "switch to buffer 5");
 assign_help("file_6", "switch to buffer 6");
 assign_help("file_7", "switch to buffer 7");
 assign_help("file_8", "switch to buffer 8");
 assign_help("file_9", "switch to buffer 9");
 assign_help("last_file", "switch between current and previous buffer");
 assign_help("chng_col", "change color");
 assign_help("redraw_scr", "redraw screen");
 assign_help("short_sw_flags", "switch all flags for one action");
 assign_help("switch_alt", "switch alt flag");
 assign_help("switch_num", "switch num flag");
 assign_help("switch_srch", "switch srch flag");
 assign_help("set_autoind", "switch autoindent");
 assign_help("cur_up", "move current cursor position up");
 assign_help("cur_down", "move current cursor position down");
 assign_help("mv_up", "scroll screen down one line");
 assign_help("mv_dn", "scroll screen up one line");
 assign_help("mv_up_scr", "move down defined number of lines");
 assign_help("mv_dn_scr", "move up defined number of lines");
 assign_help("move_first_line", "position cursor to the first line of text");
 assign_help("mv_last_line", "position cursor to the last line of text");
 assign_help("mv_l", "move cursor left");
 assign_help("mv_l_n", "move cursor left defined number of times");
 assign_help("mv_l_beg", "move cursor to the beginnig of line");
 assign_help("mv_r", "move cursor right");
 assign_help("mv_r_n", "move cursor right defined number of times");
 assign_help("mv_r_end", "move cursor to the end of line");
 assign_help("mv_bow", "move one word(position at beginning)");
 assign_help("mv_eow", "move end of word");
 assign_help("mv_word_back", "move one word backward");
 assign_help("del_line", "delete line");
 assign_help("del_block", "delete selected block");
 assign_help("add_empty_prev", "add empty line before current");
 assign_help("add_empty_next", "add empty line after current");
 assign_help("find", "find string");
 assign_help("find_begin", "find string (only beginning of the line matches)");
 assign_help("find_name", "find string surrounded by spaces or punctuation");
 assign_help("rep_find", "repeat last find");
 assign_help("alt_rep_find", "repeat last find (alternative)");
 assign_help("show_files", "list all edited buffers");
 assign_help("mv_up_block", "move up to the first line which begins not with spaces");
 assign_help("mv_dn_block", "move down to the first line which begins not with spaces");
 assign_help("replace_selected", "replace withing selected area");
 assign_help("replace_whole", "replace withing whole file");
 assign_help("rename_selected", "rename withing selected area");
 assign_help("rename_whole", "rename withing whole file");
 assign_help("rep_repl_sel", "repeat last replace withing selected area");
 assign_help("rep_repl_whole", "repeat last replace withing whole file");
 assign_help("rep_ren_sel", "repeat last rename withing selected area");
 assign_help("rep_ren_whole", "repeat last rename withing whole file");
 assign_help("global_replace", "replace in all files");
 assign_help("global_rename", "rename in all files");
 assign_help("del_char", "delete one character");
 assign_help("del_word", "delete one word");
 assign_help("del_eol", "delete until end of line");
 assign_help("change_word", "delete one word and start edit");
 assign_help("case_word_forw", "change upper/lower case forward one word from current cursor position");
 assign_help("up_case_backw", "capitalize word backwart from current cursor position");
 assign_help("case_char", "change upper/lower case of the current character");
 assign_help("ins_edit", "start edit at the current cursor position");
 assign_help("ins_edit_after", "edit after current character");
 assign_help("new_line_after", "start edit after current line");
 assign_help("new_line_before", "start edit before current line");
 assign_help("edit_beg", "start prependig at beginning of line");
 assign_help("edit_end", "start appending to end of line");
 assign_help("indent", "insert character before each line of the selected text");
 assign_help("space", "insert space");
 assign_help("ins_char", "insert character");
 assign_help("ins_char_next", "insert character after current");
 assign_help("change_char", "change character");
 assign_help("set_mark", "set mark at current location");
 assign_help("go_mark_no", "go to specific mark");
 assign_help("select", "set selection mark");
 assign_help("yank", "yank lines");
 assign_help("paste", "paste yanked lines");
 assign_help("paste_before", "paste before current line");
 assign_help("concat_lines", "concatenate lines");
 assign_help("goto_line", "change current lines by number");
 assign_help("undo", "undo last action");
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
	|| pfunc == cmd_rename_marks || pfunc == cmd_repeat_repl_marks
	|| pfunc == cmd_repeat_rename_marks || pfunc == cmd_yank
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
newcom(char ch, int mask, int status, int (*noargsfunc)(), char* name /* , 
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
 	
	cmd_head->name = name;
	cmd_head->help_str = NULL;
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

 cmd->name = name;
 cmd->help_str = NULL;
 return 0;
}

int
assign_help(char* name, char* help_str)
{
 Cmd* cmd;
 for (cmd = cmd_head; cmd != NULL; cmd = cmd->next)
 {
	if (cmd->name)
	if (strcmp(cmd->name, name) == 0)
	{
		cmd->help_str = strdup(help_str);
		return 0;
	}
 }
 return 1;
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
 	free(cmd->name);
 	free(cmd->help_str);
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

int
put_separator()
{
 putchar(':');
 return 0;
}


int
help_comm(int argn, char** argv)
{
 FILE* fp = 
//	fopen("help_comm.out", "w");
	stdout;

 Cmd* cmd;

 int i = 0;
 
//	fprintf(fp, "i");
	fprintf(fp, "ch| flag| sts|            name|descr\n");// header
 
 for (cmd = cmd_head; cmd != NULL; cmd = cmd->next, i++)
 {
//	fprintf(fp, "%03d", i);
	if (iscntrl(cmd->ch))
		fprintf(fp, "^%c", cmd->ch + 64);
	else
		fprintf(fp, "%2c", cmd->ch);
		
	put_separator();
	
	/* flag */
	switch (cmd->flag) {
	case 0:
		fprintf(fp, "%5s", " - ");
		break;
	case ALT_FLAG:
		fprintf(fp, "%5s", "alt");
		break;
	case NUM_ROW_FLAG:
		fprintf(fp, "%5s", "num");
		break;
	case SRCH_FLAG:
		fprintf(fp, "%5s", "srch");
		break;
/*
	case SRCH_FLAG2:
		fprintf(fp, "%5s", "srch2");
		break;
	case SRCH_FLAG3:
		fprintf(fp, "%5s", "srch3");
		break;
*/
	default:
		fprintf(fp, "%5s", "unkn");
		break;
	}
	
	put_separator();
	
// status
	fprintf(fp, "%4s", (cmd->status == ON) ? "ON" : "OFF");
	
	put_separator();
	
	fprintf(fp, "%16s", cmd->name);
	
	put_separator();
	
	fprintf(fp, "%s\n", cmd->help_str);
 }
 return 0;
}