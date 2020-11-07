/*-
 * Copyright (c) 1994, 1995, 1999
 *      Simon Burge.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <curses.h>

#include "zombies.h"

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
	int	i, x, y, inwall;
	COORD	*cp;

	inwall = 0;
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
