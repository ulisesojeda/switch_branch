VERSION=	0.0.2

PREFIX=		/usr/local

PROG=	sb
OBJS=	sb.c

CFLAGS+=	-pedantic -Wall -Wextra
CFLAGS+=	-DVERSION=\"${VERSION}\"

all: ${PROG}
${PROG}: ${OBJS}
	${CC} -o ${PROG} ${OBJS} ${CFLAGS}

clean:
	rm -f ${PROG} ${OBJS}


.PHONY: all clean dist distclean install

