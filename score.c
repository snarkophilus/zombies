/*
 * score.c
 */

#include "zombies.h"

#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int	s_uid;
	int	s_score;
	int	s_level;
	int	s_starttime;
	int	s_endtime;
	char	s_name[MAXNAME];
	char	s_host[MAXNAME];
} SCORE;

int		StartTime, EndTime;
static SCORE	scores[MAX_SCORES];


/*
 * score:
 *	Post the player's score, if reasonable, and then print out the
 *	top list.
 */
score()
{
	SCORE	*s;
	char	lockfile[BUFSIZ];
	int	done_show, my_score, i, lfd;
	int	newscore, top, bottom, topline;

	strcpy(lockfile, SCORE_FILE);
	strcat(lockfile, ".lock");

	/* Lock the score file */
	while (((lfd = open(lockfile, O_CREAT | O_EXCL, 0644)) < 0) &&
	    errno == EEXIST)
		sleep(1);
	if (lfd < 0)
		return;

	/*
	 * Read in the score file.  If the score is not good enough, then
	 * return, otherwise write out the revised score file.
	 */
	read_scores();
	newscore = add_score(Score);
	if (!newscore) {
		unlink(lockfile);
		return;
	}
	write_scores();
	unlink(lockfile);

	/* Show the user how good he is! */
	move(1, 10);
	printw(" Pos\tScore\tLevel\tHost\t\tName\t");
	move(2, 10);
	printw(" ---\t-----\t-----\t----------\t----\t");
	done_show = FALSE;
	if ((top = newscore - (DISPLAY_SCORES / 2)) < 1)
		top = 1;
	if ((bottom = top + DISPLAY_SCORES - 1) > MAX_SCORES)
		bottom = MAX_SCORES;
	topline = 3 - top;
	for (i = top, s = &scores[top - 1]; i <= bottom; i++, s++) {
		if (s->s_score <= 0)
			break;
		move(i + topline, 10);
		if (!done_show && s->s_uid == MyUid && s->s_score == Score)
			standout();
		printw(" %3d\t%5d\t%3d\t%-15s\t%-8.8s ", i, s->s_score,
		    s->s_level, s->s_host, s->s_name);
		if (!done_show && s->s_uid == MyUid && s->s_score == Score) {
			standend();
			done_show = TRUE;
		}
	}
}


add_score()
{
	int	i, newscore, uidcount;
	SCORE	*s, ns, os, temp;

	if (scores[MAX_SCORES - 1].s_score > Score) {
		/* Score not good enough... */
		return(0);
	}

	/* Initialise the new score. */
	ns.s_score = Score;
	ns.s_level = Level - 1;
	ns.s_uid = MyUid;
	ns.s_starttime = StartTime;
	ns.s_endtime = time(0);
	set_name(&ns);

	os.s_score = -1;
	uidcount = newscore = 0;
	for (i = 0, s = scores; i < MAX_SCORES; i++) {
		temp = *s;
		if (s->s_score <= Score && !newscore &&
		    uidcount < USER_SCORES) {
			/*
			 * If our score is better than the current
			 * score, and we haven't added our score yet,
			 * put it in the table.
			 */
			*s = ns;
			newscore = i + 1;
			uidcount++;
			s++;
			/* Save current score. */
			os = temp;
		}
		else if (os.s_score > 0 &&
		    (os.s_uid != MyUid || uidcount < USER_SCORES)) {
			/*
			 * If we have an old score saved, put it
			 * in the table, and keep the current one
			 * for the next loop.  And bump up the uid
			 * count in necessary...
			 */
			if (os.s_uid == MyUid)
				uidcount++;
			*s++ = os;
			os = temp;
		}
		else if (os.s_score == -1) {
			/*
			 * If we haven't inserted our score yet, just
			 * move along after checking the uid count.
			 */
			if (s->s_uid == MyUid)
				uidcount++;
			s++;
		}
	}
	return(newscore);
}


set_name(s)
SCORE	*s;
{
	struct passwd	*pp;
	char	*p;

	if ((pp = getpwuid(s->s_uid)) == NULL)
		pp->pw_name = "???";
	strncpy(s->s_name, pp->pw_name, MAXNAME);
	gethostname(s->s_host, MAXNAME);
	if (p = strchr(s->s_host, '.'))
		p = '\0';
}


/*
 * show_score:
 *	Show the score list for the '-s' option.
 *
 * also used to return top score for init_field();
 */
show_score()
{
	SCORE	*s;
	int	i, inf;

	read_scores();

	inf = 1;
	printf("\n");
	printf("\t%s\t%s\t%s\t%s\t\t%s\n",
	    "Pos", "Score", "Level", "Host", "Name");
	printf("\t%s\t%s\t%s\t%s\t%s\n",
	    "---", "-----", "-----", "---------", "--------");
	for (i = 0, s = scores; i < MAX_SCORES; i++, s++)
		if (s->s_score > 0) {
			printf("\t%3d\t%5d\t%4d\t%-15s\t%.*s\n",
				inf++, s->s_score, s->s_level, s->s_host,
				sizeof(s->s_name), s->s_name);
		}
	printf("\n");
}


/*
 * read_scores:
 *	Read the current score file into memory.
 *
 */

read_scores()
{
	FILE	*sf;
	int	i;
	SCORE	*s;
	char	buf[BUFSIZ];

	/* open score file */
	if ((sf = fopen(SCORE_FILE, "r")) == NULL)
		/* no score file yet, outta here... */
		return;
	
	/* zero score list */
	for (i = 0, s = scores; i < MAX_SCORES; i++, s++)
		s->s_score = 0;
	
	/* read score file */
	for (i = 0, s = scores; i < MAX_SCORES; i++, s++) {
		s->s_score = 0;
		if (!feof(sf) && !ferror(sf) && fgets(buf, BUFSIZ, sf)) {
			sscanf(buf, "%d %d %d %d %d %s %s\n",
			    &s->s_uid, &s->s_score, &s->s_level,
			    &s->s_starttime, &s->s_endtime,
			    s->s_name, s->s_host);
		}
	}
	fclose(sf);
}


/*
 * write_scores:
 *	write the score file back to disk.
 *
 */

write_scores()
{
	FILE	*sf;
	int	i;
	SCORE	*s;

	if ((sf = fopen(SCORE_FILE, "w")) == NULL)
		/* can't write score file, oh well... */
		return;
	
	for (i = 0, s = scores; i < MAX_SCORES && s->s_score > 0; i++, s++) {
		fprintf(sf, "%d %d %d %d %d %s %s\n",
			    s->s_uid, s->s_score, s->s_level,
			    s->s_starttime, s->s_endtime,
			    s->s_name, s->s_host);
	}
	fclose(sf);
}


/*
 * top_score:
 *	Return the current highest score.
 *
 */

top_score()
{
	FILE	*sf;
	int	s;
	char	buf[BUFSIZ];

	s = scores->s_score;
	if ((sf = fopen(SCORE_FILE, "r")) != NULL) {
		if (fgets(buf, BUFSIZ, sf) != NULL)
			sscanf(buf, "%*d %d", &s);
		fclose(sf);
	}
	return(s);
}

/*
 * give_bonus:
 *	Give a bonus depending on the number of walls left on the screen.
 *
 */

give_bonus()
{
	int bonus;

	bonus = Level * WallsLeft;
	mvprintw(Y_BONUS, X_BONUS, "Bonus: %d", bonus);
	refresh();
	add_to_score(bonus);
	if (Pause)
		sleep(2);
	else
		getchar();
	move(Y_BONUS, X_BONUS);
	clrtoeol();
}


DEBUG(fmt, a1, a2, a3, a4, a5)
	char *fmt;
{
	static FILE *df = NULL;

	if (!df) {
		df = fopen("DEBUG", "w");
		if (df == NULL) {
			BEEP();
			BEEP();
		}
	}
	fprintf(df, fmt, a1, a2, a3, a4, a5);
}
