#define ctrl(c) ((c) - 96)
#define C_a '\001'
#define C_b '\002'
#define C_c '\003'
#define C_d '\004'
#define C_e '\005'
#define C_f '\006'
#define C_g '\007'
#define C_h '\010'
#define C_i '\011'
#define C_j '\012'
#define C_k '\013'
#define C_l '\014'
#define C_m '\015'
#define C_n '\016'
#define C_o '\017'
#define C_p '\020'
#define C_q '\021'
#define C_r '\022'
#define C_s '\023'
#define C_t '\024'
#define C_u '\025'
#define C_v '\026'
#define C_w '\027'
#define C_x '\030'
#define C_y '\031'
#define C_z '\032'
#define Grave '\140'


#define K_CMD 'q'
//#define K_TEST '\026'
#define K_TEST 'T'

#define K_SCRIPT C_k
#define K_MAKE_RUN 'A'
#define K_FUNC_TEMPL 'B'
#define K_F_BUF_CLOSE C_q
#define K_F_BUF_REOPEN C_a 
#define K_F_BUF_NEXT '='
#define K_F_BUF_PREV '-'
#define K_F_BUF_GO_1 '1'
#define K_F_BUF_GO_2 '2'
#define K_F_BUF_GO_3 '3'
#define K_F_BUF_GO_4 '4'
#define K_F_BUF_GO_5 '5'
#define K_F_BUF_GO_6 '6'
#define K_F_BUF_GO_7 '7'
#define K_F_BUF_GO_8 '8'
#define K_F_BUF_GO_9 '9'
#define K_F_BUF_GO_0 '0'

#define K_CHNG_COL 'y'
#define K_CHNG_COL_MODE 'Y'
#define K_REDRAW C_r

#define K_SW_ALT '!' 
#define K_DEAD_TAB '\t'

#define K_SW_ROW Grave 
#define K_SW_SRCH '@'
#define K_SW_INDENT C_g

#define K_GL_REPL C_t
#define K_GL_REN C_y

#define K_INSP_KEY C_p
/*
#define K_CUR_DN 'V'
*/
#define K_MV_UP 'k'
#define K_MV_UP_SCR 'K'
#define K_MV_UP_START '>'
#define K_MV_DN 'j'
#define K_MV_DN_SCR 'J'
#define K_MV_DN_END '<'
#define K_MV_R 'f'
#define K_MV_R_END 'g'
#define K_MV_R_N 'I'
#define K_MV_BOW 'i'
#define K_MV_WORDBACK 'u'
//#define K_MV_BOW C_v
#define K_MV_L 'd'
#define K_MV_L_BEG 's'
#define K_MV_L_N 'U'
#define K_DELLINE 't'
#define K_MKLINE 'O'
#define K_DEL 'r'
#define K_DEL_WORD 'e'
#define K_DEL_ENDLINE C_e 
#define K_CHANGE_WORD 'o'
#define K_CASE_WORD 'X'// unused
#define K_CASE_WORD_BACKW C_u
#define K_CASE_CHAR C_x
#define K_EDIT 'l'
#define K_EDIT_NL ';'

#define K_EDIT_BEG 'D'
#define K_EDIT_END 'F'

#define K_INDENT C_o
#define K_SPACE ' '
#define K_INS_CHAR 'p'
#define K_CHANGE_CHAR 'w'
#define K_FIND '/'
#define K_FIND_NAME '?'
#define K_REPLACE 'S'
#define K_REPEAT_RPL C_s
#define K_RENAME 'W'
#define K_REPEAT_REN C_w
#define K_REFIND 'n'
#define K_REFIND_NAME 'N'
#define K_WRITE_BUF '\''
#define K_WRITE_ALL '\"'
#define K_QUIT 'Q' 
#define K_SELECT 'h'
#define K_SET_MARK 'm'
#define K_GO_MARK_NUMB 'c'// unused
#define K_COPY 'a'
#define K_PASTE 'z'
#define K_CONCAT 'C'
#define K_DEL_BLOCK ']'
#define K_GOTO_LINE  'P'
#define K_SHOW_BUFF C_b
#define K_MV_UP_BLOCK C_d
#define K_MV_DN_BLOCK C_f
#define K_UNDO C_l
#define K_READ_SEQ C_v  
#define K_SEND_SEQ 'v' 

#define K_ENFRAME 'N' 
#define K_READ_ENFR_STR C_n