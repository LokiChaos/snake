/*
	ui.h
	General and specific UI.

	This software is licensed under the ISC license,
	See LICENSE for full text.
*/

/* General UI */
enum snakeParts {SNAKE_BODY,
                 SNAKE_HEAD,
                 SNAKE_CONFUSED,
                 SNAKE_GILDED,
                 SNAKE_LAST
};

typedef struct Key Key;
struct Key {
	int mode;
	int key;
	void (*func)(Game *g, const Arg *arg);
	Arg arg;
};


/* nCurses Specific */
#ifndef _SNAKES_CURSES_H_
#define _SNAKES_CURSES_H_

#include <ncurses.h>
#include <signal.h>

#define SNAKES_CURSES

enum windows {WIN_UNIVERSE,
              WIN_STATUS,
              WIN_MENU,
              WIN_ROOT,
              WIN_LAST};


/* NCurses Context */
typedef struct UI UI;
struct UI {
	WINDOW *w[WIN_LAST];
	char status[100];
};

typedef struct Glyph Glyph;
struct Glyph {
	char glyph;
	int fg;
	int bg;
	int attr;
};

void resizeHandler(int);

#endif /* _SNAKES_CURSES_H_ */


/* External Functions (shared between all UI implementations) */

UI *initUI(Bounds);
void cleanupUI(UI *);

void redraw(Game *, UI *);

int  getKey();
void doKey(Game *, int);
