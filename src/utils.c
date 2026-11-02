#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include "player.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <conio.h>
    #include <io.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

//чтение ввода
bool read_line(char* buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) == NULL) return false;
    if (strchr(buffer, '\n') == NULL && !feof(stdin))
    {
        int ch = getchar();
        if (ch != '\n' && ch != EOF)
        {
            while ((ch = getchar()) != '\n' && ch != EOF);
            return false;
        }
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';
    return true;
}

bool read_int(char* prompt, int min, int max, int* value)
{
    char buffer[INPUT_SIZE];
    while (true)
    {
        fputs(prompt, stdout);
        bool status = read_line(buffer, sizeof(buffer));
        if (status == false)
        {
            puts("Ввод слишком длинный");
            continue;
        } 
        char* end;
        long parsed = strtol(buffer, &end, 10);
        errno = 0;
        bool has_number = end != buffer;
        while (isspace((unsigned char)*end))
            ++end;
        if (has_number && !*end && errno != ERANGE && parsed >= min && parsed <= max) {
            *value = (int)parsed;
            return true;
        }
        printf("Введите целое число от %d до %d.\n", min, max);
    }
}

int read_key(void)
{
#ifdef _WIN32
    if (_isatty(_fileno(stdin)))
    {
        int key = _getwch();
        if (key == 0 || key == 224)
        {
            _getwch();
            return 0;
        }
        switch (key)
        {
        case L'\u0446': case L'\u0426': return 'W'; // ц, Ц
        case L'\u0444': case L'\u0424': return 'A'; // ф, Ф
        case L'\u044B': case L'\u042B': return 'S'; // ы, Ы
        case L'\u0432': case L'\u0412': return 'D'; // в, В
        case L'\u0439': case L'\u0419': return 'Q'; // й, Й
        }
        if (key == 3 || key == 26) return EOF;
        if (key > 127) return 0;
        return key;
    }
#else
    if (isatty(STDIN_FILENO))
    {
        struct termios saved, mode;
        if (tcgetattr(STDIN_FILENO, &saved) != 0) return EOF;
        mode = saved;
        mode.c_lflag &= (tcflag_t)~(ICANON | ECHO);
        mode.c_cc[VMIN] = 1;
        mode.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &mode) != 0) return EOF;
        int key = getchar();
        if (tcsetattr(STDIN_FILENO, TCSANOW, &saved) != 0) return EOF;
        return key;
    }
#endif
    return getchar();
}

bool read_command(char* command) //ввод в подземелье
{
    puts("W/A/S/D — движение, Q — домой (только на U). Enter не нужен.");
    while (true)
    {
        int key = read_key();
        if (key == EOF) return false;
        if (isspace((unsigned char)key)) continue;
        key = toupper((unsigned char)key);
        if (key != 0 && strchr("WASDQ", key))
        {
            *command = (char)key;
            return true;
        }
        puts("Неизвестная команда. Введите одну букву W, A, S, D или Q.");
    }
}

//работа со временем
void time_print(GameTime* time)
{
    printf("Текущее время: День %d, %02d:00\n", time->day, time->hour);
}

int time_hours_til_sleep(GameTime* time)
{
    return (SLEEP_HOUR - time->hour + HOURS_PER_DAY) % HOURS_PER_DAY;
}

bool time_wait(GameTime* time, int hours)
{
    if (hours < 1 || hours > time_hours_til_sleep(time))
    {
        puts("Нельзя ждать дольше 02:00, ложитесь спать!");
        return false;
    }
    int total = time->hour + hours;
    time->day += total / HOURS_PER_DAY;
    time->hour = total % HOURS_PER_DAY;
    return true;
}

bool time_sleep(GameTime* time)
{
    time->day++;
    time->hour = MORNING_HOUR;
    return true;
}

//работа с файлами
bool load_inventory(Player* player)
{
    FILE* file = fopen("items.txt", "r");
    if (file == NULL)
    {
        printf("Файл items.txt не найден.\n Используется инвентарь по умолчанию.\n");
        return false;
    }
    char line[256];
    int loaded = 0;
    int loaded_inventory[INVENTORY_SIZE] = { EMPTY };
    while (loaded < INVENTORY_SIZE && fgets(line, sizeof(line), file) != NULL)
    {
        int id;
        char name[ITEM_NAME_SIZE];

        if (sscanf(line, "%d %31s", &id, name) != 2 ||
            id < EMPTY || id >= MAX_ITEMS)
        {
            printf("Некорректная строка в items.txt пропущена.\n");
            continue;
        }
        loaded_inventory[loaded] = id;
        strcpy(player->item_names[id], name);
        loaded++;
    }
    fclose(file);
    if (loaded == 0)
    {
        printf("В items.txt нет подходящих предметов. Используется инвентарь по умолчанию.\n");
        return false;
    }
    memcpy(player->inventory, loaded_inventory, sizeof(player->inventory));
    return true;
}

void inventory_default_init(Player* player)
{
    char* inventory[MAX_ITEMS] = {
        "Пусто", "Дерево", "Камень", "Семена", "Лейка", "Мотыга",
        "Удобрение", "Пшеница", "Морковь", "Картошка"
    };
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        strcpy(player->item_names[i], inventory[i]);
    }
}

void write_diary(Player* player, GameTime* time)
{
    if (!player_require(player)) return;
    FILE* file = fopen("diary.txt", "a");
    if (file == NULL)
    {
        printf("Не удалось открыть diary.txt для записи\n");
        return;
    }

    fprintf(file, "===== Дневник фермера =====\n");
    fprintf(file, "Фермер: %s\n", player->name);
    fprintf(file, "HP: %d/%d | Выносливость: %d/%d | Золото: %d\n",
        player->hp, player->max_hp, player->stamina, player->max_stamina, player->gold);
    fprintf(file, "День: %d, час: %02d:00\n", time->day, time->hour);
    fprintf(file, "Инвентарь:\n");

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        fprintf(file, "Слот %d: [%d] - %s\n", i, player->inventory[i], inventory_get_item_name(player, player->inventory[i]));
    }
    fprintf(file, "===========================\n\n");
    fclose(file);
    printf("Состояние записано в diary.txt\n");
}

void npc_shedule(GameTime* time)
{
    typedef struct
    {
        char name[NPC_NAME_SIZE];
        char location[PLAYER_NAME_SIZE];
        int found;
    } NPC;

    NPC npcs[NPC_LIMIT] = { "","",0 };

    FILE* input = fopen("input.txt", "r");
    if (input == NULL)
    {
        printf("Не удалось открыть input.txt для чтения\n");
        return;
    }

    FILE* output = fopen("output.txt", "w");
    if (output == NULL)
    {
        printf("Не удалось открыть output.txt для записи\n");
        return;
    }

    char buffer[256];
    int npc_count = 0;
    while (fgets(buffer, sizeof(buffer), input) != NULL)
    {
        char name[NPC_NAME_SIZE];
        char location[PLAYER_NAME_SIZE];
        int start_hour, end_hour;

        sscanf(buffer, "%63[^;];%d;%d;%63[^\n]", name, &start_hour, &end_hour, location);

        if (time->hour > start_hour && time->hour < end_hour && npcs[npc_count].found == 0)
        {
            strcpy(npcs[npc_count].name, name);
            strcpy(npcs[npc_count].location, location);
            npcs[npc_count].found = 1;
        }
        if (strchr(location, ',') != NULL && npcs[npc_count].found == 0)
        {
            if (!(strcmp(npcs[npc_count - 1].name, name) == 0))
            {
                strcpy(npcs[npc_count].name, name);
                strcpy(npcs[npc_count].location, "Спит дома");
                npcs[npc_count].found = 1;
            }
        }
        if (npcs[npc_count].found == 1) npc_count++;
    }
    fclose(input);
    fprintf(output, "==== Сводка состояния жителей на день: %d, час: %02d:00 ====\n", time->day, time->hour);
    for (int i = 0; i < NPC_LIMIT; i++)
    {
        if (npcs[i].found && npcs[i].name != '\n')
        {
            printf("%s, локация: %s\n", npcs[i].name, npcs[i].location);
            fprintf(output, "%s, локация: %s\n", npcs[i].name, npcs[i].location);
        }
    }
    fclose(output);
}

