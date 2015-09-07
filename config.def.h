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
/* Percent of snake lost on impact. 0 means fixed loss-only */
static const int OnImpactLoss = 50;
/* Minimum segment lost on impacting a wall: 0 means factional loss-only */
static const int OnImpactMinLoss = 10;

/* Apple Properties ======================================================== */
/* Odds are N out of N0 (normal apples) */
/* eg: N0 = 1000, 100 means 100 out of 1000 chance. */
/* Zero means that type will never spawn. */
static const AppleStat AppleT[APPLE_LAST] = {
   /* Odds, Growth, Rot_Time,       Rot_To, Nourishment, Saturation,  Score */
   /*       onSpawn,      onTick,       onEat,   onRotAway*/
	{ 1000,      1,        0,           -1,        5000,         30,    500, /* Normal Apple */
               NULL,        NULL,        NULL,        NULL},

	{  200,      0,        0,           -1,        3000,         15,    250, /* Tiny Apple */
               NULL,        NULL,        NULL,        NULL},

	{  100,      3,     1000, APPLE_NORMAL,       15000,         60,   1500, /* Juicy Apple */
               NULL,        NULL,        NULL,        NULL},

	{   10,      5,      750,  APPLE_JUICY,       25000,        150,   5000, /* Golden Apple */
               NULL,        NULL,        NULL,        NULL},

	{   50,     -1,      250,           -1,      -10000,     -10000,  -1000, /* Rotten Apple */
               NULL,        NULL,        NULL,        NULL},

	{   15,     -5,      250,           -1,      -15000,     -10000,  -5000, /* Poison Apple */
               NULL,        NULL, taintApples,        NULL},

	{   50,      1,      300, APPLE_ROTTEN,        2500,         30,   1000, /* Fermented Apple */
               NULL,        NULL,  intoxicate,        NULL},

	{    5,      1,      500,           -1,        1000,        100,   7500, /* Pure Apple */
               NULL,        NULL,purifyApples,        NULL},

	{    1,      1,      500,           -1,        1000,         50,  25000, /* Weeping Apple */
               NULL,quantumLock,        NULL,        NULL},

	{   50,      1,      500,           -1,        1000,         50,  15000, /* Shy Apple */
               NULL,spookApples,        NULL,        NULL},

	{    0,     -1,     1000,           -1,       -7500,     -10000,  -1000, /* Amputated Segment */
               NULL,        NULL,        NULL,        NULL}
};

/* Apple Spawn Values= ===================================================== */
/* Maximum number of apples allowed to spawn */
/* NB: Some mechanics may allow this cap to be exceeded */
static const int AppleMaxPop = 10;
/* Chance N in M that on eating an apple a bonus one will spawn */
static const Odds AppleBonusChance = {1, 10};
/* Chance N in M that an apple with spawn randomly on a tick */
static const Odds AppleSpawnChance = {1, 2000};
/* Apple Rot Fuzz, actual apples will be +/- up to this value */
static const int AppleRotFuzz = 100;

/* Tick Values ============================================================= */
/* Base tick unit in uS */
static const int TickBase = 1000;
/* Default Tick Time in multiple of TICK_BASE uS*/
static const int TickDefault = 200;
/* Use dynamic ticks? Set to false for static tick length */
static const bool TickDynamic = true;
/* Minimum tick length in multiples of TICK_BASE*/
static const int TickMinimum = 75;
/* Factor to speed game up by for each apple eaten (of any type) */
static const double TickScaleFactor = 0.5;

/* Hunger Values =========================================================== */
/* Snake's starting saturation level */
static const int SnakeInitalSaturation = 50;
/* Snake's maximum saturation level */
static const int SnakeMaxSaturation = 200;
/* Cap on stored hunger based on snake length */
static const int HungerPerSegment = 5000;
/* Hunger Drain scaling factor */
static const int HungerFactor = 5;

/* Apple Effect Values ===================================================== */
/* Golden Apple */
static const int GildedDuration = 500;
/* Fermented Apple */
static const int IntoxicatedMinimum = 10;
/* Weeping Apple */
/* Chance an unobserved weeping apple moves, odds are expressed as N in M */
static const Odds WeepingWarpChance = {1, 5};
/* Chance an unobserved weeping apple strikes, odds expressed as N in M */
static const Odds WeepingAttackChance = {1, 100};
/* Number of segments lost due to an attack */
static const int WeepingAttackDamage = 1;
/* Shy Apple*/
/* Chance a shy apple being directly looked at will flee the direct gaze and
   move to somewhere else, odds are expressed as N in M */
static const Odds ShyMoveChance = {1, 5};
/* Chance a shy apple being watched will flee entiry (despawn) */
static const Odds ShyFleeChance = {1, 15};
