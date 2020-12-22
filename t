program example
	defvar a
	assign a 10
loop:
	print a
	decr a
	jumpnz loop a
	com show_files
	waitkey