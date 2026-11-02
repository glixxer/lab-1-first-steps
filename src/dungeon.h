#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdbool.h>
#include "player.h"
#include "utils.h"

typedef enum
{
    TILE_WALL = '#',
    TILE_FLOOR = '.',
    TILE_TREE = 'T',
    TILE_ROCK = 'R',
    TILE_ICE = '~',
    TILE_UP = 'U',
    TILE_DOWN = 'D',
    TILE_HERO = '@'
} Tile;

typedef enum
{
    STEP_BLOCKED,
    STEP_MOVED,
    STEP_DESCEND,
    STEP_EVACUATE
} StepResult;

typedef struct Dungeon
{
    char** cells;
    int width, height;
    unsigned int depth;
    bool sliding;
    int slide_dx, slide_dy;
} Dungeon;

Dungeon* dungeon_create(int width, int height, unsigned int depth);
Dungeon* dungeon_generate(unsigned int depth);
void dungeon_destroy(Dungeon* dungeon);
bool dungeon_can_enter(const Dungeon* dungeon, int x, int y);
bool dungeon_stairs_reachable(const Dungeon* dungeon);
StepResult dungeon_step(Dungeon* dungeon, Player* player, int dx, int dy);
bool dungeon_descend(Dungeon** dungeon, Player* player, GameTime* time);
bool adventure_allowed(const Player* player, const GameTime* time);
void adventure_run(Dungeon** dungeon, Player* player, GameTime* time);
#endif
