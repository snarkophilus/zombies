/* $Header: /cvsroot/games/zombies/move.c,v 1.2 1999/06/22 13:15:02 simonb Exp $ */

/*
 * move.c
 */

#include <curses.h>
#include <unistd.h>

#include "zombies.h"

/*
 * get_move:
 *	Get and execute a move from the player
 */

void
get_move(void)
{
	int	c;

	for (;;) {
		c = getchar();
		switch (c) {
		  case ' ':
		  case '.':
		  case '5':
			if (do_move(0, 0))
				return;
			break;
		  case 'y':
		  case '7':
			if (do_move(-1, -1))
				return;
			break;
		  case 'k':
		  case '8':
			if (do_move(-1, 0))
				return;
			break;
		  case 'u':
		  case '9':
			if (do_move(-1, 1))
				return;
			break;
		  case 'h':
		  case '4':
			if (do_move(0, -1))
				return;
			break;
		  case 'l':
		  case '6':
			if (do_move(0, 1))
				return;
			break;
		  case 'b':
		  case '1':
			if (do_move(1, -1))
				return;
			break;
		  case 'j':
		  case '2':
			if (do_move(1, 0))
				return;
			break;
		  case 'n':
		  case '3':
			if (do_move(1, 1))
				return;
			break;
		  case 'q':
		  case 'Q':
			if (query("Really ?"))
				quit(0);
			refresh();
			break;
		  case CTRL('L'):
		  case CTRL('R'):
			wrefresh(curscr);
			break;
		  default:
			BEEP();
			fflush(stdout);
			break;
		}
	}
}

/*
 * do_move:
 *	Execute a move
 */

int
do_move(int dy, int dx)
{
	static COORD	newpos;

	newpos.y = Mypos.y + dy;
	newpos.x = Mypos.x + dx;
	if (newpos.y <= 0 || newpos.y >= Y_FIELDSIZE ||
	    newpos.x <= 0 || newpos.x >= X_FIELDSIZE) {
		BEEP();
		return FALSE;
	}
	else if (dy == 0 && dx == 0)
		return TRUE;
	switch (Field[newpos.y][newpos.x]) {
		case BACKGR:
			MVAddCh(Mypos.y, Mypos.x, BACKGR);
			Mypos = newpos;
			MVAddCh(Mypos.y, Mypos.x, PLAYER);
			refresh();
			return TRUE;
			break;
		case WALL:
			BEEP();
			return FALSE;
			break;
		case ZOMBIE:
			Dead = 1;
			MVAddCh(Mypos.y, Mypos.x, BACKGR);
			MVAddCh(newpos.y, newpos.x, DEAD);
			return TRUE;
			break;
		default:
			/* NOTREACHED */;
	}
	return(0);	/* keep gcc -Wall happy */
}


/*
 * move_zombies:
 *	Move the zombies around
 */

void
move_zombies(void)
{
	COORD	*zp;

	for (zp = Zombies; zp < &Zombies[MAX_ZOMBIES]; zp++) {
		if (zp->y < 0)
			continue;
		MVAddCh(zp->y, zp->x, BACKGR);
		zp->y += Xsign(Mypos.y - zp->y);
		zp->x += Xsign(Mypos.x - zp->x);
		if (zp->y <= 0)
			zp->y = 0;
		else if (zp->y >= Y_FIELDSIZE)
			zp->y = Y_FIELDSIZE - 1;
		if (zp->x <= 0)
			zp->x = 0;
		else if (zp->x >= X_FIELDSIZE)
			zp->x = X_FIELDSIZE - 1;
		switch (Field[zp->y][zp->x]) {
			case ZOMBIE:
				zp->y = -1; /* Killed himself! */
				Alive_zombies--;
				add_to_score(1);
				break;
			case PLAYER:
				Dead = TRUE; /* Killed you! */
				MVAddCh(zp->y, zp->x, DEAD);
				break;
			case WALL:
				kill_wall(zp->y, zp->x);
				Alive_zombies--;
				add_to_score(1);
				zp->y = -1; /* Killed himself! */
				break;
			case BACKGR:
				MVAddCh(zp->y, zp->x, ZOMBIE);
				break;
			default:
				/* NOTREACHED */;
		}
	}
}

/*
 * kill_wall:
 *	A zombie broke a wall
 */

void
kill_wall(int y, int x)
{
	x = (x / (WALLSIZE + X_WALLGAP)) * (WALLSIZE + X_WALLGAP) + X_WALLGAP;
	while (Field[y][x] == WALL) {
		MVAddCh(y, x, BACKGR);
		x++;
	}
	WallsLeft--;
}

/*
 * add_to_score:
 *	Add a score to the overall point total
 */

void
add_to_score(int add)
{
	Score += add;
	move(Y_SCORE, X_SCORE);
	printw("%d", Score);
}

/*
 * Xsign:
 *	Return the almost sign of the number
 */

int
Xsign(int n)
{
	if (n < 0)
		return -1;
	else
		return 1;
}
