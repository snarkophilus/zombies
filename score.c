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
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>

#include "zombies.h"


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

static void set_name(SCORE *s);

/*
 * score:
 *	Post the player's score, if reasonable, and then print out the
 *	top list.
 */

void
score(void)
{
	int	done_show, i, lfd;
	int	newscore, top, bottom, topline;
	char	lockfile[BUFSIZ];
	SCORE	*s;

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
	newscore = add_score();
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


int
add_score(void)
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


static void
set_name(SCORE *s)
{
	char		*p, buf[MAXHOSTNAMELEN];
	struct passwd	*pp;

	if ((pp = getpwuid(s->s_uid)) == NULL)
		pp->pw_name = "???";
	strncpy(s->s_name, pp->pw_name, MAXNAME);
	gethostname(buf, MAXHOSTNAMELEN);
	if ((p = strchr(buf, '.')) != NULL)
		*p = '\0';
	strncpy(s->s_host, buf, MAXNAME);
}


/*
 * show_score:
 *	Show the score list for the '-s' option.
 *
 * also used to return top score for init_field();
 */

void
show_score(void)
{
	int	i, inf;
	SCORE	*s;

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
				(int)sizeof(s->s_name), s->s_name);
		}
	printf("\n");
}


/*
 * read_scores:
 *	Read the current score file into memory.
 *
 */

void
read_scores(void)
{
	int	i;
	char	buf[BUFSIZ];
	FILE	*sf;
	SCORE	*s;

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

void
write_scores(void)
{
	int	i;
	FILE	*sf;
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

int
top_score(void)
{
	int	s;
	char	buf[BUFSIZ];
	FILE	*sf;

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

void
give_bonus(void)
{
	int	bonus;

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
