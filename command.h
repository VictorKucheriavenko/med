#include "med.h"

#define ALT_FLAG 01
#define NUM_ROW_FLAG 02
#define SRCH_FLAG 04
#define SRCH_FLAG2 010
#define SRCH_FLAG3 020

#define ON 1
#define OFF 0

typedef struct command {
	char ch;
	int flag;
	int status; // ON/OFF
	int (*noargsfunc) ();
	char* name;
	char* help_str;

	struct command *next;
	struct command *prev;
} cmd_t;


typedef struct command_str {
	char **pstr;
	int (*func) (int argn, char** argv);

	struct command_str *next;
	struct command_str *prev;
} cmd_str_t;

//typedef int (*func_t)();

char** split(char* str);
int command(char* );
cmd_t* command_char(char ch);
int newcom(char ch, int mask, int status, int (*noargsfunc)(), char* name);
int newcom_str(char* str, int (*func)(int argn, char** argv));
int assign_help(char* name, char* help_str);
int freecommands();
int freecommands_str();
int make_commands();
int make_help();
int help_comm(int argn, char** argv);
int help_comm2(int argn, char** argv);
/*
int help_comm(int, char**);
int fuck(int, char**);
*/
int set_tab(int argn , char** argv);
int cmd_open(int argn, char** argv);