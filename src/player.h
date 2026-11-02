#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "config.h"

typedef struct Player {
    char name[PLAYER_NAME_SIZE];
    int hp, max_hp;
    int stamina, max_stamina;
    int gold;
    int x, y;
    int inventory[INVENTORY_SIZE];
    char item_names[MAX_ITEMS][ITEM_NAME_SIZE];
} Player;

Player* player_create(char* name);
void player_destroy(Player* player);
void player_restore(Player* player);
void player_spend_stamina(Player* player, int amount);
void player_add_gold(Player* player, int floor);
void player_evacuate(Player* player);
bool player_require(Player* player);
char* inventory_get_item_name(Player* player, int id);
void inventory_check(Player* player);
void inventory_put_item(Player* player);
void inventory_delete(Player* player);
void inventory_favorite(Player* player);
void inventory_search(Player* player);
#endif
