#define ALT_FLAG 01
#define NUM_ROW_FLAG 02
#define SRCH_FLAG 04
#define SRCH_FLAG2 010
#define SRCH_FLAG3 020

typedef struct command {
	char ch;
	int flag;
	int status; // ON/OFF
	int (*noargsfunc) ();

	struct command *next;
	struct command *prev;
} Cmd;


typedef struct command_str {
	char **pstr;
	int (*func) (int argn, char** argv);

	struct command_str *next;
	struct command_str *prev;
} Cmd_str;

//typedef int (*func_t)();

char** split(char* str);
int command(char* );
func_t command_char(char ch);
int newcom(char ch, int mask, int status, int (*noargsfunc)() );
int newcom_str(char* str, int (*func)(int argn, char** argv));
int freecommands();
int freecommands_str();
int make_commands();
int help(int, char**);
int fuck(int, char**);
int set_tab(int argn , char** argv);
int cmd_open(int argn, char** argv);