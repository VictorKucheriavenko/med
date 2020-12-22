int save_cur();
int rest_cur();
int getsize ();
int setnocanon();
int setcanon();
int setnoecho();
int setecho();
int getpos(int *y, int *x);
int set_tabs(int ncols, int tabs);
char* del (char *str, int o, int n);
int del_char();
int del_char_right();
char* insert (char *str1, int n, char *str2);
int getstring(char* buf, int nmax);
line_t* make_line_after(line_t *prev);
line_t* make_line_before(line_t *next);
int make_chain();
int make_line();
int insert_string();
int insert_space();
int insert_char(char ch);
int read_line(FILE* file, char* buf);
int read_buffer(FILE* file, line_t **head, line_t **tail);
int message(char* s);
int write_buf(int fd);
int work_buffer();
int move_down(int n);
int move_up(int n);
int move_first();
int move_last();
int move_left(int n);
int move_right(int n);
int move_tab_forw(int pos);
int move_tab_backw(int pos);
int print_line(char* str);
int del_line();
int output_line(line_t* l);
int print_lines(int s, int n);
int print_cur();
char* find_str();
int goto_curline();
