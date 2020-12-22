
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include "script.h"
#include "med.h"
#include "command.h"

//cmd_t* getcmdhead();

#ifdef COMPILE_SCRIPT

extern buffer_t* file;
extern cmd_t *cmd_head;


#define \
abort_compil(err)\
 error = err;\
 pr_error();\
 free_instr();\
 free_mem();\
 free_labels();\
 free_vars();\
 return NULL;
 
 #define \
 skip_empty_lines()\
  for (;parse_line(line) == _EMPTY_LINE; line = line->forw);\
  if ((ret = parse_line(line)) == _EOB) return scr;

int isalnum_(char c);
int isspace_(char c);

static lang_command commnd[] =
{
	{"jump", 1, 2, jump},
	{"jumpz", 2, 3, jumpz},
	{"jumpnz", 2, 3, jumpnz},
	{"assign", 2, 1, assign},
	{"incr", 1, 1, increment},
	{"decr", 1, 1, decrement},
	{"print", 1, 1, print},
	{"com", 1, ACCEPT_STR, call_command},
	{"waitkey", 0, 0, getcharrr},
};

static int ncomm = sizeof(commnd)/sizeof(lang_command);

static instr_t *instruct;

static instr_t *next_instr;

static instr_t *next_instr;

static int exec_flag;

static symbol_t *var_head;

static symbol_t *lbl_head;

static int error;

static int ln;//line no

static line_t* line;

static int nlines;

static int token_n;

static char* token[MAX_ARG];// parsed string

script_t *scr;

int
scr_init()
{
/*
	 if (scr->tail)
	 	scr->tail = scr->tail->next;	 
	 scr->head = scr->tail = malloc(sizeof(instr_t));
	 scr->tail->next = NULL;
	 
*/	 
 instruct = 0;
 next_instr = 0;
 var_head = 0;
 lbl_head = 0;
 token_n = 0;
// strcpy(token[MAX_ARG], "");
}

int
prinstr(instr_t* instr)
{
 printf("%s %s %s\n", instr->tok[0], instr->tok[1], instr->tok[2]);
 return 0;
}

int 
make_var_list()
{
 return 0;
}

int 
make_label_list()
{
 return 0;
}


int
jump_to(instr_t* arg1)
{
 next_instr = (instr_t*)arg1;
 exec_flag |= NO_INSTR_INCREMENT;
 return 0;
}

int
jump(void* arg1, void* arg2)
{
 jump_to(arg1);
 return 0;
}

int
jumpz(void* arg1, void* arg2)
{
 if (*(int*)arg2 == 0)
 	jump_to(arg1);
 return 0;
}

int
jumpnz(void* arg1, void* arg2)
{
//printf("jumpnz\n");
//prsym(lbl_head);
 if (*(int*)arg2 != 0)
 	jump_to(arg1);
 return 0;
}


int
assign(void* arg1, void* arg2)
{
 *(int*)arg1 = *(int*)arg2;
 return 0;
}

int
print(void* arg1, void* arg2)
{
 printf("%d\n", *(int*)arg1);
 return 0;
}

int
increment(void* arg1, void* arg2)
{
 (*(int*)arg1)++;
 return 0;
}

int
decrement(void* arg1, void* arg2)
{
 (*(int*)arg1)--;
 return 0;
}

int
call_command(void* arg1, void* arg2)
{
 cmd_t *cmd;
 
 for (cmd = cmd_head; cmd != NULL; cmd = cmd->next)
	if (strcmp(cmd->name, (char*)arg1) == 0)
	{
		command_char(cmd->ch);
		break;
	}
if (!cmd)
	printf("command %s not found\n", (char*)arg1);
 
 return 0;
}

int
getcharrr(void* arg1, void* arg2)
{
 printf("press any key");
 getchar();
 return 0;
}


void
pr_error()
{
 fprintf(stderr, "line %d:", line->no);
 switch (error) {
 case INVAL_CHAR:
 	fprintf(stderr, "invalid character\n");
 	break;
 case TOO_MANY_ARGS:
 	fprintf(stderr, "too many arguments\n");
 	break;
 case TOO_FEW_ARGS:
 	fprintf(stderr, "too few arguments\n");
 	break;
 case INVAL_COMM:
 	fprintf(stderr, "invalid command\n");
 	break;
 case INVAL_VAR:
 	fprintf(stderr, "invalid variable\n");
 	break;
 case INVAL_LABEL:
 	fprintf(stderr, "invalid label\n");
 	break;
 case EMPTY_LINE:
 	fprintf(stderr, "empty line\n");
 	break;
 default:
 	fprintf(stderr, "error %d\n", error);
 	break;
 	
 }
 	printf("token[0]:%s\n", token[0]);
 	printf("token[1]:%s\n", token[1]);
}

int
parse_line(line_t* l)
{

 if (!l) return _EOB;
 
 int i;
 static char str[MAX_LINE];
 
 strcpy(str, l->str);
 
 for (i = 0; i < MAX_ARG; i++)
 	token[i] = NULL;
 
 for (i = 0; str[i] && str[i] != ':'; i++);
 
 if (str[i] == ':')
 {
 	str[i] = 0;
 	i--;
 	for (; i >= 0 && isalnum_(str[i]); i--);
 	i++;
 	token[0] = str + i;
 	return _LABEL;
 }
 
 for (i = 0; isspace_(str[i]); i++);
 
 if (str[i] == '\0') 
 {
 	return _EMPTY_LINE;
 }
 	
 for (token_n = 0; token_n < MAX_ARG && str[i] != '\0'; token_n++)
 { 
	 token[token_n] = str + i;
	 for (; isalnum_(str[i]); i++);
	 if (str[i] == '\0')
	 {
	 	str[i] = 0;
	 	i++;
	 	break;
	 }
	 str[i] = 0;
	 i++;
	 for (; isspace_(str[i]); i++);
	 if (str[i] == '\0')
	 {
//	 	ln++;
 	 	break;
	 }
 }
	 token_n = token_n + 1;
	 
 return token_n;
}


int
prsym(symbol_t* head)
{
 int i;
 symbol_t *ptr;
  for(i = 0, ptr = head; ptr; ptr = ptr->next, i++)
  	printf("%d %p \"%s\"\n", i, ptr->p, ptr->name);
 return 0;
}

script_t*
compile(line_t* head)
{
 scr_init();
 int h, i, j, l, m, ret;
 int is_label = 0;
 symbol_t *tmp, *var, *lbl;
 instr_t *instr;
 
/* init line numbers */ 
 for (i = 0, line = head; line; line = line->forw, i++)
 	line->no = i + 1;
 	
 line = head;
 	
 
 scr = calloc(1, sizeof(script_t));
 
 skip_empty_lines();
 
 /* search 'program' */
 if (strcmp(token[0], "program") != 0 || token[1] == NULL)
 	scr->name = "program 1";// [i]
 else
 {
 	scr->name = strdup(token[1]);
 	line = line->forw;
 }
 
 /* search 'memory' */
 
 skip_empty_lines();
 
// printf("line->str:%s\n", line->str); 
 if (strcmp(token[0], "memory") != 0 || token[1] == NULL)
 {
 	 scr->memory = DEFAULT_MEMORY;
 }
 else
 {
	 scr->memory = atoi(token[1]);// strol, check return value of malloc
	 line = line->forw;
 }
 scr->memory_pool = malloc(scr->memory);
 scr->memory_pos = scr->memory_pool;
 
 /* search variable definition */
 
 
 skip_empty_lines();
 
 
 if (strcmp(token[0], "defvar") == 0)
 {
 	for (i = 1; i < token_n; i++)
 	{
	 	var = malloc(sizeof(symbol_t));
	 	var->name = strdup(token[i]);
	 	var->p = (int*)(scr->memory_pos);
	 	scr->memory_pos += sizeof(int);
	 	var->next = var_head;
	 	var_head = var;
	} 
//	prsym(var_head);
	
	line = line->forw;
 }
 else
 {
 }
 
 /* create instructions */
 
 for (; line; line = line->forw) 
 {
 
 skip_empty_lines();
 	 	
	 if (ret == _LABEL)
	 {
	 	is_label++;
	 	lbl = malloc(sizeof(symbol_t));
	 	lbl->next = lbl_head;
	 	lbl_head = lbl;
	 	lbl_head->name = strdup(token[0]);
	 	
//	 	prsym(lbl_head);
		 
	 	continue;
	 }
	 	
	 	
	   
	 if (!scr->head)
	 	scr->head = scr->tail = malloc(sizeof(instr_t));
	 else
	 {
	 	scr->tail->next = malloc(sizeof(instr_t));
	 	scr->tail = scr->tail->next;	 
	 }
	 scr->tail->next = NULL;
	 scr->tail->func = NULL;
	 
	 for (i = 0; i < 3; i++)
	 	if (token[i])
	 		scr->tail->tok[i] = strdup(token[i]);
	 	else
	 		scr->tail->tok[i] = "empty";
 	
 	/* assign labels */
 	lbl = lbl_head;
 	while(is_label)
 	{
 		lbl->p = (int*)scr->tail;
 		lbl = lbl->next;
 		is_label--;
 	}
	 
	 
	 /* search command */

	 for (j = 0; j < ncomm /*&& (strcmp(token[0], commnd[j].name) != 0)*/; j++);
	 for (j = 0; j < ncomm && (strcmp(token[0], commnd[j].name) != 0); j++);
	 if (j == ncomm) 
	 {
	 	abort_compil(INVAL_COMM);
	 }
	 
	 scr->tail->func = commnd[j].func;


	if (commnd[j].nargs == 0)
		continue;
		
	// first arg
if (isdigit(*token[1]))
{
	scr->tail->arg[0] = (int*)malloc(sizeof(int));
	*(int*)scr->tail->arg[0] = atoi(token[1]);
}
else
{
	
	if (commnd[j].flags & ACCEPT_LABEL || commnd[j].flags & ACCEPT_STR)
	{
		scr->tail->arg[0] = strdup (token[1]);
	}
	else
	if (commnd[j].flags & ACCEPT_VAR)
	{
		for (var = var_head; var != NULL; var = var->next)
			if (strcmp(var->name, token[1]) == 0)
			{
				scr->tail->arg[0] = var->p;
//				printf("command[%d].name: %s\n", j, commnd[j].name);
				break;
			}
			else
			{
			}
			if (var == NULL)
			{
				abort_compil(INVAL_VAR);
			}
		}
	}//else
	 
	// second arg
	if (token_n > 1 && commnd[j].nargs > 1)
	{
//	printf("token_n > 1:%d\n", token_n);
prinstr(scr->tail);
		 if (isdigit(*token[2]))
		 {
			 scr->tail->arg[1] = (int*)malloc(sizeof(int));
			 *(int*)scr->tail->arg[1] = atoi(token[2]);
		 }
		 else
			 if (commnd[j].flags & ACCEPT_VAR)
			 {
			 	for (var = var_head; var != NULL; var = var->next)
				if (strcmp(var->name, token[2]) == 0)
				{
					scr->tail->arg[1] = var->p;
	//				printf("command[%d].name: %s\n", j, commnd[j].name);
					break;
				}
				else
				{
				}
				if (var == NULL)
				{
					abort_compil(INVAL_VAR);
				}
			 }
	 	
	 	}
 }//for (; line...
 
	
/* "second pass" */
	
 for (instr = scr->head; instr; instr = instr->next)
 {
// if (instr->arg[1] == (void*)REPL_LBL)
 if (instr->func == jump || instr->func == jumpz || instr->func == jumpnz)
 {
// printf("%p\n", instr->func);
 	for (lbl = lbl_head; lbl; lbl = lbl->next)
 	if (strcmp(instr->arg[0], lbl->name) == 0)
 	{
// 	printf("??????\n");
 		free(instr->arg[0]);
 		instr->arg[0] = lbl->p;
 		break;
 	}
	if (lbl == NULL)
	{
		abort_compil(INVAL_LABEL);
	}
 }
}//for instr...
		 	
 return scr;
}


void
pr_instr(script_t* scr)
{
 instr_t* pinstr;
 int j;
 printf("all instructions:\n");
 for (pinstr = scr->head; pinstr != NULL; pinstr = pinstr->next)
 {
 	for (j = 0; j < ncomm; j++)
 		if (commnd[j].func == pinstr->func)
 			printf("command:%s\n", commnd[j].name);
 }
 printf("end instructions\n");
 
 /*
 for (j = 0; j < ncomm; j++)
 if (commnd[j].func == scr->tail->func)
 		printf("tail->command:%s\n", commnd[j].name);
*/ 		
}

int
exec_script(script_t* scr)
{
 int i;
 int j;
 symbol_t *lbl;
 
 if (!scr)
 {
  	fprintf(stderr, "exec_script: can't execute script %p\n", scr);
 	return 1;
 }
 
 
 for (i = 0, next_instr = scr->head; next_instr != NULL; i++)
 {
 /*
	for (j = 0; j < ncomm && next_instr->func != commnd[j].func; j++);
	printf("%s-", commnd[j].name);
 	printf("%p-%p-%p\n", next_instr->func, next_instr->arg[1], 
 			next_instr->arg[2]);
*/ 	
 	next_instr->func(next_instr->arg[0], next_instr->arg[1]);
 	if (!(exec_flag & NO_INSTR_INCREMENT))
 		next_instr = next_instr->next;
 	else
 		exec_flag &= ~NO_INSTR_INCREMENT;
 		
 }
 	
 printf("exec_script:success:executed %d instructions\n", i); 	
 return 0;
}

int
free_comm()
{
 return 0;
}

int
free_instr()
{
 return 0;
}

int
free_mem()
{
 free (scr->memory_pool);
 return 0;
}

int
free_vars()
{
 return 0;
}

int
free_labels(symbol_t* head)
{
 int i;
 symbol_t *sym;
 
 while (sym)
 {
 	sym = head;
 	head = sym->next;
 	free (sym->name);
 	free (sym);
 }
 return 0;
}

#endif