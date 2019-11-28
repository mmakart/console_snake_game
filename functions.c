#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include <ncurses.h>

/*#define DEBUG*/
#define BEGIN_SIZE 4

void printSnake (CELLQUEUEPTR currentPtr)
{
    while (currentPtr != NULL) {
	printw ("(%d|%d) --> ", currentPtr->cell.x, currentPtr->cell.y);
	currentPtr = currentPtr->nextPtr;
    }
    printw ("NULL\n");
}

void initBoard (char ***board, int width, int height, char whiteSpace,
	        CELLQUEUEPTR snakeHeadPtr, FoodCell *food)
{
    allocateBoard (board, width, height);

    setEmptyBoard (board, width, height, whiteSpace, BORDER_CHAR);
    setSnake (board, snakeHeadPtr);
    *food = generateFood (*board, width, height);
    setFood (*board, *food);
}

void updateBoard (char ***board, int width, int height, char whiteSpace,
		  char border, CELLQUEUEPTR snakeHeadPtr, FoodCell food)
{
#ifdef DEBUG
    printf ("width=%d\theight=%d\n", width, height);
    printf ("food.x=%d\tfood.y=%d\n", food.x, food.y);
#endif
    setEmptyBoard (board, width, height, whiteSpace, border);
    setSnake (board, snakeHeadPtr);
    setFood (*board, food);
}

void allocateBoard (char ***board, int width, int height)
{
    *board = malloc (height * sizeof (char *));

    for (int i = 0; i < height; i++)
	(*board)[i] = malloc (width * sizeof (char));
}

void freeBoard (char ***board, int width, int height)
{
    for (int i = 0; i < height; i++)
	free ((*board)[i]);
    free (*board);
}

void setEmptyBoard (char ***board, int width, int height, char space,
		    char border)
{
    for (int i = 0; i < height; i++)
	for (int j = 0; j < width; j++)
	    (*board)[i][j] = space;

    // Задание границ
    setBorder (*board, width, height, border);
}

void setBorder (char **board, int width, int height, char c)
{
    for (int i = 0; i < height; i++) {
	board[i][width - 1] = c;
	board[i][0] = c;
    }
    for (int i = 0; i < width; i++) {
	board[0][i] = c;
	board[height - 1][i] = c;
    }
}

void printBoard(char **board, int width, int height)
{
    for (int i = 0; i < height; i++) {
	for (int j = 0; j < width; j++)
	    printw ("%c", board[i][j]);
	printw ("\n");
    }
}

void initSnake (CELLQUEUEPTR *headPtr, CELLQUEUEPTR *tailPtr)
{
    SnakeCell primaryCells[BEGIN_SIZE] = {
	{ 5, 10, BODY_CHAR },
	{ 6, 10, BODY_CHAR },
	{ 7, 10, BODY_CHAR },
	{ 8, 10, HEAD_CHAR } };

    // Добавляем ячейки в змею
    for (int i = 0; i < BEGIN_SIZE; i++)
	enqueueCell (headPtr, tailPtr, primaryCells[i]);
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

void setSnake (char ***board, CELLQUEUEPTR currentPtr)
{
    while (currentPtr != NULL) {
	(*board)[currentPtr->cell.y][currentPtr->cell.x] = currentPtr->cell.c;
	currentPtr = currentPtr->nextPtr;
    }

}


// Генерирует координаты для еды, не совпадающие с координатами границ
// и змеи
FoodCell generateFood (char **board, int width, int height)
{
    FoodCell food;
    int x, y;

    while (1) {

	x = 1 + rand() % (width - 2);
	y = 1 + rand() % (height - 2);

	if (board[y][x] == BORDER_CHAR || 
		board[y][x] == BODY_CHAR ||
		board[y][x] == HEAD_CHAR)
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

void setFood (char **board, FoodCell food)
{
    board[food.y][food.x] = FOOD_CHAR;
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
    }

    return direction;
}

int moveSnake (CELLQUEUEPTR *headPtr, CELLQUEUEPTR *tailPtr,
	       SnakeDirection direction, SnakeDirection previousDirection,
	       FoodCell *food, char **board, int width, int height, int *score)
{
    SnakeCell tempCell;

    tempCell.x = (*tailPtr)->cell.x;
    tempCell.y = (*tailPtr)->cell.y;
    tempCell.c = (*tailPtr)->cell.c;

    // Предыдущую "голову" назначаем "туловищем"
    if (direction == TO_RIGHT && previousDirection == TO_TOP
	    || direction == TO_BOTTOM && previousDirection == TO_LEFT
	    || direction == TO_LEFT && previousDirection == TO_BOTTOM
	    || direction == TO_TOP && previousDirection == TO_RIGHT)
	(*tailPtr)->cell.c = LB_RT;
    else if (direction == TO_RIGHT && previousDirection == TO_BOTTOM
	    || direction == TO_BOTTOM && previousDirection == TO_RIGHT
	    || direction == TO_LEFT && previousDirection == TO_TOP
	    || direction == TO_TOP && previousDirection == TO_LEFT)
	(*tailPtr)->cell.c = LT_RB;
    else
	(*tailPtr)->cell.c = BODY_CHAR;

    // Назначаем "голове" нужные координаты
    if (direction == TO_TOP)
	tempCell.y -= 1;
    else if (direction == TO_RIGHT)
	tempCell.x += 1;
    else if (direction == TO_LEFT)
	tempCell.x -= 1;
    else if (direction == TO_BOTTOM)
	tempCell.y += 1;
    else
	tempCell.x += 1;

    // Проверка на столкновение
    if (isCollision (board, tempCell, width, height))
	return 1;

    // Добавляем "голову" в конец очереди
    enqueueCell (headPtr, tailPtr, tempCell);

    // Если наткнулись на еду
    if (board[tempCell.y][tempCell.x] == FOOD_CHAR) {
	(*food) = generateFood (board, width, height);
	(*score)++;
    }
    else
	// Убираем хвост змеи
	dequeueCell (headPtr, tailPtr);

    return 0;

}

int isCollision (char **board, SnakeCell cell, int width, int height)
{
#ifdef DEBUG
    printf ("cell.x=%d\tcell.y=%d\nboard[%d][%d]='%c'",
	    cell.x, cell.y, cell.y, cell.x, board[cell.y][cell.x]);
#endif
    return (cell.x < 0 || cell.x >= width || cell.y < 0 || cell.y >= height
	    || board[cell.y][cell.x] == BORDER_CHAR
	    /*|| board[cell.y][cell.x] == LB_RT
	    || board[cell.y][cell.x] == LT_RB*/
	    || board[cell.y][cell.x] == BODY_CHAR);
}
