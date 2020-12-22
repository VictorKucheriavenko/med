/**** command.c ****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "command.h"

cmd_t *cmd_head;
cmd_str_t* cmd_str_head;
uint32_t cmd_mask;

extern char msg[MAX_LINE];

char** split(char* str);

/****** function declarations *****/
cmd_t* getcmdhead();
int set_tab(int argn, char** argv);
int cmd_open(int argn, char** argv);
int help_comm(int argn, char** argv);

//typedef int (*func_t)();

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
 cmd_str_t *cmd;
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

cmd_t* 
command_char(char ch) {
 cmd_t *cmd;
// int (*pfunc)() = NULL;

 for ( cmd = cmd_head; cmd != NULL; cmd = cmd->next)
		if ( cmd->ch == ch)
		if ( ((((cmd_mask & cmd->flag) == cmd->flag) && (cmd->status == ON)) 
			|| (((cmd_mask & cmd->flag) == 0) && (cmd->status == OFF))) )
			return cmd;

 return NULL;
}

int
newcom(char ch, int mask, int status, int (*noargsfunc)(), char* name /* , 
		char* str, int (*func)(int argn, char** argv) */ )
{
 cmd_t *cmd;
 int i;
 int n = 0;

 if (cmd_head == NULL) {
 	cmd_head = malloc(sizeof(cmd_t));
	cmd_head->ch = ch;

	cmd_head->flag = mask;
	cmd_head->status = status;

//	cmd_head->pstr = split(str); 
 	cmd_head->next = NULL;
// 	cmd_head->func = func;
 	cmd_head->noargsfunc = noargsfunc;
 	
	cmd_head->name = strdup(name);
	cmd_head->help_str = NULL;
	return 0;
 }

 for ( cmd = cmd_head; cmd->next != NULL; cmd = cmd->next);

 cmd->next = malloc(sizeof(cmd_t));
 cmd = cmd->next;
// cmd->pch = pch;
	cmd->ch = ch;

	cmd->flag = mask;
	cmd->status = status;

// cmd->pstr = split(str);
 cmd->next = NULL;
// cmd->func = func;
 cmd->noargsfunc = noargsfunc;

 cmd->name = strdup(name);
 cmd->help_str = NULL;
 return 0;
}

int
assign_help(char* name, char* help_str)
{
 cmd_t* cmd;
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
 cmd_str_t *cmd;
 int i;
 int n = 0;

 if (cmd_str_head == NULL) {
 	cmd_str_head = malloc(sizeof(cmd_str_t));

	cmd_str_head->pstr = split(str); 
 	cmd_str_head->next = NULL;
 	cmd_str_head->func = func;
	return 0;
 }

 for ( cmd = cmd_str_head; cmd->next != NULL; cmd = cmd->next);

 cmd->next = malloc(sizeof(cmd_str_t));
 cmd = cmd->next;

 cmd->pstr = split(str);
 cmd->next = NULL;
 cmd->func = func;

 return 0;
}

int 
freecommands()
{
 cmd_t *cmd, *tmp;
 int i, j = 0;
 cmd = cmd_head;
 if(!cmd_head) 
 {
 	return 0;
 }
 for(cmd = cmd_head, j = 0, tmp = cmd_head; tmp != NULL && j < 5; cmd = tmp, j++) {
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
 cmd_str_t *cmd, *tmp;
 int i, j;
 if(!cmd_str_head) return 0;
 for(cmd = tmp = cmd_str_head, j = 0; tmp != NULL; cmd = tmp, j++) {
 	tmp = cmd->next;
	for(i = 0; (cmd->pstr[i]); i++) {
		free( (cmd->pstr[i]) );
	}
	free(cmd->pstr);
	free(cmd);
 }
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

 cmd_t* cmd;

 int i = 0;
 
//	fprintf(fp, "i");
	fprintf(fp, "ch| flag| sts|            name|description\n");// header
 
// cmd_t* cmd;
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

int
help_comm2(int argc, char** argv)
{
 int i;
 cmd_t* cmd;
 
 for (i = 0; i < 128; i++)
 {
	 for (cmd = cmd_head; cmd != NULL; cmd = cmd->next)
	 {
	 	if (cmd->ch == i)
	 	{
	 		if (iscntrl(i))
				printf("^%c\t", i + 64);
			else
				printf("%c\t", i);
//			printf("%s\t", (cmd->flag == ALT_FLAG) ? "ALT" : "-");
//			printf("%o\t", cmd->flag);
			/* flag */
			switch (cmd->flag) {
			case 0:
				printf("%s", " - ");
				break;
			case ALT_FLAG:
				printf("%s", "alt");
				break;
			case NUM_ROW_FLAG:
				printf("%s", "num");
				break;
			case SRCH_FLAG:
				printf("%s", "srch");
				break;
		/*
			case SRCH_FLAG2:
				printf("%s", "srch2");
				break;
			case SRCH_FLAG3:
				printf("%s", "srch3");
				break;
		*/
			default:
				printf("%s", "unkn");
				break;
			}
			printf("\t");
			
			if (cmd->flag)
				printf("%3s", (cmd->status == 1) ? "ON" : "OFF");
			else
				printf("---");
			printf("%16s\n", cmd->name);
		}
	 }
 }
 return 0;
}
