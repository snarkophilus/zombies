/* $Header$ */

/*
 * zombies.h
 */

#include <curses.h>

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

/*
 * functions
 */

COORD	*rnd_pos();
int	top_score();
