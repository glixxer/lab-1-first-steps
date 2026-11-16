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
    player->lantern.oil = LANTERN_MAX_OIL;
    player->lantern.mode = LIGHT_NORMAL;
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
    long long reward = (long long)FLOOR_GOLD_MULTIPLIER * floor;
    if (lantern_is_bright(&player->lantern))
    {
        reward += reward * BRIGHT_GOLD_BONUS_PERCENT / PERCENT_TOTAL;
    }
    if (reward <= INT_MAX - player->gold)
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

bool player_require(const Player* player)
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

bool lantern_is_bright(const Lantern* lantern)
{
    return lantern->mode == LIGHT_BRIGHT && lantern->oil > 0;
}

const char* lantern_mode_name(const Lantern* lantern)
{
    if (lantern->oil == 0 || lantern->mode == LIGHT_OFF) return "Погашен";
    return lantern->mode == LIGHT_BRIGHT ? "Яркий" : "Обычный";
}

void lantern_step(Player* player)
{
    Lantern* lantern = &player->lantern;
    int cost = 0;
    if (lantern->mode == LIGHT_NORMAL) cost = LANTERN_NORMAL_COST;
    if (lantern->mode == LIGHT_BRIGHT) cost = LANTERN_BRIGHT_COST;

    lantern->oil = cost >= lantern->oil ? 0 : lantern->oil - cost;
    if (lantern->oil == 0 && lantern->mode != LIGHT_OFF)
    {
        lantern->mode = LIGHT_OFF;
        puts("Масло закончилось. Фонарь погас.");
    }
    if (lantern->mode == LIGHT_OFF && rand() % PERCENT_TOTAL < DARK_HIT_PERCENT)
    {
        player->hp = player->hp > DARK_HP_DAMAGE ? player->hp - DARK_HP_DAMAGE : 0;
        player_spend_stamina(player, DARK_STAMINA_COST);
        printf("В темноте вы ударились о камни: -%d HP, -%d выносливости.\n",
            DARK_HP_DAMAGE, DARK_STAMINA_COST);
    }
}

void lantern_menu(Player* player)
{
    printf("Фонарь: %s | Масло: %d/%d\n", lantern_mode_name(&player->lantern),
           player->lantern.oil, LANTERN_MAX_OIL);
    printf("[0] Назад\n[1] Погасить\n[2] Обычный (%d масла за шаг)\n"
           "[3] Яркий (%d масла за шаг, +%d%% золота за этаж, видны тайники)\n",
           LANTERN_NORMAL_COST, LANTERN_BRIGHT_COST, BRIGHT_GOLD_BONUS_PERCENT);
    int action;
    if (!read_int("Режим: ", 0, 3, &action) || action == 0) return;
    if (action != 1 && player->lantern.oil == 0)
    {
        puts("Нет масла. Купите его в главном меню.");
        return;
    }
    player->lantern.mode = (LightMode)(action - 1);
    printf("Фонарь: %s.\n", lantern_mode_name(&player->lantern));
}

void lantern_buy_oil(Player* player)
{
    if (!player_require(player)) return;
    int capacity = LANTERN_MAX_OIL - player->lantern.oil;
    int affordable = player->gold / LANTERN_OIL_PRICE;
    printf("Масло: %d/%d. Цена: %d золота за единицу. Золото: %d.\n",
        player->lantern.oil, LANTERN_MAX_OIL, LANTERN_OIL_PRICE, player->gold);
    if (capacity == 0)
    {
        puts("Фонарь уже заправлен.");
        return;
    }
    if (affordable == 0)
    {
        puts("Недостаточно золота.");
        return;
    }
    int limit = capacity < affordable ? capacity : affordable;
    int amount;
    if (!read_int("Сколько масла купить? (0 — отмена): ", 0, limit, &amount) || amount == 0) return;
    player->lantern.oil += amount;
    player->gold -= amount * LANTERN_OIL_PRICE;
    printf("Куплено масла: %d. Запас: %d/%d.\n", amount, player->lantern.oil, LANTERN_MAX_OIL);
}
