# $Header: /cvsroot/games/zombies/Makefile,v 1.2 1999/06/22 13:15:00 simonb Exp $

PROG=	zombies
SRCS=	level.c main.c misc.c move.c score.c
MAN=	zombies.6
DPADD=	${LIBCURSES} ${LIBTERM}
LDADD=	-lcurses -ltermlib
WARNS=	1

#SCORE=	-DSCORE_FILE=\"/usr/local/share/games/zombies_score\"
#SIGTYPE=-DSIGTYPE=void
#DEBUG=	-g
CFLAGS+= ${DEBUG} ${SCORE} ${SIGTYPE}

BINDIR=	/usr/local/bin
BINOWN=	games
BINGRP=	games
BINMODE=4555

.include <bsd.prog.mk>
