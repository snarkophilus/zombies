/* $Header: /cvsroot/games/zombies/level.c,v 1.2 1999/06/22 13:15:01 simonb Exp $ */

#include <curses.h>

#include "zombies.h"

/*
 * play_level.c
 */

void
play_level(void)
{
	flush_in();
	wrefresh(curscr);
	Alive_zombies = Num_zombies;
	while (!Dead && Alive_zombies > 0) {
		move(Mypos.y, Mypos.x);
		refresh();
		get_move();
		if (!Dead)
			move_zombies();
	}
}


/*
 * make_level:
 *	Make the current level
 */

void
make_level(void)
{
	int	i;
	COORD	*cp;
	int	x, y;
	int	inwall = 0;

	WallsLeft = 0;
	for (y = 1; y < Y_FIELDSIZE; y++) {
		for (x = 1; x < X_FIELDSIZE; x++)
			if ((x % (X_WALLGAP + WALLSIZE) < X_WALLGAP) ||
						(y % (Y_WALLGAP + 1))) {
				MVAddCh(y, x, BACKGR);
				inwall = 0;
			}
			else {
				MVAddCh(y, x, WALL);
				if (!inwall) {
					inwall = 1;
					WallsLeft++;
				}
			}
		inwall = 0;
	}

	leaveok(stdscr, FALSE);
	for (cp = Zombies; cp < &Zombies[MAX_ZOMBIES]; cp++)
		cp->y = -1;
	Mypos.x = X_FIELDSIZE / 2;
	Mypos.y = Y_FIELDSIZE / 2;
	if (Field[Mypos.y][Mypos.x] == WALL)
		Mypos.y++;
	MVAddCh(Mypos.y, Mypos.x, PLAYER);

	i = Num_zombies = min(Num_zombies + ZOMBIE_INCR, MAX_ZOMBIES);
	while (i-- > 0) {
		do
			cp = rnd_pos();
		while (Field[cp->y][cp->x] != BACKGR &&
				!((abs(Mypos.y - cp->y) < 3) &&
				(abs(Mypos.x - cp->x) < 3)));
		Zombies[i] = *cp;
		MVAddCh(cp->y, cp->x, ZOMBIE);
	}
	move(Y_LEVEL, X_LEVEL);
	printw("%d", Level);
	refresh();
}
