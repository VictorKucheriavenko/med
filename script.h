#include "med.h"

#define MAX_ARG 128

#define DEFAULT_MEMORY 256

#define _LABEL -2
#define _EOB -1
#define _EMPTY_LINE 0
#define _SUCCESS 1

#define REPL_LBL 1

#define NO_INSTR_INCREMENT 1

#define INVAL_CHAR 1
#define TOO_MANY_ARGS 2
#define TOO_FEW_ARGS 3
#define INVAL_COMM 4
#define EMPTY_LINE 5
#define NO_PROG_NAME 6
#define INVAL_VAR 7
#define INVAL_LABEL 8

#define ACCEPT_VAR 01
#define ACCEPT_LABEL 02
#define ACCEPT_STR 04

typedef int (*lang_func_t) (void*, void*);

typedef struct {
	char *name;
	int nargs;//number of arguments command accepts
	int32_t flags;
	lang_func_t func;
} lang_command;

/*
typedef struct {
	char *name;
} lang_keyword;
*/

typedef struct  lang_instruction{
	lang_func_t func;
	void* arg[2];
	char* tok[3];
	struct lang_instruction* next;
} instr_t;

typedef struct {
	instr_t* head;
	instr_t* tail;
	long memory;
	char* memory_pool;
	char* memory_pos;
	char* name;
} script_t;

typedef struct symbol {
	char* name;
	int *p;
/*
	union {
		int *p;
		struct instr_t *p;
	}
*/
	struct symbol *next;
} symbol_t;


void pr_instr(script_t* scr);
int prsym();
int init();
int make_var_list();
int make_label_list();
int jump_to(instr_t*);
int jump(void*, void*);
int jumpz(void*, void*);
int jumpnz(void*, void*);
void pr_error();
int assign(void*, void*);
int print(void*, void*);
int increment(void*, void*);
int decrement(void*, void*);
int call_command(void* arg1, void* arg2);
int getcharrr(void* arg1, void* arg2);
int compile_error(int err);
int parse_line(line_t* l);
script_t* compile(line_t* head);
int free_comm();
int free_instr();
int free_mem();
int free_labels();
int free_vars();
int exec_script();