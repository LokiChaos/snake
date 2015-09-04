/*
	UI Configuration
	Sections of this are intrinsically linked to the UI implementation.

	This software is licensed under the ISC license,
	See LICENSE for full text.
*/

/* nCurses Specific Settings */
#ifdef SNAKES_CURSES
/* Colors & Attributes {{{
   Valid Colors
	-1 for default
	COLOR_BLACK     COLOR_BLUE
	COLOR_RED       COLOR_MAGENTA
	COLOR_GREEN     COLOR_CYAN
	COLOR_YELLOW    COLOR_WHITE
   Valid Arributes
	A_NORMAL        A_BLINK
	A_STANDOUT      A_DIM
	A_UNDERLINE     A_BOLD
	A_REVERSE
   Multiple attributes can be combined with bitwise or |
   }}} */
static const Glyph AppleGlyphs[APPLE_LAST] = {
/*  Glyph,   Foreground,   Background, Attributes */
     {'A',    COLOR_RED,           -1,          0}, /* Normal Apple */
     {'J',    COLOR_RED,           -1,     A_BOLD}, /* Juicy Apple */
     {'A', COLOR_YELLOW,           -1,     A_BOLD}, /* Golden Apple */
     {'R',  COLOR_GREEN,           -1,          0}, /* Rotten Apple */
     {'P',  COLOR_GREEN,           -1,     A_BOLD}, /* Poison Apple */
     {'F',   COLOR_CYAN,           -1,          0}, /* Fermented Apple */
     {'P',  COLOR_WHITE,           -1,     A_BOLD}, /* Pure Apple */
     {'A',  COLOR_WHITE,           -1,     A_BOLD}, /* Weeping Apple */
     {'S',  COLOR_WHITE,           -1,     A_BOLD}, /* Shy Apple */
     {'O',  COLOR_GREEN,           -1,          0}  /* Rotting Segment */
};

static const Glyph SnakeGlyphs[SNAKE_LAST] = {
/*  Glyph,   Foreground,    Background, Attributes */
     {'O',  COLOR_GREEN,           -1,     A_BOLD}, /* Snake's Body */
     {'#',  COLOR_GREEN,           -1,     A_BOLD}, /* Snake's Head */
     {'?',  COLOR_GREEN,           -1,     A_BOLD}, /* Snake's Head When Confused */
     {'#', COLOR_YELLOW,           -1,     A_BOLD}  /* Snake's Head When Gilded */
};

#endif /* SNAKES_CURSES */

/* SDL Specific Settings */
#ifdef SNAKES_SDL
#endif /* SNAKES_SDL */

/* Xlib Specific Settings */
#ifdef SNAKES_XLIB
#endif /* SNAKES_XLIB */


/* Keys */
#define KEY_ANY (0)
#define STATE_ANY (-1)

/* STATE_ANY may be used for bindings for all states */
static const Key Keys[] = {
/*            Mode,       Key,      Function,           Argument*/
	{STATE_PREGAME,   KEY_ANY,   changeState,   {.i = STATE_GAME}},
	{   STATE_GAME,  KEY_LEFT,     changeDir,     {.i = DIR_LEFT}},
	{   STATE_GAME,  KEY_DOWN,     changeDir,     {.i = DIR_DOWN}},
	{   STATE_GAME,    KEY_UP,     changeDir,       {.i = DIR_UP}},
	{   STATE_GAME, KEY_RIGHT,     changeDir,    {.i = DIR_RIGHT}},
	{   STATE_GAME,       'h',     changeDir,     {.i = DIR_LEFT}},
	{   STATE_GAME,       'j',     changeDir,     {.i = DIR_DOWN}},
	{   STATE_GAME,       'k',     changeDir,       {.i = DIR_UP}},
	{   STATE_GAME,       'l',     changeDir,    {.i = DIR_RIGHT}},
	{   STATE_GAME,       'p',   changeState,  {.i = STATE_PAUSE}},
	{  STATE_PAUSE,       'p',   changeState,   {.i = STATE_GAME}},
#ifdef DEBUG
	{   STATE_GAME,       'd',   changeState,  {.i = STATE_DEBUG}},
	{  STATE_PAUSE,       'd',   changeState,  {.i = STATE_DEBUG}},
	{  STATE_DEBUG,       '1',   debugToggle,   {.i = DEBUG_WAIT}},
	{  STATE_DEBUG,       '2',   debugToggle,{.i = DEBUG_ASTEROIDS}},
	{  STATE_DEBUG,       '3',   debugToggle, {.i = DEBUG_SPECTER}},
	{  STATE_DEBUG,       '4',   debugToggle, {.i = DEBUG_ANOREXIC}},
	{  STATE_DEBUG,       '5',    debugSpawn,            {.i =-1}},
	{  STATE_DEBUG,       '6',    debugSpawn,  {.i =APPLE_GOLDEN}},
	{  STATE_DEBUG,       'p',   changeState,   {.i = STATE_GAME}},
#endif /* DEBUG */
	{    STATE_ANY,       'w',      dumpGame,                 {0}},
	{    STATE_ANY,       'q',   changeState,   {.i = STATE_QUIT}}
};
