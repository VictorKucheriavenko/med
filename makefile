med: med.c command.c split.c undo.c FORCE
	./autohead med.c > function.h

	gcc -Wimplicit-function-declaration -fmax-errors=1 -o med med.c command.c split.c undo.c
FORCE:
