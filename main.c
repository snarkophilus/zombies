/* $Header: /cvsroot/games/zombies/main.c,v 1.5 1999/06/22 14:34:53 simonb Exp $ */

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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "zombies.h"

/* ----- global variables ----- */

COORD	Mypos;			/* current player position */
COORD	Zombies[MAX_ZOMBIES];	/* zombies positions */

int	Num_zombies;		/* total zombies to start level with */
int	Alive_zombies;		/* number of zombies left alive */
int	Level;			/* current level */
int	Dead;			/* am I dead? */
int	Score;			/* current score */
int	WallsLeft;		/* number of walls left on this level */
int	Pause = FALSE;		/* pause between levels, instead of waiting
				   for a keypress */
int	MyUid;			/* the user id of the player for scoring */

char	Field[Y_FIELDSIZE][X_FIELDSIZE];


int
main(int argc, char **argv)
{
	int	c, show_only;

	MyUid = getuid();
	show_only = FALSE;
	while ((c = getopt(argc, argv, "ps")) != EOF)
		switch (c) {
		  case 'p':
			Pause = TRUE;
			break;
		  case 's':
			show_only = TRUE;
			break;
		  case '?':
			usage();
			/* NOTREACHED */
		}
	if (optind < argc)
		usage();
		/* NOTREACHED */

	if (show_only) {
		show_score();
		exit(0);
		/* NOTREACHED */
	}

	initscr();
	crmode();
	noecho();
	nonl();
	signal(SIGINT, quit);
	if (LINES != Y_SIZE || COLS != X_SIZE) {
		if (LINES < Y_SIZE || COLS < X_SIZE) {
			endwin();
			printf("Need at least a %dx%d screen\n",
			    Y_SIZE, X_SIZE);
			exit(1);
		}
		delwin(stdscr);
		stdscr = newwin(Y_SIZE, X_SIZE, 0, 0);
	}

	srand(getpid() * time(0));	/* as good as any... */

	/* show time! */
	do {
		init_field();
		StartTime = time(NULL);
		for (Level = 1; !Dead; Level++) {
			make_level();
			play_level();
			if (!Dead)
				give_bonus();
		}
		move(Y_DEAD, X_DEAD);
		printw("You're Dead!");
		refresh();
		score();
	} while (another());
	quit(0);
	exit(0);	/* keep gcc -Wall happy */
}


/*
 * init_field:
 *	Lay down the initial pattern whih is constant across all levels,
 *	and initialize all the global variables.
 */

void
init_field(void)
{
	int	i;
	static int	first = TRUE;
	static char	*desc[] = {
				"Directions:",
				"",
				"y k u",
				" \\|/",
				"h- -l",
				" /|\\",
				"b j n",
				"",
				"Commands:",
				"q:  quit",
				"^L: redraw",
				"",
				"Legend:",
				"+:  zombie",
				"#:  wall",
				"@:  you",
				"",
				"High:  0",
				"",
				"Level: 0",
				"Score: 0",
				NULL
	};

	Dead = FALSE;
#ifdef flushok
	flushok(stdscr, TRUE);
#endif
	Score = 0;
	Num_zombies = START_ZOMBIES;

	erase();
	move(0, 0);
	addch('+');
	for (i = 1; i < Y_FIELDSIZE; i++) {
		move(i, 0);
		addch('|');
	}
	move(Y_FIELDSIZE, 0);
	addch('+');
	for (i = 1; i < X_FIELDSIZE; i++)
		addch('-');
	addch('+');
	if (first)
		refresh();
	move(0, 1);
	for (i = 1; i < X_FIELDSIZE; i++)
		addch('-');
	addch('+');
	for (i = 1; i < Y_FIELDSIZE; i++) {
		move(i, X_FIELDSIZE);
		addch('|');
	}
	if (first)
		refresh();
	for (i = 0; desc[i] != NULL; i++) {
		move(i, X_FIELDSIZE + 3);
		addstr(desc[i]);
	}
	move(Y_HIGH, X_HIGH);
	printw("%d", top_score());
	if (first)
		refresh();
	first = FALSE;
}


/*
 * quit:
 *	Leave the program elegantly.
 */
SIGTYPE
quit(int ignored)
{
#ifndef	_putchar
	extern	int _putchar();
#endif
	extern	char *CE;

	mvcur(0, COLS - 1, LINES - 1, 0);
#ifndef SYSV
	if (CE) {	/* try to clear to end of line */
		_puts(CE);
		endwin();
	}
	else {		/* otherwise move done a line */
#endif
		endwin();
		putchar('\n');
#ifndef SYSV
	}
#endif
	exit(0);
	/* NOTREACHED */
}


/*
 * another:
 *	See if another game is desired
 */

int
another(void)
{
	if (query("Play again?"))
		return TRUE;
	else
		return FALSE;
}


void
usage(void)
{
	fprintf(stderr, "usage: zombies [-p] [-s]\n");
	exit(1);
	/* NOTREACHED */
}
