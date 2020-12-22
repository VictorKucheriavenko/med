/********find_s.c********/
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "find.h"

char* reg (char* s, char *t, int *l) {   //s - string to find in string t  
if (s == NULL) 
	{
	printf("find_s: first argument is NULL\n");
	return NULL;
	}
if  (t == NULL) 
	{
	printf("find_s: second argument is NULL\n");
	return NULL;
	}
//print_ascii(s);
int i = 0;
int j=0;
int k=0;//temp counter if match
int reg_ret;
regex_t *exp;
regmatch_t rm[10];
char ch_tmp;
char* p_tmp;
int len = 0;
char* t_end;//end of string t

t_end = t + len;
len = strlen(t);
exp = malloc(sizeof(regex_t));

reg_ret = regcomp(exp, s, 0);
if (reg_ret != 0)
{
 printf("regcomp error\n");
 return reg_ret;
}

reg_ret = 1;
while(reg_ret && *t != '\0') 
{
 if(t + SS_LEN < t_end)
 {
     p_tmp = t + SS_LEN;
     ch_tmp= *p_tmp;
     *p_tmp = '\0';
     reg_ret=regexec(exp, t, 1, rm, 0);
     *p_tmp = ch_tmp;
 }
 else
     reg_ret=regexec(exp, t, 1, rm, 0);
 t++;
}
*l=rm[0].rm_eo-rm[0].rm_so;
 for(i=0; i<10; i++)
 printf("rm_so:%d, rm_eo:%d\n", rm[i].rm_so, rm[i].rm_eo);
regfree(exp);
if(!reg_ret)
 return --t;
else
 return NULL;
}

char* find_reg (char* s, char *t, int *l) {   //s - string to find in string t  
if (s == NULL) 
	{
	printf("find_s: first argument is NULL\n");
	return NULL;
	}
if  (t == NULL) 
	{
	printf("find_s: second argument is NULL\n");
	return NULL;
	}
//print_ascii(s);
int i = 0;
int j=0;
int k=0;//temp counter if match
int reg_ret;
regex_t *exp;
regmatch_t rm[10];
char ch_tmp;
char* p_tmp;
int len = 0;
char* t_end;//end of string t

t_end = t + len;
len = strlen(t);
exp = malloc(sizeof(regex_t));

reg_ret = regcomp(exp, s, 0);
if (reg_ret != 0)
{
 printf("regcomp error\n");
 return reg_ret;
}

reg_ret = 1;
while(reg_ret && *t != '\0') 
{
 if(t + SS_LEN < t_end)
 {
     p_tmp = t + SS_LEN;
     ch_tmp= *p_tmp;
     *p_tmp = '\0';
     reg_ret=regexec(exp, t, 1, rm, 0);
     *p_tmp = ch_tmp;
 }
 else
     reg_ret=regexec(exp, t, 10, rm, 0);
 t++;
}
*l=rm[0].rm_eo-rm[0].rm_so;
 for(i=0; i<10; i++)
 printf("rm_so:%d, rm_eo:%d\n", rm[i].rm_so, rm[i].rm_eo);
regfree(exp);
if(!reg_ret)
 return --t;
else
 return NULL;
}




char* find_s (char* s, char *t) {	//s - string to find in string t  

if (s == NULL) 
{
	printf("find_s: first argument is NULL\n");
	return NULL;
}

if  (t == NULL) 
{
	printf("find_s: second argument is NULL\n");
	return NULL;
}

//print_ascii(s);
int i = 0;
int j=0;
int k=0;

	while( *(t+j) != '\0' )
	{
		i=0;
		k=j;
/*
		for(; *(s+i) == ' ' || *(s+i) == '\t'; i++);
		for(; *(t+k) == ' ' || *(t+k) == '\t'; k++);
*/
		while( *(s+i) == *(t+k) )
		{
			if( *(s+i+1) == '\0' )
				return (t+j);///!!!! j
/*
		if( *(s+i) == ' ' || *(s+i) == '\t') 
		{
			i++;
			continue;
		}
		if( *(t+k) == ' ' || *(t+k) == '\t') 
		{
			k++;
			continue;
		}
*/
/*
			if( *(t+k) == '\0' )
				return NULL;//???
*/
			i++;
			k++;

		}


		j++;
	}
return NULL;
}

char* find_ss (char* s, char *t) {	//skip spaces

if (s == NULL) 
{
	printf("find_s: first argument is NULL\n");
	return NULL;
}

if  (t == NULL) 
{
	printf("find_s: second argument is NULL\n");
	return NULL;
}

//print_ascii(s);
int i = 0;
int j=0;
int k=0;

	while( *(t+j) != '\0' )
	{
		i=0;
		k=j;
		while( 1 ) 
		{
			if( *(s+i) == ' ' || *(s+i) == '\t') 
			{
				i++;
				continue;
//printf("debug"); getchar();
			}
			if( *(t+k) == ' ' || *(t+k) == '\t') 
			{
				k++;
				continue;
			}
			if( *(s+i+0) == '\0' )
				return (t+j);///!!!! j
/*
				if( *(t+k) == '\0' )
					return NULL;//???
*/
			if ( *(s+i) != *(t+k) )
				break;
			else
			{
//printf(" %c %c ", *(s+i), *(t+k) ); getchar();
				i++;
				k++;
			}
			if( *(s+i+0) == '\0' )
				return (t+j);///!!!! j

		}
		j++;
	}

return NULL;
}

char* find_s_reverse (char* s, char *t) {/*s - string to find
					in string t  (last char) */
if (s == NULL) 
	{
	printf("find_s: first argument is NULL\n");
	return NULL;
	}
if  (t == NULL) 
	{
	printf("find_s: second argument is NULL\n");
	return NULL;
	}
int i = 0;
int j=0;
int k=0;//temp counter if match


	while( *(t-j) != '\0' )
	{
		i=0;
		k=j;
		while( *(s+i) == *(t-k) )
		{
			if( *(s+i+1) == '\0' )
				return (t-j);///!!!! j
			if( *(t-1) == '\0' )
				return NULL;
			/*
			if( *(t+i) == '\0' )
				//return NULL;//???
				continue;
				*/
			i++;
			k--;

		}


		j++;
	}
return NULL;
}
