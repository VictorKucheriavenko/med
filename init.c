#include <string.h>
#include "med.h"
#include "move_x.h"
#include "command.h"
#include "undo.h"

#define ctrl(c) ((c) - 96)
#define C_a '\001'
#define C_b '\002'
#define C_c '\003'
#define C_d '\004'
#define C_e '\005'
#define C_f '\006'
#define C_g '\007'
#define C_h '\010'
#define C_i '\011'
#define C_j '\012'
#define C_k '\013'
#define C_l '\014'
#define C_m '\015'
#define C_n '\016'
#define C_o '\017'
#define C_p '\020'
#define C_q '\021'
#define C_r '\022'
#define C_s '\023'
#define C_t '\024'
#define C_u '\025'
#define C_v '\026'
#define C_w '\027'
#define C_x '\030'
#define C_y '\031'
#define C_z '\032'
#define Grave '\140'


#define K_CMD 'q'
//#define K_TEST '\026'
#define K_TEST 'T'

#define K_SCRIPT C_k
#define K_MAKE_RUN 'A'
#define K_FUNC_TEMPL 'B'
#define K_F_BUF_CLOSE C_q
#define K_F_BUF_REOPEN C_a 
#define K_F_BUF_NEXT '='
#define K_F_BUF_PREV '-'
#define K_F_BUF_GO_1 '1'
#define K_F_BUF_GO_2 '2'
#define K_F_BUF_GO_3 '3'
#define K_F_BUF_GO_4 '4'
#define K_F_BUF_GO_5 '5'
#define K_F_BUF_GO_6 '6'
#define K_F_BUF_GO_7 '7'
#define K_F_BUF_GO_8 '8'
#define K_F_BUF_GO_9 '9'
#define K_F_BUF_GO_0 '0'

#define K_CHNG_COL 'y'
#define K_CHNG_COL_MODE 'Y'
#define K_REDRAW C_r

#define K_SW_ALT '!' 
#define K_DEAD_TAB '\t'

#define K_SW_ROW Grave 
#define K_SW_SRCH '@'
#define K_SW_INDENT C_g

#define K_GL_REPL C_t
#define K_GL_REN C_y

#define K_INSP_KEY C_p
/*
#define K_CUR_DN 'V'
*/
#define K_MV_UP 'k'
#define K_MV_UP_SCR 'K'
#define K_MV_UP_START '>'
#define K_MV_DN 'j'
#define K_MV_DN_SCR 'J'
#define K_MV_DN_END '<'
#define K_MV_R 'f'
#define K_MV_R_END 'g'
#define K_MV_R_N 'I'
#define K_MV_BOW 'i'
#define K_MV_WORDBACK 'u'
//#define K_MV_BOW C_v
#define K_MV_L 'd'
#define K_MV_L_BEG 's'
#define K_MV_L_N 'U'
#define K_DELLINE 't'
#define K_MKLINE 'O'
#define K_DEL 'r'
#define K_DEL_WORD 'e'
#define K_DEL_ENDLINE C_e 
#define K_CHANGE_WORD 'o'
#define K_CASE_WORD 'X'// unused
#define K_CASE_WORD_BACKW C_u
#define K_CASE_CHAR C_x
#define K_EDIT 'l'
#define K_EDIT_NL ';'

#define K_EDIT_BEG 'D'
#define K_EDIT_END 'F'

#define K_INDENT C_o
#define K_SPACE ' '
#define K_INS_CHAR 'p'
#define K_CHANGE_CHAR 'w'
#define K_FIND '/'
#define K_FIND_NAME '?'
#define K_REPLACE 'S'
#define K_REPEAT_RPL C_s
#define K_RENAME 'W'
#define K_REPEAT_REN C_w
#define K_REFIND 'n'
#define K_REFIND_NAME 'N'
#define K_WRITE_BUF '\''
#define K_WRITE_ALL '\"'
#define K_QUIT 'Q' 
#define K_SELECT 'h'
#define K_SET_MARK 'm'
#define K_GO_MARK_NUMB 'c'// unused
#define K_COPY 'a'
#define K_PASTE 'z'
#define K_CONCAT 'C'
#define K_DEL_BLOCK '['
#define K_GOTO_LINE  'P'
#define K_SHOW_BUFF C_b
#define K_MV_UP_BLOCK C_d
#define K_MV_DN_BLOCK C_f
#define K_UNDO C_l
#define K_READ_SEQ C_v  
#define K_SEND_SEQ 'v' 
#define K_RE_COM 'H'

#define K_ENFRAME 'N' 
#define K_READ_ENFR_STR C_n


extern char compile_and_run_buf[MAX_LINE];
extern buffer_t *file;

int
init2()
{
/*
 strcpy(compile_and_run_buf, "gcc  -o tmp.out ");
 strcat(compile_and_run_buf, file->filename);
 strcat(compile_and_run_buf, " -lm");
*/
 strcpy(compile_and_run_buf, "make");
 return 0;
}

int 
make_commands() {

 newcom_str("tab", &set_tab);
 newcom_str("ds", &set_debug_screen);
 newcom_str("cs", &clear_debug_screen);
 newcom_str("open o", &cmd_open);
 newcom_str("create c", &cmd_create);
 newcom_str("rename ren", &cmd_rename);
 newcom_str("jump j", &set_jump);
 newcom_str("ind indent", &set_autoindent);
 newcom_str("help_comm h", &help_comm);
 newcom_str("cm", &command_mode);
 newcom_str("shell", &shell_command_mode);

 newcom_str("cur_up", cur_up);
 newcom_str("cur_down", cur_down);

 newcom(K_TEST, ALT_FLAG, OFF, test, "test");
 newcom(K_TEST, ALT_FLAG, ON, test1, "test1");
 newcom(K_INSP_KEY, 0, ON, inspect_key, "inspect_key");
 newcom(K_MAKE_RUN, ALT_FLAG, OFF, make_run, "make_run");
 newcom(K_MAKE_RUN, ALT_FLAG, ON, get_compile_and_run_string, "get_cnr_str");
 newcom(K_FUNC_TEMPL, 0, OFF, new_func_templ, "new_func_templ");
 newcom(K_SCRIPT, 0, OFF, script, "script");
 newcom(K_QUIT, 0, OFF, cmd_quit, "quit"); 
 newcom(K_WRITE_BUF, 0, OFF,  save_file, "save_file");
 newcom(K_WRITE_ALL, 0, OFF, save_all, "save_all");
 newcom(K_CMD, ALT_FLAG, OFF, invoke_command, "inv_comm");
 newcom(K_CMD, ALT_FLAG, ON, invoke_shell_command, "exe_shell");
 newcom(K_F_BUF_CLOSE, 0, OFF, close_file, "close_file");
 newcom(K_F_BUF_REOPEN, 0, OFF, reopen_file, "reopen");
 newcom(K_F_BUF_GO_1, NUM_ROW_FLAG, ON,  go_mark_1, "mark_1");
 newcom(K_F_BUF_GO_2, NUM_ROW_FLAG, ON,  go_mark_2, "mark_2");
 newcom(K_F_BUF_GO_3, NUM_ROW_FLAG, ON,  go_mark_3, "mark_3");
 newcom(K_F_BUF_GO_4, NUM_ROW_FLAG, ON,  go_mark_4, "mark_4");
 newcom(K_F_BUF_GO_5, NUM_ROW_FLAG, ON,  go_mark_5, "mark_5");
 newcom(K_F_BUF_GO_6, NUM_ROW_FLAG, ON,  go_mark_6, "mark_6");
 newcom(K_F_BUF_GO_7, NUM_ROW_FLAG, ON,  go_mark_7, "mark_7");
 newcom(K_F_BUF_GO_8, NUM_ROW_FLAG, ON,  go_mark_8, "mark_8");
 newcom(K_F_BUF_GO_9, NUM_ROW_FLAG, ON,  go_mark_9, "mark_9");
 newcom(K_F_BUF_GO_0, NUM_ROW_FLAG, ON,  go_mark_last, "last_mark");
 newcom(K_F_BUF_PREV, NUM_ROW_FLAG, ON, go_mark_prev, "prev_mark");
 newcom(K_F_BUF_NEXT, NUM_ROW_FLAG, ON, go_mark_next, "next_mark");
 newcom(K_F_BUF_GO_1, NUM_ROW_FLAG, OFF,  file_buffer_go_1, "file_1");
 newcom(K_F_BUF_GO_2, NUM_ROW_FLAG, OFF,  file_buffer_go_2, "file_2");
 newcom(K_F_BUF_GO_3, NUM_ROW_FLAG, OFF,  file_buffer_go_3, "file_3");
 newcom(K_F_BUF_GO_4, NUM_ROW_FLAG, OFF,  file_buffer_go_4, "file_4");
 newcom(K_F_BUF_GO_5, NUM_ROW_FLAG, OFF,  file_buffer_go_5, "file_5");
 newcom(K_F_BUF_GO_6, NUM_ROW_FLAG, OFF,  file_buffer_go_6, "file_6");
 newcom(K_F_BUF_GO_7, NUM_ROW_FLAG, OFF,  file_buffer_go_7, "file_7");
 newcom(K_F_BUF_GO_8, NUM_ROW_FLAG, OFF,  file_buffer_go_8, "file_8");
 newcom(K_F_BUF_GO_9, NUM_ROW_FLAG, OFF,  file_buffer_go_9, "file_9");
 newcom(K_F_BUF_GO_0, NUM_ROW_FLAG, OFF,  file_buffer_go_last, "last_file");
 newcom(K_F_BUF_PREV, NUM_ROW_FLAG, OFF, file_buffer_prev, "prev_file");
 newcom(K_F_BUF_NEXT, NUM_ROW_FLAG, OFF,  file_buffer_next, "next_file");
 newcom(K_CHNG_COL, 0, OFF, cmd_change_color, "chng_col");
 newcom(K_CHNG_COL_MODE, 0, OFF, cmd_change_color_mode, "chng_col_mode");
 newcom(K_REDRAW, 0, OFF, redraw_screen, "redraw_scr");
 newcom(K_DEAD_TAB, 0, OFF, cmd_dead_key_pressed, "short_sw_flags");
 newcom(K_SW_ALT, 0, OFF, cmd_switch_alt_mode, "switch_alt");
 newcom(K_SW_ROW, 0, OFF, cmd_switch_num_row_mode, "switch_num");
 newcom(K_SW_SRCH, 0, OFF, cmd_switch_search_mode, "switch_srch");
 newcom(K_SW_INDENT, 0, OFF, cmd_set_autoindent, "set_autoind");
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
 newcom(, ALT_FLAG, OFF,  move_bow, "mv_bow");
 newcom(KMV_EOW, ALT_FLAG, OFF,  move_bow, "mv_bow");
 newcom(K_MV_EOW, ALT_FLAG, ON,  move_eow, "mv_eow");
 */
 newcom(K_MV_BOW, ALT_FLAG, OFF,  move_bow, "mv_bow");
 newcom(K_MV_BOW, ALT_FLAG, ON,  move_eow, "mv_eow");
 newcom(K_MV_WORDBACK, ALT_FLAG, OFF, move_word_back, "mv_word_back");
 newcom(K_MV_WORDBACK, ALT_FLAG, ON, move_long_word_back, "mv_long_word_back");
 newcom(K_DELLINE, 0, OFF, cmd_del_line, "del_line");
 newcom(K_DEL_BLOCK, 0, OFF, cmd_del_block, "del_block");
 newcom(K_MKLINE, ALT_FLAG, ON, cmd_make_line_before, "add_empty_prev");
 newcom(K_MKLINE, ALT_FLAG, OFF, cmd_make_line_after, "add_empty_next");
 newcom(K_FIND, SRCH_FLAG, OFF, cmd_simple_find, "find");
 newcom(K_FIND, SRCH_FLAG, ON, cmd_find_beginning, "find_begin");
 newcom(K_FIND_NAME, 0, OFF, cmd_find_name, "find_name");
 newcom(K_REFIND, ALT_FLAG, OFF,  refind, "rep_find");
 newcom(K_REFIND, ALT_FLAG, ON,  alt_refind, "alt_rep_find");
 newcom(K_SHOW_BUFF, 0, ON,  show_hide_buffers, "show_files");
 newcom(K_MV_UP_BLOCK, 0, ON,  move_up_block, "mv_up_block");
 newcom(K_MV_DN_BLOCK, 0, ON,  move_down_block, "mv_dn_block");
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
 newcom(K_DEL_WORD, ALT_FLAG, ON, cmd_del_word, "del_word");
 newcom(K_DEL_WORD, ALT_FLAG, OFF, cmd_del_long_word, "del_long_word");
 newcom(K_DEL_ENDLINE, 0, OFF, cmd_del_endline, "del_eol");
 newcom(K_CHANGE_WORD, ALT_FLAG, ON, cmd_change_word, "change_word");
 newcom(K_CHANGE_WORD, ALT_FLAG, OFF, cmd_change_long_word, "change_long_word");
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
 newcom(K_COPY, 0, OFF, yank, "yank");
 newcom(K_PASTE, ALT_FLAG, OFF, cmd_paste, "paste");
 newcom(K_PASTE, ALT_FLAG, ON, cmd_paste_before, "paste_before");
 newcom(K_CONCAT, 0, OFF, concat_lines, "concat_lines");
 newcom(K_GOTO_LINE, 0, OFF, cmd_goto_line, "goto_line");
 newcom(K_UNDO, 0, OFF, do_undo, "undo");
 newcom(K_READ_SEQ, 0, OFF, read_sequence, "read_seqnce");
 newcom(K_SEND_SEQ, 0, OFF, send_sequence, "send_seqnce");
 newcom(K_RE_COM, 0, OFF, repeat_command, "repeat_command");
 newcom(K_ENFRAME, ALT_FLAG, OFF, enframe_cmd, "enframe");
 newcom(K_ENFRAME, ALT_FLAG, ON, enframe_alt_ind_cmd, "enframe_alt");
 newcom(K_READ_ENFR_STR, ALT_FLAG, ON, read_enfr_str, "read_enfr_str");
 newcom(K_READ_ENFR_STR, ALT_FLAG, OFF, enframe_alt_noind_cmd, "enfr_alt_no_ind");
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
 assign_help("exe_shell", "invoke shell command via command line");
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
 assign_help("mv_bow", "move one word(position at the beginning)");
 assign_help("mv_eow", "move end of word");
 assign_help("mv_word_back", "move one word backward");
 assign_help("mv_long_word_back", "move long word backward");
 assign_help("del_line", "delete_lines line");
 assign_help("del_block", "delete_lines selected block");
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
 assign_help("del_char", "delete_lines one character");
 assign_help("del_word", "delete_lines one word");
 assign_help("del_eol", "delete_lines until end of line");
 assign_help("change_word", "delete_lines one word and start edit");
 assign_help("case_word_forw", "change upper/lower case forward one word from current cursor position");
 assign_help("up_case_backw", "capitalize word backwart from current cursor position");
 assign_help("case_char", "change upper/lower case of the current character");
 assign_help("ins_edit", "start edit at the current cursor position");
 assign_help("ins_edit_after", "edit after current character");
 assign_help("new_line_after", "start edit after current line");
 assign_help("new_line_before", "start edit before current line");
 assign_help("edit_beg", "start prependig at the beginning of line");
 assign_help("edit_end", "start appending to the end of line");
 assign_help("indent", "insert character before each line of the selected text");
 assign_help("space", "insert space");
 assign_help("ins_char", "insert character");
 assign_help("ins_char_next", "insert character after current position");
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
 assign_help("read_seqnce", "read characters that can be then executed by program");
 assign_help("send_seqnce", "send recorded sequence to program");
 return 0;
}