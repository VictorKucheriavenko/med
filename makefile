FLAGS = -Wall -Werror -Wno-unused-variable -Wno-return-type \
	-Wno-unused-but-set-variable -Wno-char-subscripts -fmax-errors=1\
	-Wno-unused-label
	
C_FILES = med.c init.c move_x.c command.c split.c undo.c script.c

H_FILES = function.h init.h move_x.h

med: $(C_FILES) $(H_FILES) FORCE
	gcc -g -o med $(FLAGS) $(C_FILES)
	
red: $(C_FILES) $(H_FILES) FORCE
	gcc -D READ -o red $(FLAGS) $(C_FILES)
	
all: $(C_FILES) $(H_FILES) FORCE
	gcc -o med $(FLAGS) $(C_FILES)
	gcc -D READ -o red $(FLAGS) $(C_FILES)
	
install: all
	sudo cp -f ./med /usr/bin/
	sudo cp -f ./red /usr/bin/
	
run: med
	./med file.c
	
function.h: med.c
	./autohead med.c > function.h
init.h:	init.c
	./autohead init.c > init.h
move_x.h: move_x.c
	./autohead move_x.c > move_x.h
edit.h: edit.c
	./autohead edit.c > edit.h

FORCE:


