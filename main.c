/* $Header: /cvsroot/games/zombies/main.c,v 1.2 1999/06/22 13:15:01 simonb Exp $ */

/*
 * main.c
 */

#include <curses.h>
#include <signal.h>
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
int	Pause = 0;		/* pause between levels, instead of waiting
				   for a keypress */
int	MyUid;			/* the user id of the player for scoring */

char	Field[Y_FIELDSIZE][X_FIELDSIZE];


int
main(int argc, char **argv)
{
	int	show_only;

	MyUid = getuid();
	show_only = FALSE;
	if (argc > 1) {
		/* any more options and I'll switch to getopt() */
		if (argc > 2)
			usage();
		else if (strcmp(argv[1], "-s") == 0)
			show_only = TRUE;
		else if (strcmp(argv[1], "-p") == 0)
			Pause = 1;
		else
			usage();
	}

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
