/* $Header$ */

#include "zombies.h"

/*
 * flush_in:
 *	Flush all pending input.
 */
flush_in()
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
query(prompt)
char	*prompt;
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
	return retval;
}



/*
 * rnd_pos:
 *	Pick a random, unoccupied position
 */
COORD *
rnd_pos()
{
	static COORD	pos;
	int	i = 0;

	do {
		pos.y = rnd(Y_FIELDSIZE - 1) + 1;
		pos.x = rnd(X_FIELDSIZE - 1) + 1;
		refresh();
	} while (Field[pos.y][pos.x] != BACKGR);
	return(&pos);
}

rnd(range)
int	range;
{
	unsigned int	rand();

	return(rand() % range);
}
