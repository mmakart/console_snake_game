#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include "functions.h"
#include "kbhit.h"

int main(int argc, char **argv)
{
    char **board;
    int height = 20, width = 20;
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

    Snake snake;
    snake.headPtr = NULL;
    snake.tailPtr = NULL;
    snake.direction = TO_RIGHT;

    initscr ();

    initSnake (&snake);
    initBoard (&board, width, height, WHITE_SPACE, snake, &foodCell);
    printBoard (board, width, height);

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
		&snake, prevDirection, &foodCell,
		board, width, height, &score);

	// 3 - check for collision
	if (collisionOccured) {
	    printw ("You lost!\n");
	    refresh ();
	    getch ();
	    goto END;
	}

	// 4 - redrawing the board
	updateBoard (&board, width, height, WHITE_SPACE,
		BORDER_CHAR, snake, foodCell);

	// 5 - printing the board
	move (0, 0);
	printBoard (board, width, height);
	printw ("score: %d\n", score);
#ifdef DEBUG
	printSnake (snake);
#endif

	refresh ();
    }

END:
    freeBoard (&board, width, height);

    endwin ();

    return 0;
}
