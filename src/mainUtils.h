#ifndef MAINUTILS_H
#define MAINUTILS_H

#include <stdbool.h>
#include "dungeon.h"
#include "config.h"

void create_character(Player** player, Dungeon** dungeon);
void character_card(Player* player, GameTime* time);
void time_menu(Player* player, GameTime* time);
void run_menu(Player** player, Dungeon** dungeon, GameTime* time);


#endif
