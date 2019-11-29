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
    char **symbols;
    int width;
    int height;
} Board;

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

void allocateBoard (Board *board);
void initBoard (Board *board, int initWidth, int initHeight, Snake snake, FoodCell *food);
void freeBoard (Board *board);
void setEmptyBoard (Board *board);
void setBorder (Board *board);
void printBoard(Board board);
void initSnake (Snake *snake);
void setSnake (Board *board, Snake snake);
void printSnake (Snake snake);
FoodCell generateFood (Board board);
void setFood (Board *board, FoodCell food);

SnakeDirection identificateDirection (int command);
int isCollision (Board *board, SnakeCell cell);
void updateBoard (Board *board, Snake snake, FoodCell food);
int moveSnake (Snake *snake, SnakeDirection previousDirection, FoodCell *food, Board *board, int *score);
#endif
