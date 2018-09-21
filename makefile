med: med.c command.c split.c undo.c FORCE
	./autohead med.c > function.h

	gcc -Wall -Wno-unused-variable -Wno-return-type -Wno-unused-but-set-variable -Wno-char-subscripts -fmax-errors=1 -o med med.c command.c split.c undo.c
cp:
	sudo cp -f ./med /usr/bin/
FORCE:
