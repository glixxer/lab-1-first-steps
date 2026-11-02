#include "dungeon.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

static void print_menu(void)
{
    puts("\n[0] Выход\n"
        "[1] Создать / пересоздать персонажа\n"
        "[2] Карточка персонажа\n"
        "[3] Посмотреть инвентарь\n"
        "[4] Положить предмет в слот\n"
        "[5] Выбросить предмет\n"
        "[6] Любимый ресурс\n"
        "[7] Поиск предмета\n"
        "[8] Дневник фермера\n"
        "[9] Расписание жителей\n"
        "[10] Отправиться в приключение\n"
        "[11] Время и отдых");
}

void create_character(Player** player, Dungeon** dungeon)
{
    if ((*player) != NULL)
    {
        int confirm;
        if (!read_int("Удалить текущего персонажа? 1 — да, 0 — нет: ", 0, 1, &confirm) || !confirm) return;
        dungeon_destroy((*dungeon));
        (*dungeon) = NULL;
        player_destroy((*player));
        (*player) = NULL;
    }
    char name[PLAYER_NAME_SIZE];
    while (true)
    {
        puts("Введите имя персонажа:");
        bool status = read_line(name, sizeof(name));
        if (status == false)
        {
            if (feof(stdin) || ferror(stdin)) return;
            printf("Имя слишком длинное. Максимум %d символов.\n", PLAYER_NAME_SIZE - 1);
            continue;
        }
        bool has_text = false;
        for (size_t i = 0; name[i]; i++)
        {
            if (!isspace((unsigned char)name[i]))
            {
                has_text = true;
            }
        }
        if (!has_text)
        {
            puts("Имя не может быть пустым.");
            continue;
        }
        break;
    }
    (*player) = player_create(name);
    if ((*player) == NULL)
    {
        puts("Не удалось выделить память для персонажа.");
        return;
    }
    inventory_default_init(*player);
    load_inventory(*player);
    printf("Персонаж %s создан.\n", (*player)->name);
}

void character_card(const Player* player, const GameTime* time)
{
    if (!player_require(player)) return;
    printf("\n===== Фермер: %s =====\nHP: %d/%d\nВыносливость: %d/%d\nЗолото: %d\n",
        player->name, player->hp, player->max_hp, player->stamina, player->max_stamina, player->gold);
    time_print(time);
    inventory_check(player);
}

void time_menu(Player* player, GameTime* time)
{
    if (!player_require(player)) return;
    while (true)
    {
        puts("\nВремя и отдых:\n[0] Назад\n[1] Посмотреть на часы\n[2] Ждать\n[3] Лечь спать");
        int action;
        if (!read_int("Действие: ", 0, 3, &action) || action == 0) return;
        switch (action)
        {
        case 1:
            time_print(time);
            break;
        case 2:
        {
            int hours;
            int limit = time_hours_til_sleep(time);
            if (!limit)
            {
                puts("Уже 02:00. Нужно лечь спать.");
                break;
            }
            printf("До ближайших 02:00 можно ждать не больше %d час(ов).\n", limit);
            if (!read_int("Сколько часов ждать? ", 1, HOURS_PER_DAY, &hours)) return;
            if (!time_wait(time, hours))
            {
                puts("Нельзя ждать через границу 02:00.");
            }
            time_print(time);
            break;
        }
        case 3:
            if (!player_require(player))
            {
                break;
            }
            time_sleep(time);
            player_restore(player);
            puts("Вы выспались. Здоровье и выносливость полностью восстановлены.");
            time_print(time);
            break;
        }
    }
}

void run_menu(Player** player, Dungeon** dungeon, GameTime* time)
{
    while (true)
    {
        print_menu();
        int action;
        if (!read_int("Действие: ", MENU_EXIT, MENU_TIME, &action)) return;
        switch ((MenuAction)action)
        {
        case MENU_EXIT:
            puts("Выход из программы.");
            return;
        case MENU_CREATE:
            create_character(player, dungeon);
            break;
        case MENU_CARD:
            character_card(*player, time);
            break;
        case MENU_INVENTORY:
            inventory_check(*player);
            break;
        case MENU_PUT:
            inventory_put_item(*player);
            break;
        case MENU_DELETE:
            inventory_delete(*player);
            break;
        case MENU_FAVORITE:
            inventory_favorite(*player);
            break;
        case MENU_SEARCH:
            inventory_search(*player);
            break;
        case MENU_DIARY:
            write_diary(*player, time);
            break;
        case MENU_NPC:
            npc_shedule(time);
            break;
        case MENU_ADVENTURE:
            adventure_run(dungeon, *player, time);
            break;
        case MENU_TIME:
            time_menu(*player, time);
            break;
        }
    }
}
