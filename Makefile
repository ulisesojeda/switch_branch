VERSION=	0.0.1

PREFIX=		/usr/local
MANPREFIX=	${PREFIX}/share/man

PROG=	sb
OBJS=	switch_branch.c

INSTALL_PROGRAM=	install -s -m 0755
INSTALL_MAN=		install -m 0644

CFLAGS+=	-pedantic -Wall -Wextra
CFLAGS+=	-DVERSION=\"${VERSION}\"

DISTFILES=	CHANGELOG.md \
		LICENSE \
		Makefile \
		README.md \
		switch_branch.c

all: ${PROG}

${PROG}: ${OBJS}
	${CC} -o ${PROG} ${OBJS} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJS}

dist:
	set -e; \
	d=${PROG}-${VERSION}; \
	mkdir $$d; \
	for f in ${DISTFILES}; do \
		mkdir -p $$d/`dirname $$f`; \
		cp $$f $$d/$$f; \
	done; \
	tar czvf $$d.tar.gz $$d; \
	sha256 $$d.tar.gz >$$d.sha256; \
	rm -r $$d

distclean: clean
	rm -f ${PROG}-${VERSION}.tar.gz ${PROG}-${VERSION}.sha256

install: ${PROG}
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	#@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	${INSTALL_PROGRAM} ${PROG} ${DESTDIR}${PREFIX}/bin
	#${INSTALL_MAN} switch_branch.1 ${DESTDIR}${MANPREFIX}/man1

.PHONY: all clean dist distclean install

-include config.mk
