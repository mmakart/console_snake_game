#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include "functions.h"
#include "kbhit.h"

int main(int argc, char **argv)
{
    Board board;
    SnakeDirection prevDirection;
    char command[20];
    FoodCell foodCell;
    int collisionOccured = 0;
    int timePeriod;
    int score = 0;
    int transparentBorder = 0;
    int isRepeating = 0;
    int boardWidth = 20, boardHeight = 10;

    srand (time(NULL));

    if (argc > 1)
	timePeriod = atoi (argv [1]);
    else
	timePeriod = 400;

    if (argc >= 3) {
	transparentBorder = !(strcmp (argv[2], "--transparent"));
    }

    isRepeating = transparentBorder;

    Snake snake;

    initscr ();

    initBoard (&board, boardWidth, boardHeight, &snake, &foodCell);


    printBoard (board, isRepeating);

    refresh ();

    while (1) {

	napms (timePeriod);

	prevDirection = snake.direction;

	// 1 - direction identification
	if (kbhit ()) {
	    snake.direction = identificateDirection (getch ());
	}

	// 2 - transformations over the snake
	collisionOccured = moveSnake (
		&snake, prevDirection, &foodCell, &board, &score, transparentBorder);

	// 3 - check for collision
	if (collisionOccured) {
	    printw ("You lost!\n");
	    refresh ();
	    getch ();
	    goto END;
	}

	// 4 - redrawing the board
	updateBoard (&board, snake, foodCell);

	// 5 - printing the board
	move (0, 0);
	printBoard (board, isRepeating);
	printw ("score: %d\n", score);
#ifdef DEBUG
	printSnake (snake);
#endif

	refresh ();
    }

END:
    freeBoard (&board);

    endwin ();

    return 0;
}
