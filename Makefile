# snake
# This software is licensed under the ISC license,
# See LICENSE for full text.

include config.mk

SRC = snake.c apples.c ui_curses.c
OBJ = ${SRC:.c=.o}

all: options snake

options:
	@echo snake build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo "SRC      = ${SRC}"
	@echo "OBJ      = ${OBJ}"

.c.o:
	@echo CC -c $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h uiconfig.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

uiconfig.h:
	@echo creating $@ from uiconfig.def.h
	@cp uiconfig.def.h $@

snake: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f snake ${OBJ} snake-${VERSION}.tar.gz snake.dump

dist: clean
	@echo creating dist tarball
	@mkdir -p snake-${VERSION}
	@cp -R LICENSE Makefile README config.def.h config.mk \
		${SRC} snake-${VERSION}
	@tar -cf snake-${VERSION}.tar snake-${VERSION}
	@gzip snake-${VERSION}.tar
	@rm -rf snake-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f snake ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/snakeo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < snake.1 > ${DESTDIR}${MANPREFIX}/man1/snake.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/snake.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/snake
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/snake.1

.PHONY: all options clean dist install uninstall
