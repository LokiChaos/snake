/*
	Not-so-simple Snake/Worm-like game
	Author: LokiChaos
	This software is licensed under the ISC license,
	See LICENSE for full text.
*/

#include <time.h>
#include <unistd.h>

#include "snake.h"
#include "ui.h"

#include "apples.h"
#include "config.h"

void place(Coord *, int, int);
int placeRandomly(Game *, Coord *);

Segment *newSegment();
Segment *newSnake(Coord, unsigned int);
void moveSnake(Game *);
void amputateSnake(Game *, Segment *, bool);

Apple *newApple();
void eatApple(Game *, Apple *);
void tickAppleRot(Game *);
void tickRotAway(Game *);

Game *newGame();
void cleanupGame(Game *);
int recalcTick(Game *);
int smartDir(Coord, Bounds);
void tick(Game *);
bool checkApples(Game *);
bool checkWall(Coord *, Bounds, int);
bool checkBite(Segment *, int);

int calculateScore(Game *);
int hungerDrain(int, Bounds);
int hungerCap(int);

void
die(const char *errstr, ...) {
	va_list ap;
	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

/* Sets coordinate to specified values */
void
place(Coord *c, int x, int y) {
	c->x = x;
	c->y = y;
}

/* Sets coordinate to a random, unoccupied location */
int
placeRandomly(Game *g, Coord *c) {
	int valid;
	int loc;
	Segment *s;
	Apple *a;

	/* Count number of vacant locations */
	valid = g->world.x.max * g->world.y.max;
	for(s = g->snake.head; s; s = s->next, valid--);
	for(a = g->apples;     a; a = a->next, valid--);

	/* If there are none, place at origin and abort */
	if(!valid) {
		place(c, g->world.x.min, g->world.y.min);
		return false;
	}

	/* Select from the valid locaitons */
	loc = rand() % valid;

	/* Translate the valid location to the real coordiantes */
	for(s = g->snake.head; s; s = s->next)
		if(loc >= s->c.x + g->world.x.max * s->c.y)
			loc++;
	for(a = g->apples;     a; a = a->next)
		if(loc >= a->c.x + g->world.x.max * a->c.y)
			loc++;

	/* Wrap around logic */
	loc %= valid;
	
	/* Perform placement */
	place(c, loc % g->world.x.max, loc / g->world.x.max);
	return true;
}

bool
inRect(Coord c, Bounds b) {
	return c.x >= b.x.min && c.x <=b.x.max && c.y >= b.y.min && c.y <= b.y.max;
}

bool
inRadius(Coord c, Coord center, int rad) {
	/* The game is orthogonal, so this more accurately represents the concept
	   of radius on a grid */
	return abs(c.x - center.x) + abs(c.y - center.y) <= rad;
}

int
smartDir(Coord p, Bounds b) {
	/* Snake should always start moving away from the two closest walls */
	int d;
	d = 1 + rand() % 2;
	if( d == DIR_RIGHT && p.x > b.x.max / 2 )
		d = DIR_LEFT;
	if( d == DIR_UP && p.y < b.y.max / 2 )
		d = DIR_DOWN;
	return d;
}

/* Snakes  {{{ */
Segment *
newSegment() {
	Segment *s;
	if(!(s = calloc(1,sizeof(Segment))))
		die("Error: Failed to create segment.\n");
	s->next = NULL;
	return s;
}

Segment *
newSnake(Coord c, unsigned int length) {
	Segment *head = NULL;
	Segment *seg = NULL;
	int i;

	for(i = 0; i < length; i++) {
		seg = head;
		head = newSegment();
		place(&(head->c), c.x, c.y);
		if(seg)
			seg->prev = head;
		head->next = seg;
	};
	return head;
}

void
snakeImpact(Game *g) {
	Segment *s;

	trimSnake(g, MAX(OnImpactMinLoss, (int)( g->snake.length * (double)OnImpactLoss / 100.0 )), 0);
	
	if(!g->snake.head)
		return;

	if(OnImpactClearApples) {
		clearApples(g);
		spawnApple(g, -1, false);
	}

	/* Randomly Place Snake */
	placeRandomly(g, &(g->snake.head->c));
	g->snake.dir = smartDir(g->snake.head->c, g->world);
	for(s = g->snake.head; s; s = s->next)
		place(&(s->c), g->snake.head->c.x, g->snake.head->c.y);
	moveSnake(g);
}

void
transmogrifySegment(Segment *s) {
	//spawnApple()
	//TODO: spawn apple and remove segment
}

void
trimSnake(Game *g, int c, int corpse) {
	Segment *s;
	Segment *sp;
	/* Find end of snake */
	for(s = g->snake.head; s && s->next; s = s->next);

	/* Move back to head, removing segments */
	for(;c && s;c--, s = sp) {
		sp = s->prev;
		if(sp)
			sp->next = NULL;
		free(s);
		g->snake.length--;
	}
	/* No snake left */
	if(!g->snake.length)
		g->snake.head = NULL;
}

void
moveSnake(Game *g) {
	Segment *newHead;
	Segment *s;
	newHead = newSegment();

	if(!g->snake.head)
		return;

	/* Posistion the head based on the old head's location and current direction */
	place(&(newHead->c), 
	      g->snake.head->c.x + ((g->snake.dir & 2) ? 0 : (g->snake.dir & 1 ? 1 : -1)),
		  g->snake.head->c.y + ((g->snake.dir & 2) ? (g->snake.dir & 1 ? 1 : -1) : 0));


	/* Put the new head at the head of the snake list */
	newHead->next = g->snake.head;
	g->snake.head->prev = newHead;
	g->snake.head = newHead;

	/* Do we need to trim the tail? */
	if(g->snake.toGrow) {
		/* No, don't trim and update the growth value */
		g->snake.toGrow--;
		g->snake.length++;
	}
	else {
		/* Find the tail of the snake */
		for(s = g->snake.head;s && s->next && s->next->next;s = s->next);
		/* The new tail is not part of the snake */
		free(s->next);
		s->next = NULL;
	}
}
/* }}} */

/* Apple {{{ */
Apple *
newApple() {
	Apple *a;
	if(!(a = calloc(1,sizeof(Apple))))
		die("Error: Failed to create apple.\n");
	a->next = NULL;
	a->prev = NULL;
	return a;
}

int
appleRotTime(int type) {
	if(AppleT[type].rot_time)
		return MAX(10, AppleT[type].rot_time + 2 * ( rand() % AppleRotFuzz ) - AppleRotFuzz);
	else
		return -1;
}

void
spawnApple(Game *g, int type, int ignorePopCap) {
	Apple *a;
	int i;

	if(!ignorePopCap && g->applePop >= AppleMaxPop)
		return;

	g->applePop++;

	a = newApple();

	/* Determine Apple Type */
	if(type == -1)
		for(i = rand() % AppleT[APPLE_NORMAL].odds, type = APPLE_LAST - 1;
			type >= 0 && i > AppleT[type].odds; i -=AppleT[type--].odds);

	a->type = type;

	/* Place Apple */
	placeRandomly(g, &(a->c));

	/* Initalize apple rot timer */
	a->rot = appleRotTime(a->type);

	/* Add new apple to list */
	a->next = g->apples;
	if(g->apples != NULL)
		g->apples->prev = a;
	g->apples = a;

	/* If apple has a special action on being spawned, run it */
	if(AppleT[a->type].onSpawn) {
		AppleT[a->type].onSpawn(g, a);
	}
}


void
delistApple(Apple **head, Apple *a) {
	/* Remove a from list */
	if(a && a->prev)
		a->prev->next = a->next;
	if(a && a->next)
		a->next->prev = a->prev;

	/* If a was head of the list, make next apple the new head */
	if(a == *head)
		*head = a->next;
}

void
eatApple(Game *g, Apple *a) {

	/* Remove apple from list */
	delistApple(&g->apples, a);
	g->applePop--;

	/* Process the apple's general effects */
	g->snake.toGrow += MAX(AppleT[a->type].growth, 0);
	g->snake.hunger += AppleT[a->type].nourishment;

	if(AppleT[a->type].growth < 0)
		trimSnake(g, -1 * AppleT[a->type].growth, 0);

	g->stats.applesEaten[a->type]++;

	/* If apple has a special action on being eaten, run it */
	if(AppleT[a->type].onEat) {
		AppleT[a->type].onEat(g, a);
	}

	/* Apple has been consumed and no longer existst */
	free(a);
}

void
rotApple(Game *g, Apple *a) {
	Apple *na;

	delistApple(&g->apples, a);
	g->applePop--;

	if(0 < AppleT[a->type].rot_to) {
		/* Create decay product */
		na = newApple();
		/* Set decay products properties */
		na->type = AppleT[a->type].rot_to;
		place(&(na->c), a->c.x, a->c.y);
		na->rot = appleRotTime(na->type);

		/* Add decay product apple to list */
		na->next = g->apples;
		if(g->apples != NULL)
			g->apples->prev = na;
		g->apples = na;
	}

	/* If apple has a special action on being eaten, run it */
	if(AppleT[a->type].onRotAway) {
		AppleT[a->type].onRotAway(g, a);
	}

	/* Apple has rotted away and no longer exists*/
	free(a);
}

void
tickApples(Game *g) {
	Apple *a;
	Apple *n;
	for(a = g->apples; a != NULL ; a = n) {
		if(a) {
			n = a->next;
			if(a->rot != -1 ) {
				if(!a->rot) {
					rotApple(g, a);
				}
				else {
					if(a && AppleT[a->type].onTick) {
						AppleT[a->type].onTick(g, a);
						}
					a->rot--;
				}
			}
		}
	}
}

void
clearApples(Game *g) {
	Apple *a;
	Apple *na;
	for(a = g->apples; a; na = a->next, free(a), a = na);
	g->apples = NULL;
	g->applePop = 0;
}

void
transmogrifyApple(Apple *a, int newType, int keepRot) {
	a->type = newType;
	if(!keepRot)
		a->rot = appleRotTime(newType);
}

void
warpApple(Game *g, Apple *a, Coord *c) {
	if(c)
		placeRandomly(g, &(a->c));
	else
		place(&(a->c), c->x, c->y);
}
/* }}} */

/* Game {{{ */
Game *
newGame() {
	Game *g;
	Coord pos;

	if(!(g = (Game *)calloc(1, sizeof(Game)))) {
		die("Error: Failed to create Game.\n");
	} {
		g->world = (Bounds){{0, WORLD_WIDTH}, {0, WORLD_HEIGHT}};
		placeRandomly(g, &pos);
		g->snake.head = newSnake(pos, SnakeInitialLength);
		g->snake.length = SnakeInitialLength;
		g->snake.dir = smartDir(g->snake.head->c, g->world);
		g->snake.status[EFFECT_CONFUSED] = 0;
		g->snake.status[EFFECT_GILDED] = 0;
		g->snake.hunger = SnakeInitialLength * HungerPerSegment;
		g->snake.toGrow = 0;

		g->applePop = 0;
		spawnApple(g, -1, false);

		g->tick = TickDefault;
		g->turn = 0;
		g->score = 0;
		g->state = STATE_PREGAME;

		g->debug = 0;
	}
	return g;
}

void
cleanupGame(Game *g) {
	Segment *s;
	Segment *ns;
	Apple *a;
	Apple *na;
	/* Loop down and free whole snake list */
	for(s = g->snake.head; s; ns = s->next, free(s), s = ns);
	/* Loop down and free whole apple list */
	for(a = g->apples; a; na = a->next, free(a), a = na);
	/* Free game */
	free(g);
}
/* }}} */

/* Collision Checks {{{ */
bool
checkApples(Game *g) {
	Apple *a;

	if(!g->snake.head)
		return false;

	/* Apple collisons - Eating*/
	for(a = g->apples;a != NULL; a = a->next) {
		if(a->c.x == g->snake.head->c.x && a->c.y == g->snake.head->c.y) {
			eatApple(g, a);
			return true;
		}
	}
	return false;
}

bool
checkWall(Coord *h, Bounds w, int wrap) {
	if(wrap) {
		h->x = h->x + (h->x < 0) * w.x.max - (h->x >= w.x.max) * w.x.max;
		h->y = h->y + (h->y < 0) * w.y.max - (h->y >= w.y.max) * w.y.max;
	}
	else {
		if(h->x < w.x.min || h->x >= w.x.max ||
		   h->y < w.y.min || h->y >= w.y.max)
				return true;
	}
	return false;
}


bool
checkBite(Segment *head, int specter) {
	Segment *s;
	if(!specter)
		for(s = head->next;s != NULL; s = s->next)
			if(s->c.x == head->c.x && s->c.y == head->c.y)
				return true;
	return false;
}
/* }}} */

int
hungerDrain(int len, Bounds b) {
	int drain;
	drain = len * HungerFactor;
	drain = MIN(drain, hungerCap(len) / (b.x.max + b.y.max) );
	return drain;
}

int
hungerCap(int len) {
	return len * HungerPerSegment;
}

int
calculateScore(Game *g) {
	int i;
	int score;
	
	score = g->scoreBonus;
	/* Total up points from apples eaten */
	for(i = 0; i < APPLE_LAST; i++)
		score += AppleT[i].score * g->stats.applesEaten[i];
	/* Longer snake */
	score += g->snake.length * 100;
	/* Longer game time */
	score += g->turn;
//	score -=MAX_HUNGER - g->snake.hunger;
//	TODO: include apples decayed, lost segments, wall impact count, and self-impact count
	return MAX(0,score);
}

void
tick (Game *g) {
	if(g->state != STATE_GAME)
		return;

	g->turn++;

	g->snake.turning = false;

	if(!(DEBUG_MODE(g->debug, DEBUG_WAIT)))
		moveSnake(g);

	if(checkWall(&(g->snake.head->c), g->world, DEBUG_MODE(g->debug, DEBUG_ASTEROIDS))) {
		if(OnImpactDie)
			g->state = STATE_LOSS;
		else
			snakeImpact(g);
	}

	if(g->snake.head && checkBite(g->snake.head, DEBUG_MODE(g->debug, DEBUG_SPECTER)))
		g->state = STATE_LOSS;

	if(g->snake.status[EFFECT_CONFUSED] > 0)
		g->snake.status[EFFECT_CONFUSED]--;

	if(g->snake.status[EFFECT_GILDED] > 0)
		g->snake.status[EFFECT_GILDED]--;

	if(!DEBUG_MODE(g->debug, DEBUG_ANOREXIC))
		if(checkApples(g)) {
			/* Spawn a new apple N-1 / N times */
			if(AppleBonusChance && !(ONEIN(AppleBonusChance)))
			spawnApple(g, -1, false);
		
			/* Spawn a new apple 1 / N times */
			if(AppleBonusChance && ONEIN(AppleBonusChance))
			spawnApple(g, -1, false);
		}

	if(!DEBUG_MODE(g->debug, DEBUG_HUNGERLESS))
		if(0 >= MIN(hungerCap(g->snake.length),(g->snake.hunger -= hungerDrain(g->snake.length, g->world))))
			g->state = STATE_LOSS;

	tickApples(g);

	/* Game Over if the snake is gone */
	if(!g->snake.head)
		g->state = STATE_LOSS;

	/* Chance for random apple spawn */
	if(AppleSpawnChance && ONEIN(AppleSpawnChance))
		spawnApple(g, -1, false);

	/* Always make sure at least one apple exists */
	if(!g->apples)
		spawnApple(g, -1, false);

	g->score = calculateScore(g);

	if(TickDynamic)
		g->tick = recalcTick(g);
}

int
recalcTick(Game *g) {
	int i;
	int totalApples;

	for(i = 0, totalApples = 0; i < APPLE_LAST; i++)
		totalApples += AppleT[i].score > 0 ? g->stats.applesEaten[i] : 0;

	// TODO: Make adjustable via debug and of the form t = f(t)
	return MAX(TickMinimum, TickDefault - TickScaleFactor * (double)(totalApples * totalApples));
}

/* Key functions {{{ */
void
changeDir(Game *g, const Arg *arg) {
	int d;

	if(g->snake.status[EFFECT_CONFUSED])
		d = arg->i ^ 1;
	else
		d = arg->i;
	/* Don't let a snake turn back onto itself or to an invalid direction */
	if(!g->snake.turning && d >= 0 && d < DIR_LAST && (2 & d) != (2 & g->snake.dir)) {
		g->snake.dir = d;
		g->snake.turning = true;
	}
}

void
changeState(Game *g, const Arg *arg) {
	g->state = arg->i;
}

void
dumpGame(Game *g, const Arg *arg) {
/* Dumping Game to file [UGLY] {{{ */
	FILE *dumpFile;
	char outputFilename[] = "snake.dump";

	if(!(dumpFile = fopen(outputFilename, "w")))
		return;

	fprintf(dumpFile, "Tick  = %d\n",  g->tick);
	fprintf(dumpFile, "Turn  = %d\n",  g->turn);
	fprintf(dumpFile, "Score = %d\n", g->score);
	fprintf(dumpFile, "State = %d\n", g->state);
	fprintf(dumpFile, "Snake = [\n");
	fprintf(dumpFile, "  toGrow   = %d\n",  g->snake.toGrow);
	fprintf(dumpFile, "  dir      = %d\n",  g->snake.dir);
	fprintf(dumpFile, "  hunger   = %d\n",  g->snake.hunger);
	fprintf(dumpFile, "  confused = %d\n",  g->snake.status[EFFECT_CONFUSED]);
	fprintf(dumpFile, "  gilded   = %d\n",  g->snake.status[EFFECT_GILDED]);
	fprintf(dumpFile, "  Segments = [\n");
	Segment *s;
	for(s = g->snake.head; s; s = s->next) {
		fprintf(dumpFile, "    Addr = 0x%08lX\n", (unsigned long int)s);
		fprintf(dumpFile, "    Next = 0x%08lX\n", (unsigned long int)s->next);
		fprintf(dumpFile, "    Prev = 0x%08lX\n", (unsigned long int)s->prev);
		fprintf(dumpFile, "    x,y  = %3d, %3d\n\n", s->c.x, s->c.y);
	}
	fprintf(dumpFile, "  ]\n");
	fprintf(dumpFile, "]\n");
	fprintf(dumpFile, "Apples = [\n");
	Apple *a;
	for(a = g->apples; a; a = a->next) {
		if(!a)
			break;
		fprintf(dumpFile, "  Addr = 0x%08lX\n", (unsigned long int)a);
		fprintf(dumpFile, "  Next = 0x%08lX\n", (unsigned long int)a->next);
		fprintf(dumpFile, "  Prev = 0x%08lX\n", (unsigned long int)a->prev);
		fprintf(dumpFile, "  Type = %d\n", a->type);
		fprintf(dumpFile, "  Rot  = %d\n", a->rot);
		fprintf(dumpFile, "  x,y  = %3d, %3d\n\n", a->c.x, a->c.y);
	}
	fprintf(dumpFile, "]\n");
	fclose(dumpFile);
/* }}} */
}

void
debugToggle(Game *g, const Arg *arg) {
	g->debug = DEBUG_TOGGLE(g->debug, arg->i);
}

void
debugSpawn(Game *g, const Arg *arg) {
	spawnApple(g, arg->i, true);
}
/* }}} */

int
main(int argc, char *argv[]) {

	Game *game;
	UI  *ui;
	int key;
	int clk;

	clk = 0;

	/* Pre-game Init */
	srand(time(NULL));

	if(!(game = newGame()))
		die("Error: Failed to create game.\n");

	ui = initUI(game->world);

	/* Primary game loop */
	while(game->state != STATE_QUIT) {

		/* Grab any input */
		key = getKey();
		if(key != ERR)
			doKey(game, key);

		if(clk == game->tick) {
			tick(game);
			clk = 0;
		}

		if(game->state != STATE_QUIT)
			redraw(game, ui);

		clk++;
		/* Wait until next cycle */
		usleep(TickBase);
	}

	/* Post-game cleanup */
	cleanupGame(game);
	cleanupUI(ui);

	return 0;
}
