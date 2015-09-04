/*
	apples.c
	Special per-apple effect functions

	This software is licensed under the ISC license,
	See LICENSE for full text.
*/

#include "snake.h"
#include "config.h"

typedef struct ConvPair ConvPair;
struct ConvPair {
	int oldType;
	int newType;
	int chance;
};

static const ConvPair AppleTaint[] = {
	{APPLE_NORMAL, APPLE_ROTTEN, 1},
	{ APPLE_JUICY, APPLE_ROTTEN, 5},
	{APPLE_ROTTEN, APPLE_POISON, 5}
};

/* Poison Apples taint good apples when eaten */
void
taintApples(Game *g, Apple *self) {
	Apple *a;
	
	for(a = g->apples; a; a = a->next)
		switch (a->type) {
			case APPLE_NORMAL:
				if(ONEIN(2))
					transmogrifyApple(a, APPLE_ROTTEN, 0);
			break;
			case APPLE_ROTTEN:
				if(ONEIN(5))
					transmogrifyApple(a, APPLE_POISON, 0);
			break;
		}
}

/* Pure Apples pruify bad apples and can upgrade normal/juicy apples */
void
purifyApples(Game *g, Apple *self) {
	Apple *a;
	
	for(a = g->apples; a; a = a->next)
		switch (a->type) {
			case APPLE_NORMAL:
				if(ONEIN(2))
					transmogrifyApple(a, APPLE_JUICY, 0);
			break;
			case APPLE_JUICY:
				if(ONEIN(5))
					transmogrifyApple(a, APPLE_GOLDEN, 0);
			break;
			case APPLE_GOLDEN:
			case APPLE_PURE:
			break;
			default:
					transmogrifyApple(a, APPLE_NORMAL, 0);
		}
}

/*
	Weeping Apples are quantum locked as long as they are observed
	Un-observed apples might move around, disappear,
	or even attack the snake if given the chance.
*/
void
quantumLock(Game *g, Apple *self) {


}

void
angelStrike(Game *g, Apple *self) {
	if(ONEIN(3))
		trimSnake(g, 10, 0);
}


/* Fermented apples make the snake drunk when eaten. */
void
intoxicate(Game *g, Apple *self) {
	/* Fresher fermented apples make the snake less drunk */
	g->snake.status[EFFECT_CONFUSED] += MIN(10,AppleT[self->type].rot_time - self->rot);
}
