/* very old file */
/*************************/
/***** split_command *****/
/*************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "deb.h"
#define DEL 8

FILE *stddeb;

int check_char(char c)
{
 if (   (   c != '\"' && c != 32   &&   c != 10   
   &&   c != 13   &&   c != 9  
   &&  isprint( c)  )  ||  
   (   c != 32   &&   c != 10   
   &&   c != 13   &&   c != 9  
   && (   c >= -64   &&   c <= -1 ) ) )
   return 1;
 else
   return 0;
}

int check_quote(char c)
{
 if (c == '\"')
   return 1;
 else
   return 0;
}

/*cut the string on words*/
char** split (char *str)
{

//stddeb = fopen("split.log", "w");
stddeb = stdin;

	int inside = 0;
	int quotation = 0;
	int nwords = 0;
	int quote_count = 0;
	int words = 0;
	int *p_nchars;
	int i=0;
	int j=0;
	char **pptr_args;
	/*
	if (*str == 0)
		{
		printf("split error: zero string\n");
		return 1;
		}

	*/


		/*count words*/
	quote_count = 0;
	for(i=0; *(str+i) != '\0'; i++)
	  if( check_quote( *(str+i) ) == 1 )
	    quote_count++;
	fprintf(stddeb,"quote_count:%d\n", quote_count);

	i = 0;
	quotation = 0;
	quote_count = 0;
	while( *(str+i) != '\0')
	{
	    if (check_quote( *(str+i)) == 1)
	      {
	          if(quotation == 0)
		     {
		     quotation = 1;
		     nwords++;
		     }
		  else
		      quotation = 0;
		     
	      }

	   if(quotation == 0)
	    if (check_char( *(str+i)) == 1 )
	    {
		if (inside==0)
		    {
		    inside = 1;
		    ++nwords;
		    }
	    } else
		if (inside==1)
		    inside = 0;
		
	   i++;
	}
fprintf(stddeb, "stddeb:nwords: %d\n", nwords);
words = nwords;
	pptr_args=malloc(sizeof(char*) * (nwords+1) );
	p_nchars=malloc(sizeof(int)*(nwords+1) );

	for(i=0; i<nwords+1; i++)//init array
	    *(p_nchars+i)=0;

	/*count charachters in each word*/
	inside=0;
	i=0;
	nwords=0;
	while ( *(str+i) != '\0')
	{
	    if (check_quote( *(str+i)) == 1) 
	     {
	      if (quotation == 0)
	       {
	        quotation = 1;
	        nwords++;
	       }
	      else 
		quotation = 0;
	     }

            if( quotation == 1 && check_quote( *(str+i)) == 0)
	  	*(p_nchars+nwords-1) = *(p_nchars+nwords-1)+1;

	    else
	    if (check_char( *(str+i)) == 1)
	    {
		if (inside==0)
			{
			inside = 1; 
			nwords++;
			}
	  	*(p_nchars+nwords-1) = *(p_nchars+nwords-1)+1;
	    } else
		if (inside==1)
		    inside = 0;
	i++;
	}
/*
	for(i=0; *(p_nchars+i) != NULL; i++)
	   fprintf(stddeb, "stddeb: *(p_nchars+%d):%d\n", i, *(p_nchars+i));
*/
	    
	    /******allocation memory for each word*****/
	for (i=0; i < words+1; i++)
	    *(pptr_args+i) = malloc (sizeof (char) * (*(p_nchars+i)+1));
	   fprintf(stddeb, "words:%d\n", words);
	    *(pptr_args+words) = 0;


	/***** write words*******/
	inside=0;
	i=0;
	nwords = 0;//words
	int k = 0; //letters in word
	while( *(str+i) != '\0')//write words
	{

	    if (check_quote( *(str+i)) == 1) 
	     {
	      if (quotation == 0)
	       {
	        quotation = 1;
	        nwords++;
	       }
	      else 
		{
		quotation = 0;
		fprintf(stddeb, "k:%d\n",k);
		fprintf(stddeb, "nwords-1:%d\n",nwords-1);
		     *(*(pptr_args+nwords-1)+k) = '\0';
		k = 0;
		fprintf(stddeb, "ATTENTION:%d %c\n",i, *(str+i));
		}
	     }

            if( quotation == 1)
	    {
		if( check_quote( *(str+i)) == 0)
		{
		  k++;
		  *(*(pptr_args+nwords-1)+k-1) = *(str + i);
		  fprintf(stddeb, "split:write:%d:%c, k:%d\n",i , *(str + i),k );
		}
	    }
	    else /* quotation == 0 */
	    {
	       //if( check_quote( *(str+i) ) == 1 && inside == 0) k=0;

	     if( check_char( *(str+i)) == 1 )

	     {
		 if (inside==0)
		     {
		     inside = 1;
		     nwords++;
		     }
		 k++;
		 *(*(pptr_args+nwords-1)+k-1) = *(str + i);
	     } else
	     
		 if (inside==1)
		     {
		     *(*(pptr_args+nwords-1)+k) = '\0';
		     inside = 0;
		     k=0;
		     }
	    }
	i++;
	}
		
    /*
    printf("k:%d\n",k);
    printf("nwords-1:%d\n", nwords-1);
    printf("pptr_args:%p\n", pptr_args);
    */

    //printf("k = *(p_nchars+%d):%d\n",nwords-1,  *(p_nchars+nwords-1) );
    k = *(p_nchars+nwords-1);
    if(nwords>0)
    *(*(pptr_args+nwords-1)+k) = '\0';
return pptr_args;
}

