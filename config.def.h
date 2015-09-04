/*
	Snake Game Settings 
	Edit this file to change aspects of the game
	at compile time.

	This software is licensed under the ISC license,
	See LICENSE for full text.
*/

/* Required for Apple Table */
#include "apples.h"

/* World Properties ======================================================== */
#define WORLD_WIDTH  (60)
#define WORLD_HEIGHT (22)

/* Snake Properties ======================================================== */
/* Number of seconds a new snake starts as */
static const int SnakeInitialLength = 5;

/* Toggles if snake dies on impact with wall (classic behavior)
    or looses segments and respawns */
static const bool OnImpactDie = false;
/* Reset apple population on impact */
static const bool OnImpactClearApples = true;
/* Precent of snake lost on impact. 0 means fixed loss-only */
static const int OnImpactLoss = 50;
/* Minimum segment lost on impacting a wall: 0 means factional loss-only */
static const int OnImpactMinLoss = 10;

/* Apple Properties ======================================================== */
/* Odds are N out of N0 (normal apples) */
/* eg: N0 = 1000, 100 means 100 out of 1000 chance. */
/* Zero means that type will never spawn. */
static const AppleStat AppleT[APPLE_LAST] = {
   /* Odds, Growth, Rot_Time,       Rot_To, Nourishment, Score */
   /*       onSpawn,      onTick,       onEat,   onRotAway*/
	{ 1000,      1,        0,           -1,        5000,   500, /* Normal Apple */
               NULL,        NULL,        NULL,        NULL},

	{  100,      3,     1000, APPLE_NORMAL,       15000,  1500, /* Juicy Apple */
               NULL,        NULL,        NULL,        NULL},

	{   10,      5,      750,  APPLE_JUICY,       25000,  5000, /* Golden Apple */
               NULL,        NULL,        NULL,        NULL},

	{   50,     -1,      250,           -1,      -10000, -1000, /* Rotten Apple */
               NULL,        NULL,        NULL,        NULL},

	{   15,     -5,      250,           -1,      -15000, -5000, /* Poison Apple */
               NULL,        NULL, taintApples,        NULL},

	{   50,      1,      300, APPLE_ROTTEN,        2500,  1000, /* Fermented Apple */
               NULL,        NULL,  intoxicate,        NULL},

	{    5,      1,      500,           -1,        1000,  7500, /* Pure Apple */
               NULL,        NULL,purifyApples,        NULL},

	{    1,      1,      500,           -1,        1000, 25000, /* Weeping Apple */
               NULL,quantumLock,        NULL,        NULL},

	{   50,      1,      500,           -1,        1000,  7500, /* Shy Apple */
               NULL,spookApples,        NULL,        NULL},

	{    0,     -1,     1000,           -1,       -7500, -1000, /* Amputated Segment */
               NULL,        NULL,        NULL,        NULL}
};

/* Apple Spawn Values= ===================================================== */
/* Maximum number of apples allowed to spawn */
/* NB: Some mechanics may allow this cap to be exceeded */
static const int AppleMaxPop = 10;
/* Chance 1 in X that on eating an apple a bonus one will spawn */
static const int AppleBonusChance = 10;
/* Chance 1 in X that an apple with spawn randomly on a tick */
static const int AppleSpawnChance = 2000;
/* Apple Rot Fuzz, actual apples will be +/- up to this value */
static const int AppleRotFuzz = 100;

/* Tick Values ============================================================= */
/* Base tick unit in uS */
static const int TickBase = 1000;
/* Default Tick Time in multiple of TICK_BASE uS*/
static const int TickDefault = 200;
/* Use dyanimc ticks? Set to false for static tick length */
static const bool TickDynamic = true;
/* Minimum tick length in multiples of TICK_BASE*/
static const int TickMinimum = 75;
/* Factor to speed game up by for each apple eaten (of any type) */
static const double TickScaleFactor = 0.5;

/* Hunger Values =========================================================== */
/* Cap on stored hunger based on snake length */
static const int HungerPerSegment = 5000;
/* Hunger Drail scalking factor */
static const int HungerFactor = 5;

/* Apple Effect Values ===================================================== */
/* Golden Apple */
static const int GildedDuration = 500;
/* Fermented Apple */
static const int IntoxicatedMinimum = 10;
/* Weeping Apple */
/* Chance an unobserved weeping apple moves, are are expressed as 1 in X */
static const int WeepingWarpChance = 5;
/* Chance an unobserved weeping apple strikes, odds expressed as 1 in X */
static const int WeepingAttackChance = 5;
static const int WeepingAttackDamage = 1;
/* Shy Apple*/
static const int SpookChance = 5;
