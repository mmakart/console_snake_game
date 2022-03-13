#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include <ncurses.h>

/*#define DEBUG*/
#define BEGIN_SIZE 2

void printSnake (Snake snake)
{
	CELLQUEUEPTR currentPtr = snake.headPtr;
	while (currentPtr != NULL) {
		printw ("(%d|%d) --> ", currentPtr->cell.x, currentPtr->cell.y);
		currentPtr = currentPtr->nextPtr;
	}
	printw ("NULL\n");
}

void initBoard (Board *board, int initWidth, int initHeight,
		Snake *snake, FoodCell *food, int isRepeating)
{
	board->width = initWidth;
	board->height = initHeight;
	allocateBoard (board);

	setEmptyBoard (board, isRepeating);

	initSnake (snake, initWidth, initHeight);
	setSnake (board, *snake);

	*food = generateFood (*board);
	setFood (board, *food);
}

void updateBoard (Board *board, Snake snake, FoodCell food, int isRepeating)
{
	setEmptyBoard (board, isRepeating);
	setSnake (board, snake);
	setFood (board, food);
}

void allocateBoard (Board *board)
{
	board->symbols = malloc (board->height * sizeof (char *));

	for (int i = 0; i < board->height; i++)
		board->symbols[i] = malloc (board->width * sizeof (char));
}

void freeBoard (Board *board)
{
	for (int i = 0; i < board->height; i++)
		free (board->symbols[i]);
	free (board->symbols);
}

void setEmptyBoard (Board *board, int isRepeating)
{
	for (int i = 0; i < board->height; i++)
		for (int j = 0; j < board->width; j++)
			board->symbols[i][j] = WHITE_SPACE;

	// Задание границ
	if (!isRepeating)
		setBorder (board);
}

void setBorder (Board *board)
{
	for (int i = 0; i < board->height; i++) {
		board->symbols[i][board->width - 1] = BORDER_CHAR;
		board->symbols[i][0] = BORDER_CHAR;
	}
	for (int i = 0; i < board->width; i++) {
		board->symbols[0][i] = BORDER_CHAR;
		board->symbols[board->height - 1][i] = BORDER_CHAR;
	}
}

void printBoard(Board board, int isRepeating)
{
	move (0, 0);

	if (isRepeating)
		printBoardUpperRepeatingPart (board);

	for (int i = 0; i < board.height; i++) {
		if (isRepeating)
			printBoardLeftRepeatingPart (board, i);

		for (int j = 0; j < board.width; j++) {
			printw ("%c", board.symbols[i][j]);
		}

		if (isRepeating)
			printBoardRightRepeatingPart (board, i);

		printw ("\n");
	}

	if (isRepeating)
		printBoardBottomRepeatingPart (board);
}

int isSnakeCell(int cell)
{
	return cell == HEAD_CHAR
		|| cell == HOR_BODY_CHAR
		|| cell == VERT_BODY_CHAR
		|| cell == LB_RT 
		|| cell == LT_RB;
}

void printBoardLeftRepeatingPart (Board board, int currentLine)
{
	for (int j = board.width / 2; j < board.width; j++)
		printw ("%c",
				(isSnakeCell(board.symbols[currentLine][j])
						? MIRROR_CHAR
						: WHITE_SPACE));
}

void printBoardRightRepeatingPart (Board board, int currentLine)
{
	for (int j = 0; j < board.width / 2; j++)
		printw ("%c",
				(isSnakeCell(board.symbols[currentLine][j])
						? MIRROR_CHAR
						: WHITE_SPACE));
}

void printBoardUpperRepeatingPart (Board board)
{
	int halfHeight = board.height / 2;

	for (int i = halfHeight; i < board.height; i++) {

		for (int j = board.width / 2; j < board.width; j++)
			printw ("%c",
					(isSnakeCell(board.symbols[i][j])
							? MIRROR_CHAR
							: WHITE_SPACE));

		for (int j = 0; j < board.width; j++)
			printw ("%c",
					(isSnakeCell(board.symbols[i][j])
							? MIRROR_CHAR
							: WHITE_SPACE));

		for (int j = 0; j < board.width / 2; j++)
			printw ("%c",
					(isSnakeCell(board.symbols[i][j])
							? MIRROR_CHAR
							: WHITE_SPACE));

		printw ("\n");
	}
}

void printBoardBottomRepeatingPart (Board board)
{
	int halfHeight = board.height / 2;

	for (int i = 0; i < halfHeight; i++) {

		for (int j = board.width / 2; j < board.width; j++)
			printw ("%c",
					(isSnakeCell(board.symbols[i][j])
							? MIRROR_CHAR
							: WHITE_SPACE));

		for (int j = 0; j < board.width; j++)
			printw ("%c",
					(isSnakeCell(board.symbols[i][j])
							? MIRROR_CHAR
							: WHITE_SPACE));

		for (int j = 0; j < board.width / 2; j++)
			printw ("%c",
					(isSnakeCell(board.symbols[i][j])
							? MIRROR_CHAR
							: WHITE_SPACE));

		printw ("\n");
	}
}

void initSnake (Snake *snake, int initWidth, int initHeight)
{
	int boardWidthCenter = initWidth / 2;
	int boardHeightCenter = initHeight / 2;
	SnakeCell primaryCells[BEGIN_SIZE] = {
		{ boardWidthCenter, boardHeightCenter, HOR_BODY_CHAR },
		{ boardWidthCenter + 1, boardHeightCenter, HEAD_CHAR } };

	snake->headPtr = NULL;
	snake->tailPtr = NULL;
	snake->direction = TO_RIGHT;

	// Добавляем ячейки в змею
	for (int i = 0; i < BEGIN_SIZE; i++)
		enqueueCell (&(snake->headPtr), &(snake->tailPtr), primaryCells[i]);
}

void enqueueCell (CELLQUEUEPTR *headPtr, CELLQUEUEPTR *tailPtr,
		SnakeCell value)
{
	CELLQUEUEPTR newPtr = malloc (sizeof (CELLQUEUE));

	if (newPtr == NULL) {
		printw ("Ошибка: недостаточно памяти.\n");
		return;
	}

	newPtr->cell = value;
	newPtr->nextPtr = NULL;

	if (*headPtr == NULL)
		*headPtr = newPtr;
	else
		(*tailPtr)->nextPtr = newPtr;

	*tailPtr = newPtr;
}

SnakeCell dequeueCell (CELLQUEUEPTR *headPtr, CELLQUEUEPTR *tailPtr)
{
	CELLQUEUEPTR tempPtr = *headPtr;
	SnakeCell value = (*headPtr)->cell;

	*headPtr = (*headPtr)->nextPtr;

	// если не осталось элементов
	if (*headPtr == NULL)
		*tailPtr = NULL;

	free (tempPtr);
	return value;
}

void setSnake (Board *board, Snake snake)
{
	CELLQUEUEPTR currentPtr = snake.headPtr;
	while (currentPtr != NULL) {
		board->symbols[currentPtr->cell.y][currentPtr->cell.x] = currentPtr->cell.c;
		currentPtr = currentPtr->nextPtr;
	}

}


// Генерирует координаты для еды, не совпадающие с координатами границ
// и змеи
FoodCell generateFood (Board board)
{
	FoodCell food;
	int x, y;

	while (1) {

		x = 1 + rand() % (board.width - 2);
		y = 1 + rand() % (board.height - 2);

		if (board.symbols[y][x] != WHITE_SPACE)
			continue;
		else {
			food.x = x;
			food.y = y;
			break;
		}
	}
#ifdef DEBUG
	printf ("generated food\n"
			"x=%d\ty=%d\n", food.x, food.y);
#endif
	return food;
}

void setFood (Board *board, FoodCell food)
{
	board->symbols[food.y][food.x] = FOOD_CHAR;
}

SnakeDirection identificateDirection (int command)
{
	SnakeDirection direction;

	/*if (!strcmp (command, "w"))
	  direction = TO_TOP;
	  else if (!strcmp (command, "a"))
	  direction = TO_LEFT;
	  else if (!strcmp (command, "s"))
	  direction = TO_BOTTOM;
	  else if (!strcmp (command, "d"))
	  direction = TO_RIGHT;*/

	/*if (command == 'w')
	  direction = TO_TOP;
	  else if (command = 'a')
	  direction = TO_LEFT;
	  else if (command = 's')
	  direction = TO_BOTTOM;
	  else if (command = 'd')
	  direction = TO_RIGHT;*/

	switch (command) {
		case 'w':
			direction = TO_TOP;
			break;
		case 'a':
			direction = TO_LEFT;
			break;
		case 's':
			direction = TO_BOTTOM;
			break;
		case 'd':
			direction = TO_RIGHT;
			break;
		default:
			direction = TO_RIGHT;
			break;
	}

	return direction;
}

int moveSnake (Snake *snake, SnakeDirection previousDirection,
		FoodCell *food, Board *board, int *score, int transparentBorder)
{
	SnakeCell tempCell;

	tempCell.x = snake->tailPtr->cell.x;
	tempCell.y = snake->tailPtr->cell.y;
	tempCell.c = snake->tailPtr->cell.c;

	// Предыдущую "голову" назначаем "туловищем"
	if (snake->direction == TO_RIGHT && previousDirection == TO_TOP
			|| snake->direction == TO_BOTTOM && previousDirection == TO_LEFT
			|| snake->direction == TO_LEFT && previousDirection == TO_BOTTOM
			|| snake->direction == TO_TOP && previousDirection == TO_RIGHT)
		snake->tailPtr->cell.c = LB_RT;
	else if (snake->direction == TO_RIGHT && previousDirection == TO_BOTTOM
			|| snake->direction == TO_BOTTOM && previousDirection == TO_RIGHT
			|| snake->direction == TO_LEFT && previousDirection == TO_TOP
			|| snake->direction == TO_TOP && previousDirection == TO_LEFT)
		snake->tailPtr->cell.c = LT_RB;
	else if (snake->direction == TO_RIGHT && previousDirection == TO_RIGHT
			|| snake->direction == TO_LEFT && previousDirection == TO_LEFT)
		snake->tailPtr->cell.c = HOR_BODY_CHAR;
	else
		snake->tailPtr->cell.c = VERT_BODY_CHAR;

	// Назначаем "голове" нужные координаты
	if (snake->direction == TO_TOP) {
		// Появляемся с другой стороны
		if (tempCell.y == 0 && transparentBorder)
			tempCell.y = board->height - 1;
		else
			tempCell.y -= 1;
	}
	else if (snake->direction == TO_RIGHT) {
		// Появляемся с другой стороны
		if (tempCell.x == board->width - 1 && transparentBorder)
			tempCell.x = 0;
		else
			tempCell.x += 1;
	}
	else if (snake->direction == TO_LEFT) {
		// Появляемся с другой стороны
		if (tempCell.x == 0 && transparentBorder)
			tempCell.x = board->width - 1;
		else
			tempCell.x -= 1;
	}
	else if (snake->direction == TO_BOTTOM) {
		// Появляемся с другой стороны
		if (tempCell.y == board->height - 1 && transparentBorder)
			tempCell.y = 0;
		else	
			tempCell.y += 1;
	}
	else
		tempCell.x += 1;

	// Проверка на столкновение
	if (isCollision (board, tempCell))
		return 1;

	// Добавляем "голову" в конец очереди
	enqueueCell (&(snake->headPtr), &(snake->tailPtr), tempCell);

	// Если наткнулись на еду
	if (board->symbols[tempCell.y][tempCell.x] == FOOD_CHAR) {
		*food = generateFood (*board);
		(*score)++;
	}
	else
		// Убираем хвост змеи
		dequeueCell (&(snake->headPtr), &(snake->tailPtr));

	return 0;

}

int isCollision (Board *board, SnakeCell cell)
{
#ifdef DEBUG
	printf ("cell.x=%d\tcell.y=%d\nboard[%d][%d]='%c'",
			cell.x, cell.y, cell.y, cell.x, board[cell.y][cell.x]);
#endif
	return (
			cell.x < 0
			|| cell.x >= board->width
			|| cell.y < 0
			|| cell.y >= board->height
			|| board->symbols[cell.y][cell.x] == BORDER_CHAR
			|| board->symbols[cell.y][cell.x] == LB_RT
			|| board->symbols[cell.y][cell.x] == LT_RB
			|| board->symbols[cell.y][cell.x] == HOR_BODY_CHAR
			|| board->symbols[cell.y][cell.x] == VERT_BODY_CHAR);
}
