#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "config.h"

typedef enum LightMode {
    LIGHT_OFF,
    LIGHT_NORMAL,
    LIGHT_BRIGHT
} LightMode;

typedef struct Lantern {
    int oil;
    LightMode mode;
} Lantern;

typedef struct Player {
    char name[PLAYER_NAME_SIZE];
    int hp, max_hp;
    int stamina, max_stamina;
    int gold;
    int x, y;
    Lantern lantern;
    int inventory[INVENTORY_SIZE];
    char item_names[MAX_ITEMS][ITEM_NAME_SIZE];
} Player;

Player* player_create(char* name);
void player_destroy(Player* player);
void player_restore(Player* player);
void player_spend_stamina(Player* player, int amount);
void player_add_gold(Player* player, int floor);
void player_evacuate(Player* player);
bool player_require(const Player* player);
bool lantern_is_bright(const Lantern* lantern);
const char* lantern_mode_name(const Lantern* lantern);
void lantern_step(Player* player);
void lantern_menu(Player* player);
void lantern_buy_oil(Player* player);
char* inventory_get_item_name(Player* player, int id);
void inventory_check(Player* player);
void inventory_put_item(Player* player);
void inventory_delete(Player* player);
void inventory_favorite(Player* player);
void inventory_search(Player* player);
#endif
