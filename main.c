#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include "functions.h"
#include "kbhit.h"

int main(int argc, char **argv)
{
    char **board;
    int height = 20, width = 20;
    SnakeDirection snakeDirection = TO_RIGHT;
    SnakeDirection prevDirection;
    char command[20];
    FoodCell foodCell;
    int collisionOccured = 0;
    int timePeriod;
    int score = 0;

    srand (time(NULL));

    if (argc > 1)
	timePeriod = atoi (argv [1]);
    else
	timePeriod = 400;

    // Очередь из ячеек змейки
    CELLQUEUEPTR snakeHeadPtr = NULL, snakeTailPtr = NULL;

    initscr ();

    initSnake (&snakeHeadPtr, &snakeTailPtr);
    initBoard (&board, width, height, WHITE_SPACE, snakeHeadPtr, &foodCell);
    printBoard (board, width, height);

    refresh ();

    while (1) {

	napms (timePeriod);

	prevDirection = snakeDirection;

	// 1 - direction identification
	if (kbhit ()) {
	    snakeDirection = identificateDirection (getch ());
	}

	// 2 - transformations over the snake
	collisionOccured = moveSnake (&snakeHeadPtr, &snakeTailPtr, snakeDirection,
		prevDirection, &foodCell, board, width, height, &score);

	// 3 - check for collision
	if (collisionOccured) {
	    printw ("You lost!\n");
	    refresh ();
	    getch ();
	    goto END;
	}

	// 4 - redrawing the board
	updateBoard (&board, width, height, WHITE_SPACE,
		BORDER_CHAR, snakeHeadPtr, foodCell);

	// 5 - printing the board
	move (0, 0);
	printBoard (board, width, height);
	printw ("score: %d\n", score);
#ifdef DEBUG
	printSnake (snakeHeadPtr);
#endif

	refresh ();
    }

END:
    freeBoard (&board, width, height);

    endwin ();

    return 0;
}
