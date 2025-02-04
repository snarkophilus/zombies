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

#ifndef SCORE_FILE
#define SCORE_FILE "/usr/local/share/games/zombies_score"
#endif

#ifndef SIGTYPE
#define SIGTYPE void
#endif

/*
 * miscellaneous constants
 */

#define	Y_FIELDSIZE	22
#define	X_FIELDSIZE	64
#define	Y_SIZE		24
#define	X_SIZE		80

#define WALLSIZE	5
#define X_WALLGAP	2
#define Y_WALLGAP	1

#define START_ZOMBIES	20
#define ZOMBIE_INCR	5
#define MAX_ZOMBIES	100

#define	Y_HIGH		17
#define	X_HIGH		(X_FIELDSIZE + 10)
#define	Y_BONUS		22
#define	X_BONUS		(X_FIELDSIZE + 3)
#define	Y_SCORE		20
#define	X_SCORE		(X_FIELDSIZE + 10)
#define	Y_LEVEL		19
#define	X_LEVEL		(X_FIELDSIZE + 10)
#define	Y_DEAD		18
#define	X_DEAD		(X_FIELDSIZE + 3)
#define	Y_PROMPT	(Y_FIELDSIZE - 1)
#define	X_PROMPT	(X_FIELDSIZE + 3)

#define	MAX_SCORES	1000
#define DISPLAY_SCORES	15
#define USER_SCORES	10
#define	MAXNAME		16

/*
 * characters on screen
 */

#define	PLAYER		'@'
#define	ZOMBIE		'+'
#define	WALL		'#'
#define BACKGR		'.'
#define DEAD		'$'

/*
 * pseudo functions
 */

#ifdef	CTRL
#undef	CTRL
#endif
#define	CTRL(X)	(X & 0x1f)

#ifdef	min
#undef	min
#endif
#define	min(a, b)		((a) < (b) ? (a) : (b))

#define	MVAddCh(y, x, ch)	(mvaddch(y, x, ch), Field[y][x] = ch)
#define BEEP()			{write(2, "\007", 1); fflush(stderr);}

/*
 * type definitions
 */

typedef struct {
	int	y, x;
} COORD;

extern	COORD	Mypos;
extern	COORD	Zombies[];
extern	int	Num_zombies;
extern	int	Alive_zombies;
extern	int	Dead;
extern	int	Score;
extern	int	WallsLeft;
extern	int	Level;
extern	int	Pause;
extern	int	MyUid;
extern	int	StartTime, EndTime;

extern	char	Field[Y_FIELDSIZE][X_FIELDSIZE];

/* -- level.c -- */
void play_level(void);
void make_level(void);

/* -- main.c -- */
int main(int argc, char **argv);
void init_field(void);
SIGTYPE quit(int ignored);
int another(void);
void usage(void);

/* -- misc.c -- */
void flush_in(void);
int query(char *prompt);
COORD *rnd_pos(void);

/* -- move.c -- */
void get_move(void);
int do_move(int dy, int dx);
void move_zombies(void);
void kill_wall(int y, int x);
void add_to_score(int add);
int Xsign(int n);

/* -- scorec. -- */
void score(void);
int add_score(void);
void show_score(void);
void read_scores(void);
void write_scores(void);
int top_score(void);
void give_bonus(void);
