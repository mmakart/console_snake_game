#include <ncurses.h>

//Возвращает TRUE, если введён символ
//FALSE, если не введён
int kbhit()
{
	int ch, r;

	//Отключить вечное ожидание ввода
	nodelay(stdscr, TRUE);
	//Не показывать вводимый символ
	noecho();

	ch = getch();

	//Если ничего не нажималось
	if (ch == ERR)
		r = FALSE;
	else
	{
		r = TRUE;
		ungetch(ch);
	}       

	//Включаем показ введённых символов и вечное ожидание
	echo(); 
	nodelay(stdscr, FALSE);

	//Возвращаем TRUE или FALSE
	return r;
}
