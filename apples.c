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
		if(((self->c.y < g->snake.head->c.y) == (g->snake.dir & 1) && (g->snake.dir & 2)) ||
		   ((self->c.x < g->snake.head->c.x) == (g->snake.dir & 1) && !(g->snake.dir & 2))) {
			if(ODDS(WeepingAttackChance)) {
				trimSnake(g, WeepingAttackDamage, 0);
				self->rot = 1;
			}
			else if(ODDS(WeepingWarpChance)) {
				placeRandomly(g, &self->c);
			}
			//TODO: Once random placement permits limited to subregious
			// Limit warp to unobserved area
		}

}

void
spookApples(Game *g, Apple *self) {
		if((self->c.x == g->snake.head->c.x && ((self->c.y > g->snake.head->c.y) == (g->snake.dir & 1) && (g->snake.dir & 2))) ||
		   (self->c.y == g->snake.head->c.y && ((self->c.x > g->snake.head->c.x) == (g->snake.dir & 1) && !(g->snake.dir & 2)))) {
			if(ODDS(ShyFleeChance))
				self->rot = 1;
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
