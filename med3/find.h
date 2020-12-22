#include "deb.h"
#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <regex.h>
#define MAX 1000 
#define SS_LEN 500 // search_string length
#define COMMANDS_NUM 200
#define ON 1
#define OFF 0
#define REG_NUM 1 
#define FIND 1
#define CMND 2
#define MUELLER 3


typedef struct p_link {
	char *search;
	struct p_link *next;
	int n;
	//struct p_link prev;
	} P_link;


typedef struct buffer {
	FILE *fp;
	char *start_pos;
	char *end_pos;
	regmatch_t res[REG_NUM];
	char *search_pos;
	int search;
	char *print_pos;
	int print;
	char *file_name;
	char *title_pos;
	char *author_pos;
	char *str;
	char *pos;
	char *pos1;
	char *pos2;
	long len;
	int n;
	char *left;
	char *right;
	int number;
	int power;
	} Buff;

Buff *pbuff;
Buff *cur_buff;

char* open_text_buffer(char *file);
int prints(char* s, int n);
int printsr(char* s, int n);
int printch(char* s, int n);
int w_prints(char* s, int n);
char *find_s(char* s, char* t);
char *find_reg(char* s, char* t, int *l);
char *find_s_reverse(char* s, char* t);
//P_link* sentence_borders(char* str, char* pos); 
//sentence_borders( Buff *curr_buff, char  b, int n);
int sentence_borders(char* str, char* pos, char b, int n, 
	char** l, char** r, int max);
int print_sentence(char *left, char *right, int n);
int print_sentence_color(char *left, char *right, char *col1, char *col2, int n);
char** split(char *str);
int compare_string(char *s1, char *s2) ;
int getcommand(char*);
long parseint(char*);
int cmd_command(char** );
int cmd_mueller(char* str);
int reset_search();

char *found [MAX];
int search_entry_counter;
char arch[1000];//search entries

int border;//1,2,3,... lines, sentenses, ....x....
int border_n;
int random_f;//is random flag 
int search_f;
int print_max;
int color;
int how_much_to_show;//maximum to ouput
//char command_prefix[100];
char *command_prefix;
char *mueller_prefix;
char *finder_prefix;
char *change_mode_prefix;
char* command_prompt;
char* mueller_prompt;
char* finder_prompt;
int prompt_mode;

int global_counter;
 
typedef struct cmd{
char** cmd_str;
int (*cmd_func)(int, char**);
struct cmd *next;
struct cmd *prev;
}Cmd;
// Cmd* head_cmd_global_scope_pointer;
Cmd* global_ptr;
Cmd* global_head_ptr;
//Cmd *pCmd[COMMANDS_NUM];
FILE* m_file;

