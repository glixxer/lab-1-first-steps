#include "player.h"
#include "utils.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Player* player_create(char* name)
{
    size_t len = strlen(name);
    if (!len || len >= PLAYER_NAME_SIZE) return NULL;

    Player* player = calloc(1, sizeof(*player));
    if (player == NULL) return NULL;

    name[strcspn(name, "\r\n")] = '\0';
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
        puts("Память структуры не нуждается в очистке\n");
    }
    else
    {
        printf("Память структуры %p очищена\n", (void*)player);
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

bool player_require(Player* player)
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
    if (!player_require(player)) return;

    for (int item = 0; item < INVENTORY_SIZE; item++)
    {
        printf("Слот %d: [%d] (%s)\n", item, player->inventory[item], inventory_get_item_name(player, player->inventory[item]));
    }
}

void inventory_put_item(Player* player)
{
    if (!player_require(player)) return;
    inventory_check(player);
    int slot, item;
    if (!read_int("В какой слот положить предмет?\n", 0, INVENTORY_SIZE - 1, &slot)) return;
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        printf("%d — %s\n", i, inventory_get_item_name(player, i));
    }
    if (!read_int("Какой предмет положить?\n", EMPTY, MAX_ITEMS - 1, &item)) return;
    player->inventory[slot] = item;
    puts("Слот заменён.");
}

void inventory_delete(Player* player)
{
    if (!player_require(player)) return;
    inventory_check(player);
    int slot;
    if (read_int("Из какого слота выбросить предмет?\n", 0, INVENTORY_SIZE - 1, &slot))
    {
        player->inventory[slot] = EMPTY;
        puts("Предмет выброшен.");
    }
}

void inventory_favorite(Player* player)
{
    if (!player_require(player)) return;
    int counts[MAX_ITEMS] = { 0 };
    int favorite = EMPTY;
    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        int id = player->inventory[i];
        if (id > EMPTY && id < MAX_ITEMS)
        {
            ++counts[id];
        }
    }
    for (int i = 1; i < MAX_ITEMS; i++)
    {
        if (counts[i] > counts[favorite])
        {
            favorite = i;
        }
    }
    if (favorite == EMPTY)
    {
        puts("Инвентарь пуст.");
    }
    else
    {
        printf("Любимый ресурс: %s, встречается %d раз(а).\n", inventory_get_item_name(player, favorite), counts[favorite]);
    }
}

void inventory_search(Player* player)
{
    if (!player_require(player)) return;
    char query[ITEM_NAME_SIZE];
    puts("Введите название предмета для поиска:");
    bool status = read_line(query, sizeof(query));
    if (status == false)
    {
        if (feof(stdin) || ferror(stdin)) return;
        puts("Название слишком длинное.");
        return;
    }
    bool found = false;
    for (int slot = 0; slot < INVENTORY_SIZE; slot++)
    {
        if (!strcmp(query, inventory_get_item_name(player, player->inventory[slot])))
        {
            printf("Предмет %s находится в слоте %d.\n", query, slot);
            found = true;
        }
    }
    if (!found)
    {
        puts("Предмет не найден в инвентаре.");
    }
}
