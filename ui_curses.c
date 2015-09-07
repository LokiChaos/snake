/*
    ui_curses.c - nCurses based TUI (Text User Interface)
    All UI code is isolated here away from game logic.

	This software is licensed under the ISC license,
	See LICENSE for full text.
*/

#include "snake.h"
#include "ui.h"

//#include "config.h"
#include "uiconfig.h"

void initInput(UI *);
void initColors();
void blankWorld(UI *);
void printApple(UI *, Coord, int);
void printSegment(UI *, Coord, bool);
void printBlank(UI *, Coord);
void printGlyph(UI *, Coord, char, int);
void printStatus(Game *,UI *);
void printBorders(WINDOW *);
int getAttr(int, int, int);
void refreshAll(UI *);

/* External */
int
getKey(){
	int k;
	k = getch();
	return k;
}

UI *
initUI(Bounds world) {
	UI *t;

	/* Handler when terminal is resized */
	signal(SIGWINCH, resizeHandler);

	if(!(t = (UI *)calloc(1, sizeof(UI))))
		die("Error: Failed to init UI context.\n");

	/* Init nCurses */
	if(!(t->w[WIN_ROOT] = initscr()))
		die("Error: Failed to init ncurses.\n");


	initInput(t);
	initColors();

	// TODO: Smarter size & placement logic
	t->w[WIN_UNIVERSE] = subwin(t->w[WIN_ROOT], 3 + world.y.max,  3 + world.x.max, 0, 0);
	t->w[WIN_STATUS] =   subwin(t->w[WIN_ROOT], 3 + world.y.max, 77 - world.x.max, 0, 3 + world.x.max);

	printBorders(t->w[WIN_UNIVERSE]);
	printBorders(t->w[WIN_STATUS]);

	refreshAll(t);

	return t;
}

void
cleanupUI(UI *t) {
	int i;
	for (i = 0; i < WIN_LAST; i++)
		delwin(t->w[i]);
	endwin();
	refresh();
	free(t);
}

void
redraw(Game *g, UI *t) {
	Apple *a;
	Segment *s;

	blankWorld(t);

	/* Draw Apples */
	for(a = g->apples; a != NULL; a = a->next) {
		printApple(t, a->c, a->type);
	}

	/* Draw Snake */
	for(s = g->snake.head; s != NULL; s = s->next) {
		printSegment(t, s->c, false);
	}

	if(g->snake.head)
		printSegment(t, g->snake.head->c, true);

	/* Redraw Status */
	printStatus(g, t);

	refreshAll(t);
}

void
doKey(Game *g, int key) {
	unsigned int i;
	int mode;
	
	mode = g->state;

	for(i = 0; i < LENGTH(Keys); i++)
		if((mode == Keys[i].mode || Keys[i].mode == STATE_ANY) &&
		   (key == Keys[i].key || Keys[i].key == KEY_ANY ) &&
		   Keys[i].func)
			Keys[i].func(g, &(Keys[i].arg));
}


/*
	Internal
*/
void
initInput(UI *t) {
	/* Make cursor invisible */
	curs_set(0);
	/* Set terminal input modes */
	cbreak();
	noecho();
	nodelay(t->w[WIN_ROOT], true);
	keypad(t->w[WIN_ROOT], true);
}

void
initColors(){
	int i, j;
	start_color();
	use_default_colors();
	for(i = 0; i < 9; i++)
		for(j = 0; j < 9; j++)
			init_pair(1 + i + (j*9), i - 1, j - 1);
}


int
getAttr(int fg, int bg, int attr) {
	return attr | COLOR_PAIR( 1 + (fg + 1)  + (9 * (bg + 1)));
}


void
printApple(UI *t, Coord c, int type) {
	printGlyph(t, c, AppleGlyphs[type].glyph,
	           getAttr(AppleGlyphs[type].fg,
	                   AppleGlyphs[type].bg,
	                   AppleGlyphs[type].attr));

}

void
printSegment(UI *t, Coord c, bool head) {
	printGlyph(t, c, SnakeGlyphs[head].glyph,
	           getAttr(SnakeGlyphs[head].fg,
	                   SnakeGlyphs[head].bg,
	                   SnakeGlyphs[head].attr));
}

void
printBlank(UI *t, Coord c) {
	printGlyph(t, c, ' ', 0);
}

void
printGlyph(UI *t, Coord c, char glyph, int color) {
	wattron(t->w[WIN_UNIVERSE], getAttr(-1,-1, A_NORMAL));
	wattron(t->w[WIN_UNIVERSE], color ? color : getAttr(-1, -1, 0));
	mvwaddch(t->w[WIN_UNIVERSE], 1 + c.y, 1 + c.x, glyph);
	wattroff(t->w[WIN_UNIVERSE], A_BOLD);
	wattron(t->w[WIN_UNIVERSE], getAttr(-1,-1, A_NORMAL));
}

void
blankWorld(UI *t) {
	int x, y;
	int height = 0;
	int width = 0;

	getmaxyx(t->w[WIN_UNIVERSE], height, width);

	for(y = 1; y < height - 1; y++)
		for(x = 1; x < width - 1; x++)
			mvwaddch(t->w[WIN_UNIVERSE], y, x, ' ');

}

void
printStatus(Game *g, UI *t) {
	wattron(t->w[WIN_UNIVERSE], A_BOLD | COLOR_PAIR(1));
	sprintf(t->status, "Score:");
	mvwaddstr(t->w[WIN_STATUS], 1, 1, t->status);
	sprintf(t->status, "%16d", g->score);
	mvwaddstr(t->w[WIN_STATUS], 2, 1, t->status);
	sprintf(t->status, "Tick:");
	mvwaddstr(t->w[WIN_STATUS], 3, 1, t->status);
	sprintf(t->status, "%16d", g->turn);
	mvwaddstr(t->w[WIN_STATUS], 4, 1, t->status);
	sprintf(t->status, "Hunger:");
	mvwaddstr(t->w[WIN_STATUS], 5, 1, t->status);
	sprintf(t->status, "%16d", g->snake.hunger);
	mvwaddstr(t->w[WIN_STATUS], 6, 1, t->status);
	sprintf(t->status, "Length:");
	mvwaddstr(t->w[WIN_STATUS], 7, 1, t->status);
	sprintf(t->status, "%16d", g->snake.length);
	mvwaddstr(t->w[WIN_STATUS], 8, 1, t->status);
	sprintf(t->status, "Apple Pop:");
	mvwaddstr(t->w[WIN_STATUS], 9, 1, t->status);
	sprintf(t->status, "%16d", g->applePop);
	mvwaddstr(t->w[WIN_STATUS], 10, 1, t->status);

	switch (g->state) {
		case STATE_PAUSE:
			mvwaddstr(t->w[WIN_STATUS], 22, 1, " [PAUSED] ");
		break;
		case STATE_DEBUG:
			mvwaddstr(t->w[WIN_STATUS], 22, 1, " [DEBUG]  ");
		break;
		case STATE_LOSS:
			mvwaddstr(t->w[WIN_STATUS], 22, 1, "Game Over ");
		break;
		default:
			mvwaddstr(t->w[WIN_STATUS], 22, 1, "          ");
	}
}

void
printBorders(WINDOW *w) {
	wborder(w, 0, 0, 0, 0, 0, 0, 0, 0);
}

void
refreshAll(UI *t) {
	int i;
	for (i = 0; i < WIN_LAST; i++)
		if(t->w[i])
			wrefresh(t->w[i]);
}

void
resizeHandler(int sig) {
	int nh, nw;
	getmaxyx(stdscr, nh, nw);
	nh = nw;
	nw = nh;
}
