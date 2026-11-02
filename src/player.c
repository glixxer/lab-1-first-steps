#include "player.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Player* player_create(char* name)
{
    size_t len = strlen(name);
    if (!len || len >= PLAYER_NAME_SIZE) return NULL;

    Player* player = calloc(1, sizeof(*player));
    if (player == NULL) return NULL;

    memcpy(player->name, name, len + 1);
    player->max_hp = START_HP;
    player->hp = START_HP;
    player->max_stamina = START_STAMINA;
    player->stamina = START_STAMINA;
    player->gold = START_GOLD;
    player->x = START_X;
    player->y = START_Y;
    int start_inventory[INVENTORY_SIZE] = {
        EMPTY, WOOD, STONE, SEEDS, SEEDS, SEEDS, HOE, FERTILIZER, WATERING_CAN, WHEAT
    };
    memcpy(player->inventory, start_inventory, sizeof(start_inventory));

    return player;
}

void player_destroy(Player* player)
{
    if (player == NULL) 
    {
        printf("Память структуры не нуждается в очистке");
    }
    else
    {
        printf("Память структуры %p очищена", (void*)player);
        free(player);
    }
}

void player_restore(Player* player)
{
    player->hp = player->max_hp;
    player->stamina = player->max_stamina;
}

void player_spend_stamina(Player* player, int amount)
{
    player->stamina = amount >= player->stamina ? 0 : player->stamina - amount;
}

void player_add_gold(Player* player, int floor)
{
    long reward = FLOOR_GOLD_MULTIPLIER * floor;
    if (reward < INT_MAX)
    {
        player->gold += (int)reward;
    }
}

void player_evacuate(Player* player)
{
    int pay = (int)(player->gold / 2);
    player->gold -= pay;
    player->x = START_X;
    player->y = START_Y;
    printf("Герой упал в обморок, его нашли спасатели. Плата: %d золота.\n", pay);
}

bool player_requare(Player* player)
{
    if (player != NULL) return true;
    puts("Сначала создайте персонажа!");
    return false;
}

char* inventory_get_item_name(Player* player, int id)
{
    return id < MAX_ITEMS ? player->item_names[id] : "Неизвестный предмет";
}

void inventory_check(Player* player)
{
    if (!player_requare) return;

    for (int item = 0; item < INVENTORY_SIZE; item++)
    {
        printf("Слот %d: [%d] (%s)\n", item, player->inventory, get_item_name(player, item));
    }
}

void inventory_put_item()
{
    if (!player_require) return;
    //
}


