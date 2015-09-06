/* 
	snakes.h

	This software is licensed under the ISC license,
	See LICENSE for full text.
*/


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef _SNAKES_H_
#define _SNAKES_H_

#define MIN(A,B) ((A < B) ? A : B)
#define MAX(A,B) ((A > B) ? A : B)
#define LENGTH(A) (sizeof(A)/sizeof(A[0]))
#define ONEIN(N) (!(rand() % (N)))
#define ODDS(A) ((rand() % (A.outOf)) < A.n)

#define DEBUG_MODE(G,A)   ((G >> A) & 1)
#define DEBUG_SET(G,A)    (G | (1 << A))
#define DEBUG_UNSET(G,A)  (G & ~(1 << A))
#define DEBUG_TOGGLE(G,A) (G ^ (1 << A))

enum Directions  {DIR_LEFT,
                  DIR_RIGHT,
                  DIR_UP,
                  DIR_DOWN,
                  DIR_LAST
};

enum GameStates  {STATE_PREGAME,
                  STATE_GAME,
                  STATE_PAUSE,
                  STATE_DEBUG,
                  STATE_LOSS,
                  STATE_QUIT,
                  STATE_LAST
};

enum SnakeStatus {EFFECT_CONFUSED,
                  EFFECT_GILDED,
                  EFFECT_LAST
};

enum AppleTypes  {APPLE_NORMAL,
                  APPLE_TINY,
                  APPLE_JUICY,
                  APPLE_GOLDEN,
                  APPLE_ROTTEN,
                  APPLE_POISON,
                  APPLE_FERMENT,
                  APPLE_PURE,
                  APPLE_WEEPING,
                  APPLE_SHY,
                  APPLE_SNAKE,
                  APPLE_LAST
};

enum DebugModes  {DEBUG_WAIT,
                  DEBUG_ASTEROIDS,
                  DEBUG_SPECTER,
                  DEBUG_IMMUNE,
                  DEBUG_ANOREXIC,
                  DEBUG_HUNGERLESS,
                  DEBUG_LAST
};


/* 2-D coordinates */
typedef struct Coord Coord;
struct Coord {
	int x, y;
};

/* Bounding box */
typedef struct Bounds Bounds;
struct Bounds {
	struct {
		int min, max;
	} x;
	struct {
		int min, max;
	} y;
};

/* Odds expression */
typedef struct Odds Odds;
struct Odds {
	int n;
	int outOf;
};

/* Generic function argument */
typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;

/* Snake body segment */
typedef struct Segment Segment;
struct Segment {
	Coord c;
	Segment *next;
	Segment *prev;
};

typedef struct Snake Snake;
struct Snake {
	Segment *head;
	int length;
	int toGrow;
	int dir;
	int turning;
	int hunger;
	int saturation;
	int status[EFFECT_LAST];
};

/* Apple */
typedef struct Apple Apple;
struct Apple {
	int type;
	int rot;
	int dead;
	Coord c;
	Apple *next, *prev;
};

/* Game State */
typedef struct Game Game;
struct Game {
	Snake snake;
	Apple *apples;
	unsigned int applePop;
	Bounds world;
	unsigned int tick;
	unsigned int turn;
	int score;
	int scoreBonus;
	int state;

	struct {
		unsigned int applesEaten[APPLE_LAST];
		unsigned int applesDecayed;
		unsigned int maxLength;
		unsigned int segmentsLost;
		unsigned int hitWall;
		unsigned int hitSelf;
	} stats;

	unsigned int debug;
};

typedef struct AppleStat AppleStat;
struct AppleStat {
	int odds;
	int growth;
	int rot_time;
	int rot_to;
	int nourishment;
	int score;
	void (*onSpawn)(Game *g, Apple *self);
	void (*onTick)(Game *g, Apple *self);
	void (*onEat)(Game *g, Apple *self);
	void (*onRotAway)(Game *g, Apple *self);
};

/* Functions */

/* Key Operations */
void changeDir(Game *, const Arg *);
void changeState(Game *, const Arg *);
void dumpGame(Game*, const Arg *);
void debugToggle(Game*, const Arg *);
void debugSpawn(Game*, const Arg *);

void die(const char *, ...);

/* Operations that may be useful externally */
void place(Coord *, int, int);
int placeRandomly(Game *, Coord *);
bool inRect(Coord, Bounds);
bool inRadius(Coord, Coord, int);
void trimSnake(Game *, int, int);
void snakeImpact(Game *);
void spawnApple(Game *, int, int);
void eatApple(Game *, Apple *);
void rotApple(Game *, Apple *);
void clearApples(Game *);
void transmogrifyApple(Apple *, int, int);
void warpApple(Game *, Apple *, Coord *);

#endif /* _SNAKES_H_ */
