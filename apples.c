/*
	apples.c
	Special per-apple effect functions

	This software is licensed under the ISC license,
	See LICENSE for full text.
*/

#include "snake.h"
#include "config.h"

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
	Bounds b;
	if(!inView(self->c, g->snake.head->c, g->snake.dir)) {
		if(inRadius(g->snake.head->c, self->c, WeepingAttackRange) &&
		   ODDS(WeepingAttackChance)) {
			trimSnake(g, WeepingAttackDamage, 0);
			self->dead = true;
		}
		else if(ODDS(WeepingWarpChance)) {
			if(ODDS(WeepingChaseChance)) {
				b.x.min = MIN(self->c.x, g->snake.head->c.x);
				b.x.max = MAX(self->c.x, g->snake.head->c.x);
				b.y.min = MIN(self->c.y, g->snake.head->c.y);
				b.y.max = MAX(self->c.y, g->snake.head->c.y);
				placeInBounds(g, b, &self->c);
			}
			else
				placeRandomly(g, &self->c);
		}
	}
}

void
spookApples(Game *g, Apple *self) {
	if(inGaze(self->c, g->snake.head->c, g->snake.dir)) {
		if(ODDS(ShyFleeChance))
				self->dead = true;
		if(ODDS(ShyMoveChance))
			placeRandomly(g, &self->c);
		}
}

void
gildSnake(Game *g, Apple *self) {
	g->snake.status[EFFECT_GILDED] += GildedDuration;
}

/* Fermented apples make the snake drunk when eaten. */
void
intoxicate(Game *g, Apple *self) {
	/* Fresher fermented apples make the snake less drunk */
	g->snake.status[EFFECT_CONFUSED] += MIN(IntoxicatedMinimum, AppleT[self->type].rot_time - self->rot);
}
