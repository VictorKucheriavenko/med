#include <sys/time.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h> //ncurses
#include "stack.h"
#define NUM 30
#define NROWS 4
#define WIN_H 6
#define WIN_W 15
#define ESC 27

int load_file(char *f, char *l);
int cmd_load(char *l);
int print_letters(char* l);
int write_row(char *letters, int row);

main()
{
 setlocale(LC_CTYPE, "ru_RU.koi8-r");
 initscr();
 cbreak();
 keypad(stdscr, TRUE);
 noecho();

 WINDOW *menu_win;
 menu_win = newwin(WIN_H,WIN_W,(LINES-WIN_H)/2,(COLS-WIN_W)/2);

 int row = 0;
 int n = 0;
 struct timeval start, current, last_current;
 double working_time;
 long d;
 char c = 0, ch, *pcur_ch;
 char cur_ch = 0;
 int i = 0, j = 0, k = 0;

 char *st;
 st = make_stack(80);

 int r;
 FILE *init_fp;
 char initfile[80]={0};
 char letters[NROWS][NUM];//={0};
// char letters[NROWS][NUM]={0};
 for (i = 0; i < NROWS; i++)
 	for(j = 0; j < NUM; j++)
 		letters[NROWS][NUM] = 0;
 char defaultfile[]="default.ini";

 strcpy(initfile, defaultfile);

 endwin();
 load_file(defaultfile, (char*)letters);
 for (j = 0 ; j < NROWS; j++)
	for (i = 0; i < NUM; i++);
		printf("%c",letters[i][j]);
 //print_letters((char*)letters);
 initscr();

 //srand(clock());
 gettimeofday(&start, NULL);
 last_current.tv_sec = start.tv_sec;
 srand(start.tv_usec);
	
/**** init stack ****/
 for(i = 0; i < 10; i++) 
 {
	ch = 0;
	while(ch == 0)
	{
		r = rand() % NUM;
		ch = letters[row][r];
	}
	pushf(ch, st);
 }
/*******************/
 while(1)
 {
	clear();
	mvprintw(0,0, "%d", n);
	gettimeofday(&current, NULL);
/*	//mvprintw(0,30,"%f", (double)
		(current.tv_sec - last_current.tv_sec ));  */
	if ( current.tv_sec - last_current.tv_sec < 10) 
	working_time += current.tv_sec - last_current.tv_sec;
	else 
	//last_current.tv_sec += 10;
	working_time += 10;
	last_current.tv_sec = current.tv_sec;
	mvprintw(1,0,"%0.0f", (double)n/(working_time)*60);

	refresh();
	n++;

	ch = 0;
	while(ch == 0)
	{
		r = rand() % NUM;
		ch = letters[row][r];
	}

	pushl(ch, st);
	ch = popf(st);

	mvprintw(LINES/2,COLS/2,"%c",ch);
	mvprintw(LINES/2,COLS/2+1,"%s",st);
	refresh();
	move(LINES/2+1,COLS/2);
	cur_ch = 0;
	while(cur_ch!=ch)
	{
		cur_ch = getch();
		mvprintw(LINES/2+1,COLS/2, "%c",cur_ch);
		mvprintw(0,COLS-5,"wrong");
		refresh();
		move(LINES/2+1,COLS/2);
		if (cur_ch == ESC) 
		{
			clear();
			refresh();
			mvwprintw(menu_win,0,0,"1,2,3,4-");
			mvwprintw(menu_win,1,0,"change row");
			mvwprintw(menu_win,3,0,"ESC-quit");
			mvwprintw(menu_win,4,0,"l-load");
			wrefresh(menu_win);
			int exitmenu = 0;
			while (exitmenu != 1) 
			{
				cur_ch = getch();
				switch(cur_ch) {
				case '1':
					row = 0;
					exitmenu = 1;
					break;
				case '2':
					row = 1;
					exitmenu = 1;
					break;
				case '3':
					row = 2;
					exitmenu = 1;
					break;
				case '4':
					row = 3;
					exitmenu = 1;
					break;
				case 'l':
				case -60:
					cmd_load((char*)letters);
					//cmd_load(letters);
					
					/**** init stack ****/
					for(i = 0; i < 10; i++)
					{
						ch = 0;
						while(ch == 0)
						{
							r = rand()%NUM;
							ch = letters[row][r];
						}
						pushf(ch, st);
					}
					/*******************/
					exitmenu = 1;
					break;
				case 'k':
				case -52:
					endwin();
					write_row(letters[row], row);
					initscr();
					exitmenu = 1;
					break;

				case 27:
					goto end;
	
				}
			}
					break;
					}


			}
	}

	end:

	endwin();//finish ncurses
}// the end


int load_file(char *f, char *l)
{
	char c;
	int i, j;
	FILE *file;
	memset((char*)l,0,NROWS*NUM);
	file=fopen(f,"r");


	i = 0; j = 0;
	while((c != EOF) && (j < NROWS))
	{
		c = getc(file);
		if (c != 10)
	    {
		   // l[i][j]=c;
//bug!!!!!!!!!!!!!!!!!
		    *((char*)l+i+j*NUM )=c;
	    }
		else
		{
			j++;
			i = 0;
		}

		i++;
	}

	fclose(file);
}//end function load

int cmd_load(char *l)
{
	char f[20];
memset(f,0,20);
clear();
mvprintw(5,5,"enter file name: ");
refresh();
//gets(f);
//scanf("%s",f);
endwin();
	//fgets(f, 20, stdin);
	gets(f);
//puts(f);
initscr();
	load_file(f,(char*)l);
	//load_file("ruright",(char*)l);
	puts("debug");
	return 0;
}

int print_letters(char* l)
{
char c;	
int i = 0, j = 0;

	while  ( j < 4)
		{
		printf("%c\n", *((char*)l+i+j*NROWS));
		i++;
		if ( *((char*)l+i+j*NROWS) == '\0')
			j++;

		}
}


int write_row(char *letters, int row)
{
	gets(letters);
}

