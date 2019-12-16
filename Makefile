all:
	gcc -o snake -g functions.c main.c kbhit.c -lncurses
