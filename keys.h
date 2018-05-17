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
#define K_TEST '\026'

//#define K_F_BUF_CLOSE '\021'
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

#define K_CHNG_COL C_g
#define K_REDRAW C_r

#define K_KMOD Grave 
#define K_KDEAD '\t'

#define K_SW_ROW '!'
#define K_SW_SRCH '@'

#define K_GLOBAL C_t
#define K_GLOBAL2 C_y

#define K_CUR_UP C_p
#define K_CUR_DOWN C_n
#define K_M_UP 'k'
#define K_M_UP_SCR 'K'
#define K_M_UP_START C_k
#define K_M_DOWN 'j'
#define K_M_DOWN_SCR 'J'
#define K_M_DOWN_END C_h
#define K_M_RIGHT 'f'
#define K_M_RIGHT2 'g'
#define K_M_RIGHT1 'I'
#define K_GOWORD 'i'
#define K_GOWORD1 C_b
#define K_GOWORDBACK 'u'
#define K_M_LEFT 'd'
#define K_M_LEFT2 's'
#define K_M_LEFT1 'U'
#define K_DELLINE 't'
#define K_MKLINE 'O'
#define K_DEL 'r'
#define K_DEL_WORD 'e'
#define K_CHANGE_WORD 'o'
#define K_CASE_WORD 'X'
#define K_CASE_WORD_BACKW C_u
#define K_CASE_CHAR C_x
#define K_INS_MOD 'l'
#define K_INS_MOD1 ';'

#define K_INS_PREPEND 'D'
#define K_INS_APPEND 'F'

#define K_INS_INDENT C_o
#define K_INS_SPACE ' '
#define K_INS_CHAR 'p'
#define K_CHANGE_CHAR '['
#define K_FIND1 '/'
#define K_FIND2 '?'
#define K_SRCH_RPL 'S'
#define K_REPEAT_RPL C_s
#define K_SRCH_RPL2 'W'
#define K_REPEAT_RPL2 C_w
#define K_REFIND 'n'
#define K_REFIND2 'N'
#define K_WRITE_ALL '\"'
#define K_WRITE '\''
#define K_QUIT 'Q' 
#define K_SET_COPY_START 'h'
#define K_SET_MARK 'm'
#define K_GO_MARK_NUMB 'c'
#define K_COPY 'a'
#define K_PASTE 'z'
#define K_CONCAT 'C'
#define K_DEL_BLOCK 'w'
#define K_GO_LINE  'P'
#define K_SH_BUFF C_b
#define K_UP_BLOCK C_d
#define K_DOWN_BLOCK C_f
#define K_UNDO C_l
//#define K_ENTER 10
//#define K_EN				TER 10
//#define K_ENTER 10