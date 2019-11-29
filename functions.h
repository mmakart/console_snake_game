#ifndef FUNCTIONS_H
# define FUNCTIONS_H

# define BORDER_CHAR '#'
# define HEAD_CHAR '@'
# define BODY_CHAR 'o'
# define FOOD_CHAR '*'
# define WHITE_SPACE ' '
# define LB_RT '/'
# define LT_RB '\\'

typedef struct {
    int x;
    int y;
    char c;
} SnakeCell;

enum snakeDirection { TO_TOP, TO_RIGHT, TO_BOTTOM, TO_LEFT };
typedef enum snakeDirection SnakeDirection;

struct cellQueue {
    SnakeCell cell;
    struct cellQueue *nextPtr;
};
typedef struct cellQueue CELLQUEUE;
typedef CELLQUEUE* CELLQUEUEPTR;

typedef struct {
    CELLQUEUEPTR headPtr;
    CELLQUEUEPTR tailPtr;
    SnakeDirection direction;
} Snake;

struct foodCell {
    int x;
    int y;
    char c;
};
typedef struct foodCell FoodCell;

void enqueueCell (CELLQUEUEPTR *headPtr, CELLQUEUEPTR *tailPtr,
	SnakeCell cell);
SnakeCell dequeueCell (CELLQUEUEPTR *headPtr, CELLQUEUEPTR *tailPtr);

void allocateBoard (char ***board, int width, int height);
void initBoard (char ***board, int width, int height, char whiteSpace, Snake snake, FoodCell *food);
void freeBoard (char ***board, int width, int height);
void setEmptyBoard (char ***board, int width, int height, char space, char border);
void setBorder (char **board, int width, int height, char c);
void printBoard(char **board, int width, int height);
void initSnake (Snake *snake);
void setSnake (char ***board, Snake snake);
void printSnake (Snake snake);
FoodCell generateFood (char **board, int width, int height);
void setFood (char **board, FoodCell food);

SnakeDirection identificateDirection (int command);
int isCollision (char **board, SnakeCell cell, int width, int height);
void updateBoard (char ***board, int width, int height, char whiteSpace, char border, Snake snake, FoodCell food);
int moveSnake (Snake *snake, SnakeDirection previousDirection, FoodCell *food, char **board, int width, int height, int *score);
#endif
