/* $Header: /cvsroot/games/zombies/misc.c,v 1.2 1999/06/22 13:15:01 simonb Exp $ */

#include <curses.h>
#include <stdlib.h>

#include "zombies.h"

static int rnd(int range);

/*
 * flush_in:
 *	Flush all pending input.
 */

void
flush_in(void)
{
# ifdef TIOCFLUSH
	ioctl(fileno(stdin), TIOCFLUSH, NULL);
# else TIOCFLUSH
	crmode();
# endif TIOCFLUSH
}


/*
 * query:
 *	Ask a question and get a yes or no answer.  Default is "no".
 */

int
query(char *prompt)
{
	int	c, retval;
	int	y, x;

	getyx(stdscr, y, x);
	move(Y_PROMPT, X_PROMPT);
	addstr(prompt);
	clrtoeol();
	refresh();
	retval = ((c = getchar()) == 'y' || c == 'Y');
	move(Y_PROMPT, X_PROMPT);
	clrtoeol();
	move(y, x);
	return(retval);
}



/*
 * rnd_pos:
 *	Pick a random, unoccupied position
 */
COORD *
rnd_pos(void)
{
	static COORD	pos;

	do {
		pos.y = rnd(Y_FIELDSIZE - 1) + 1;
		pos.x = rnd(X_FIELDSIZE - 1) + 1;
		refresh();
	} while (Field[pos.y][pos.x] != BACKGR);
	return(&pos);
}

static int
rnd(int range)
{
	return(rand() % range);
}
