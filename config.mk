# This software is licensed under the ISC license,
# See LICENSE for full text.

# snake version
VERSION = 1.0

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# includes and libs
LIBS = -lncurses

# flags
CPPFLAGS = -D _DEFAULT_SOURCE -DVERSION=\"${VERSION}\" -D _TUI -D DEBUG
#CFLAGS   = -g -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${CPPFLAGS}
LDFLAGS  = -s ${LIBS}

# compiler and linker
CC = cc
