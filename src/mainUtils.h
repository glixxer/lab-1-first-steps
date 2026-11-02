#ifndef MAINUTILS_H
#define MAINUTILS_H

#include <stdbool.h>
#include "dungeon.h"
#include "config.h"

static void print_menu(void);
void create_character(Player** player, Dungeon** dungeon);
void character_card(const Player* player, const GameTime* time);
void time_menu(Player* player, GameTime* time);
void run_menu(Player** player, Dungeon** dungeon, GameTime* time);


#endif