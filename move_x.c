#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "med.h"
#include "move_x.h"
#include "macros.h"

#define HS 15

extern int _wintop, _winbot, _wincur;
extern int trm_width;
extern buffer_t* file;
extern line_t* cur_line;
extern int offset;
extern int x_curs;
extern int _h_scroll;
extern int autoindent;
extern int tab_len;

int
place_cursor()
{
 moveyx(_wincur, file->x_curs);
// movex(file->x_curs);//not working
 return 0;
}


int
get_next_move_x()
{
 if (cur_str[offs] == '\0')
	return 1;

 if (cur_str[offs] == '\t')
	file->x_curs = nexttab1();
 else
	file->x_curs += 1;
	
 offs++;
 
 return 0;
}


int
move_forward(int n)
{
 int i;
 int need_reprint = 0;

 file->cur_line->len = cur_len();

 for( i = 0; i < n; i++ )
// if (cl->len - offs - 1>0) {
 if ( offs + 1 < file->cur_line->len ) { 
// if (cur_str[offs]) {

	 if( *(cur_str + offs ) == '\t' )
		file->x_curs = nexttab1();
	 else
		file->x_curs++;

	 while (file->x_curs > trm_width) {
		 file->cur_line->x -= _h_scroll;
		 file->x_curs -= _h_scroll;
		 need_reprint = 1;
	 }
	 offs++;
 }

 if(need_reprint == 1)
   print_cur_line();
 place_cursor();
 return 0;
}


int
move_backward(int n)
{
 int i, tmp = offs;
 int tmp_x = cl->x;
 
 for( i = 0; i < n && offs != 0; i++ ) {
	reset_x();
	
	move_forward(--tmp);
	
	if (tmp_x != cl->x)
		print_cur_line();
 }
 return 0;
}

int
position_cursor()
{
 int tmp = file->x_curs;
 move_first_col();
 while(file->x_curs < tmp && offs + 1 < file->cur_line->len)
 move_forward(1);
}


int
reset_x()
{
 offs = 0;
 file->x_curs = 1;
 file->cur_line->x = 1;
 return 0;
}


int
move_first_col()
{
 offs = 0;
 file->x_curs = 1;
 file->cur_line->x = 1;
 moveyx(_wincur, 1);
 print_cur_line();
 return 0;
}


int
move_last_col()
{
// move_forward(file->cur_line->len);
 move_forward(cur_len());
 return 0;
}


int
force_forward(int n)
{
 int i;
 int need_reprint = 0;

 file->cur_line->len = cur_len();


 for( i = 0; i < n; i++ )
/* if ( offs + 1 < file->cur_line->len ) */{ 

	 if( *(cur_str + offs ) == '\t' )
		file->x_curs = nexttab1();
	 else
		file->x_curs++;

/*
	 while (file->x_curs > trm_width) {
		 file->cur_line->x -= _h_scroll;
		 file->x_curs -= _h_scroll;
		 need_reprint = 1;
	 }
	 offs++;
*/
 }

/*
 if(need_reprint == 1)
   print_cur_line();
*/
 place_cursor();
 return 0;
}


int
move_forward_one()
{
 move_forward(1);
 return 0;
}


int
move_forward_hs()
{
 move_forward(HS);
 return 0;
}


int
move_backward1(int n)
{
 int i, tmp;
 int need_reprint = 0;

 for( tmp = offs, i = 0; i < n && tmp != 0; tmp--, i++ )
	;
 offs = 0;
 file->x_curs = 1;

/* move forward */
 while( offs != tmp ) 
 if ( offs + 1 < file->cur_line->len ) { 
	 if( *(cur_str + offs + 0) == '\t' ) {
		file->x_curs = nexttab1();
	 }
	 else
		file->x_curs++;

	 while (file->x_curs > trm_width) {
//		 file->cur_line->x -= _h_scroll;
		 file->x_curs -= _h_scroll;
//		 need_reprint = 1;
	 }
	 offs++;
 }
gmessage("left");
message("\t\t");

	 while (file->x_curs < 1 ) {
gmessage("< 1");
message("\t\t");
		 file->cur_line->x += _h_scroll;
		 file->x_curs += _h_scroll;
		 need_reprint = 1;
	 }

// if(need_reprint == 1)
   print_cur_line();
 place_cursor();
 return 0;
}


int
move_backward_one()
{
 move_backward(1);
 return 0;
}


int
move_backward_hs()
{
 move_backward(HS);
 return 0;
}


int
nexttab()
{
 int i;
 int y, x;
 getpos(&y, &x);
 i = (x - 1) / tab_len * tab_len + tab_len + 1;
 return i;
}


int
nexttab3(int x, int o)
{
 int i;
 i = (x - 1) / tab_len * tab_len + tab_len + 1;
 return i + o;
}


int
nexttab1()
{
 return (file->x_curs + (file->cur_line->x - 1) - 1) / tab_len * tab_len + tab_len + 1
		- (file->cur_line->x - 1);
}


int
get_bow()
{
 int i, tmp;
 char *str;

 str = cur_str;
 i = tmp = offs;
 for ( ; isalnum( str[i] ) && str[i] != 0; i++ );
 for ( ; !isalnum( str[i] ) && str[i] != 0; i++ );
 
 return i;
}


int
get_eow()
{
 int i, tmp;
 char *str;

 str = cur_str;
 i = tmp = offs;
 for ( ; !isalnum( str[i] ) && str[i] != 0; i++ );
 for ( ; isalnum( str[i] ) && str[i] != 0; i++ );
 
 return i;
}

int
get_long_eow()
{
 int i, tmp;
 char *str;

 str = cur_str;
 i = tmp = offs;
 for ( ; !isalnum( str[i] ) && str[i] != 0; i++ );
 for ( ; (isalnum( str[i] ) || str[i] == '_') && str[i] != 0; i++ );
 
 return i;
}


int
move_bow()
{
 int i = 0;
 i = get_bow();
 move_first_col();
 move_forward(i);
}


int
move_eow()
{
 int i = 0;
 i = get_eow();
 move_first_col();
 move_forward(i);
}


int
move_word_back()
{
 int tmp = 0;
 move_backward(1);  
 tmp = offs;
 
 while( isspace(cur_str[tmp]) )
	tmp--;
		
 if( isalnum(cur_str[offs]) )
 {
	while( isalnum(cur_str[tmp]) )
		tmp--;
 }
 else
 if( !isalnum(cur_str[offs]) && !isspace(cur_str[offs]))
 {
	while( isspace(cur_str[tmp]) )
		tmp--;
 	while( !isalnum(cur_str[tmp])
 		&& !isspace(cur_str[tmp]))
		tmp--;
/*
	while( isalnum(cur_str[tmp]) )
		tmp--;
*/
 }
 else if (isspace(cur_str[offs]))
 {
	while( isspace(cur_str[tmp]) )
		tmp--;
	if (isalnum(cur_str[tmp]))		
	while (isalnum(cur_str[tmp]))
		tmp--;
	if (!isalnum(cur_str[tmp]) && !isspace(cur_str[tmp]))
	while (!isalnum(cur_str[tmp]) && !isspace(cur_str[tmp]))
		tmp--;
 }
 tmp++;
 move_first_col();
 move_forward(tmp+0);
 
 return 0; 
}

// newcom(K_MV_WORDBACK, 0, ON, move_long_word_back, "mv_long_word_back");
int
move_long_word_back()
{
 int tmp = 0;
 move_backward(1);  
 tmp = offs;
 if( isalnum(cur_str[offs]) || cur_str[offs] == '_')
	 while( isalnum(cur_str[tmp]) || cur_str[tmp] == '_')
	   tmp--;
 else
	 while( !isalnum(cur_str[tmp]) && cur_str[tmp] != '_')
	   tmp--;
 tmp++;
 move_first_col();
 move_forward(tmp+0);
 
 return 0; 
}
