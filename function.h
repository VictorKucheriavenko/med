int main( int argn, char *argv[]);
int switch_mode (int flag);
int cmd_switch_alt_mode();
int cmd_switch_num_row_mode();
int cmd_switch_search_mode();
int switch_global();
int invoke_command();
int save_cur();
int rest_cur();
int getsize ();
int setnocanon();
int setcanon();
int setnoecho();
int setecho();
int getpos(int *y, int *x);
int del (char **pstr, int o, int n);
int del_char();
int case_word();
int case_backward();
int case_char();
int move_word_back();
char* insert_str (char **pstr1, int n, char *str2);
char* insert_ch (char **pstr, int n, char c);
int insert_indent();
int simple_print(int n, char* str);
int readstring(char* buf, int nmax);
int getstring(char* buf, char* str);
int insert_string();
int get_next_move(char c);
int insert_string_after();
int cmd_edit_new_line_before();
int cmd_edit_new_line_after();
int cmd_edit_line_begin();
int cmd_edit_line_end();
int concat_lines();
int calculate_all_lengths();
int destroy_line(line_t *line);
line_t* make_line_after(line_t *prev);
line_t* make_line_before(line_t *next);
int cmd_make_line_before();
int cmd_make_line_after();
int init_line( line_t *ln );
int make_line();
int change_char();
int insert_space();
int insert_char();
int insert_char_next();
int read_line(FILE* file, char* buf);
line_t* read_file(FILE* file);
int open_file(char *name);
int close_file();
int reopen_file ();
int message(char* s);
int fmessage(char* format, ...);
int gmessage(char* s);
int write_buf(int fd);
int save_file();
int save_all();
int make_backup_copy(char* fname);
int remove_backup(char* fname);
int cur_up();
int cur_down();
int move_down(int n);
int move_down_half_screen();
int move_up(int n);
int move_up_half_screen();
int move_down_smooth(int n);
int move_up_smooth(int n);
int move_down_one();
int move_up_one();
int move_up_block();
int move_down_block();
int move_first_line();
int move_last_line();
int position_cursor();
int reset();
int move_first_col();
int move_last_col();
int move_forward(int n);
int force_forward(int n);
int move_forward_one();
int move_forward_hs();
int move_backward1(int n);
int move_backward(int n);
int move_backward_one();
int move_backward_hs();
int nexttab();
int nexttab3(int x, int o);
int nexttab1();
int get_bow();
int get_eow();
int move_bow();
int move_eow();
int del_word();
int cmd_del_word();
int del_endline();
int cmd_del_endline();
int cmd_change_word();
int print_top_line();
int print_bottom_line();
int print_line(int line, char *str);
int print_line_x(int o, int line, char *str);
int print_line_x_color(int o, int line, char *str, int col);
int print_color_line(line_t* l, int pos);
int print_cur_line();
int redraw_screen();
char* find_ss (char* s, char *t);
int simple_find();
int cmd_simple_find();
int find_name();
int cmd_find_name();
int find_beginning();
int cmd_find_beginning();
int refind();
int alt_refind();
int search_and_replace(line_t* start_srch, line_t* end_srch, int name);
int cmd_replace_whole();
int cmd_replace_global();
int cmd_rename_global();
int cmd_replace_marks();
int cmd_rename_whole();
int cmd_rename_marks();
int cmd_repeat_repl_whole();
int cmd_repeat_repl_marks();
int cmd_repeat_rename_whole();
int cmd_repeat_rename_marks();
int copy_lines(line_t *start, line_t *end, line_t **buf_head, line_t **buf_tail);
int insert_lines(line_t *place, line_t *buf_head, line_t *buf_tail);
int insert_lines_before(line_t *place, line_t *buf_head, line_t *buf_tail);
int set_mark1();
int set_mark2();
int auto_set_mark();
int set_mark(int n);
int cmd_set_mark();
int go_mark_number(int n);
int cmd_go_mark_number();
int go_mark_1();
int go_mark_2();
int go_mark_3();
int go_mark_4();
int go_mark_5();
int go_mark_6();
int go_mark_7();
int go_mark_8();
int go_mark_9();
int go_mark_next();
int go_mark_prev();
int go_mark_last ();
int go_mark(line_t *mark);
int arrange_markers(line_t** m1, line_t** m2);
int cmd_yank();
int cmd_paste();
int cmd_paste_before();
int delete(line_t *l, line_t *m);
int del_lines(line_t *l, line_t *m);
int cmd_del_block();
int cmd_del_line();
int free_lines(line_t *l, line_t *m);
int free_lines(line_t *l, line_t *m);
int cur_line_number();
int set_cur_line(int n);
line_t* get_head();
line_t* get_tail();
int show_bar();
int show_buffers();
line_t* goto_line( int n );
int cmd_goto_line();
void sigsegv_handler(int i);
int quit();
int cmd_quit();
void sig_handler();
int file_buffer_next();
int file_buffer_prev();
int file_buffer_go(int n);
int file_buffer_go_1 ();
int file_buffer_go_2 ();
int file_buffer_go_3 ();
int file_buffer_go_4 ();
int file_buffer_go_5 ();
int file_buffer_go_6 ();
int file_buffer_go_7 ();
int file_buffer_go_8 ();
int file_buffer_go_9 ();
int file_buffer_go_last (int n);
int read_key_pressed();
int write_key_pressed();
int change_color(int col);
int cmd_change_color();
int cmd_dead_key_pressed();
int cmd_open(int argn, char** argv);
int cmd_rename(int argn, char** argv);
int set_jump(int argn, char** argv);
int cmd_set_autoindent();
int set_autoindent(int argc, char** argv);
int indent_cur_line();
int cur_len();
int test();
