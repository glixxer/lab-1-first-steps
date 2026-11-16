#include "dungeon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Dungeon* dungeon_create(int width, int height, unsigned int depth)
{
    if (width < MAP_MIN_SIZE || width > MAP_MAX_SIZE ||
        height < MAP_MIN_SIZE || height > MAP_MAX_SIZE || depth == 0)
    {
        return NULL;
    }
    Dungeon* dungeon = calloc(1, sizeof(*dungeon));
    if (dungeon == NULL) return NULL;
    dungeon->width = width;
    dungeon->height = height;
    dungeon->depth = depth;
    dungeon->cells = calloc((size_t)height, sizeof(*dungeon->cells));
    if (dungeon->cells == NULL)
    {
        dungeon_destroy(dungeon);
        return NULL;
    }
    for (int row = 0; row < height; row++)
    {
        dungeon->cells[row] = malloc((size_t)width * sizeof(**dungeon->cells));
        if (dungeon->cells[row] == NULL)
        {
            dungeon_destroy(dungeon);
            return NULL;
        }
    }
    return dungeon;
}

void dungeon_destroy(Dungeon* dungeon) //освобождение памяти
{
    if (dungeon == NULL) return;
    if (dungeon->cells != NULL)
    {
        for (int row = 0; row < dungeon->height; row++)
        {
            free(dungeon->cells[row]);
        }
        free(dungeon->cells);
    }
    printf("Память структуры по адресу %p освобождена\n", (void*)dungeon);
    free(dungeon);
}

bool dungeon_can_enter(const Dungeon* dungeon, int x, int y)
{
    if (dungeon == NULL || x < 0 || y < 0 || x >= dungeon->width || y >= dungeon->height) return false;
    char tile = dungeon->cells[y][x];
    return tile == TILE_FLOOR || tile == TILE_ICE || tile == TILE_UP ||
        tile == TILE_DOWN || tile == TILE_CACHE;
}

bool dungeon_stairs_reachable(const Dungeon* dungeon)
{
    size_t size = (size_t)dungeon->width * dungeon->height;
    int* queue = malloc(size * sizeof(*queue));
    bool* visited = calloc(size, sizeof(*visited));
    if (queue == NULL || visited == NULL)
    {
        free(queue);
        free(visited);
        return false;
    }
    const int dx[] = { 1, -1, 0, 0 }, dy[] = { 0, 0, 1, -1 };
    int head = 0, tail = 0;
    int start = START_Y * dungeon->width + START_X;
    queue[tail++] = start;
    visited[start] = true;
    bool up_found = false, down_found = false;
    while (head < tail)
    {
        int cell = queue[head++];
        int x = cell % dungeon->width, y = cell / dungeon->width;
        if (dungeon->cells[y][x] == TILE_UP)
        {
            up_found = true;
        }
        if (dungeon->cells[y][x] == TILE_DOWN)
        {
            down_found = true;
        }
        for (size_t direction = 0; direction < sizeof(dx) / sizeof(dx[0]); direction++)
        {
            int nx = x + dx[direction], ny = y + dy[direction];
            if (!dungeon_can_enter(dungeon, nx, ny))
            {
                continue;
            }
            while (dungeon->cells[ny][nx] == TILE_ICE &&
                dungeon_can_enter(dungeon, nx + dx[direction], ny + dy[direction]))
            {
                nx += dx[direction];
                ny += dy[direction];
            }
            int next = ny * dungeon->width + nx;
            if (!visited[next])
            {
                visited[next] = true;
                queue[tail++] = next;
            }
        }
    }
    free(queue);
    free(visited);
    return up_found && down_found;
}

void fill_dungeon(Dungeon* dungeon)
{
    for (int y = 0; y < dungeon->height; y++)
    {
        for (int x = 0; x < dungeon->width; x++)
        {
            char tile = TILE_FLOOR;
            if (x == 0 || y == 0 || x == dungeon->width - 1 || y == dungeon->height - 1)
            {
                tile = TILE_WALL;
            }
            else if (rand() % PERCENT_TOTAL < OBSTACLE_PERCENT)
            {
                tile = rand() % 2 ? TILE_TREE : TILE_ROCK;
            }
            else if (rand() % PERCENT_TOTAL < ICE_PERCENT)
            {
                tile = TILE_ICE;
            }
            else if (rand() % PERCENT_TOTAL < CACHE_PERCENT)
            {
                tile = TILE_CACHE;
            }
            dungeon->cells[y][x] = tile;
        }
    }
    dungeon->cells[START_Y][START_X] = TILE_FLOOR;
    dungeon->cells[START_Y][START_X + 1] = TILE_UP;
    dungeon->cells[dungeon->height - 2][dungeon->width - 2] = TILE_DOWN;
    bool has_ice = false;
    for (int y = 1; y < dungeon->height - 1; y++)
    {
        for (int x = 1; x < dungeon->width - 1; x++)
        {
            if (dungeon->cells[y][x] == TILE_ICE)
            {
                has_ice = true;
            }
        }
    }
    if (!has_ice)
    {
        dungeon->cells[START_Y + 1][START_X] = TILE_ICE;
    }
}

Dungeon* dungeon_generate(unsigned int depth)
{
    int width = MAP_MIN_SIZE + rand() % (MAP_MAX_SIZE - MAP_MIN_SIZE + 1);
    int height = MAP_MIN_SIZE + rand() % (MAP_MAX_SIZE - MAP_MIN_SIZE + 1);
    Dungeon* dungeon = dungeon_create(width, height, depth);
    if (dungeon == NULL) return NULL;
    for (int attempt = 0; attempt < GENERATION_ATTEMPTS; attempt++)
    {
        fill_dungeon(dungeon);
        if (dungeon_stairs_reachable(dungeon)) return dungeon;
    }
    for (int y = START_Y; y < height - 1; y++)
    {
        dungeon->cells[y][START_X] = TILE_FLOOR;
    }
    for (int x = START_X; x < width - 1; x++)
    {
        dungeon->cells[height - 2][x] = TILE_FLOOR;
    }
    dungeon->cells[height - 2][width - 2] = TILE_DOWN;
    dungeon->cells[START_Y + 1][START_X] = TILE_ICE;
    return dungeon;
}

StepResult dungeon_step(Dungeon* dungeon, Player* player, int dx, int dy)
{
    if (player->stamina <= 0 || player->hp <= 0) return STEP_EVACUATE;
    if (!((dx == 0 && (dy == -1 || dy == 1)) ||
        (dy == 0 && (dx == -1 || dx == 1))))
    {
        return STEP_BLOCKED;
    }
    int x = player->x + dx, y = player->y + dy;
    if (!dungeon_can_enter(dungeon, x, y))
    {
        dungeon->sliding = false;
        return STEP_BLOCKED;
    }
    player->x = x;
    player->y = y;
    player_spend_stamina(player, STEP_STAMINA_COST);
    lantern_step(player);
    dungeon->sliding = dungeon->cells[y][x] == TILE_ICE;
    dungeon->slide_dx = dx;
    dungeon->slide_dy = dy;
    if (player->stamina <= 0 || player->hp <= 0) return STEP_EVACUATE;
    if (dungeon->cells[y][x] == TILE_CACHE && lantern_is_bright(&player->lantern))
    {
        player->gold += CACHE_GOLD;
        dungeon->cells[y][x] = TILE_FLOOR;
        printf("Вы нашли тайник: +%d золота.\n", CACHE_GOLD);
    }
    return dungeon->cells[y][x] == TILE_DOWN ? STEP_DESCEND : STEP_MOVED;
}

bool dungeon_descend(Dungeon** dungeon, Player* player, GameTime* time)
{
    if (*dungeon == NULL || time_hours_til_sleep(time) == 0)
    {
        return false;
    }
    unsigned int depth = (*dungeon)->depth + 1;
    dungeon_destroy(*dungeon);
    *dungeon = dungeon_generate(depth);
    if (*dungeon == NULL) return false;
    player->x = START_X;
    player->y = START_Y;
    player_spend_stamina(player, FLOOR_STAMINA_COST);
    (void)time_wait(time, 1);
    player_add_gold(player, depth);
    return true;
}

bool adventure_allowed(const Player* player, const GameTime* time)
{
    if (!player_require(player)) return false;
    if (player->stamina <= 0 || player->hp <= 0)
    {
        puts("Нет сил для похода. Восстановитесь во время сна.");
        return false;
    }
    if (time->hour == SLEEP_HOUR)
    {
        puts("Уже 02:00. Сначала нужно лечь спать.");
        return false;
    }
    return true;
}

static void set_tile_color(char tile, int brightness)
{
    static const unsigned char colors[][3] = {
        [TILE_HERO]   = {255, 228, 166},
        [TILE_WALL]   = {174, 169, 159},
        [TILE_FLOOR]  = {168, 136, 102},
        [TILE_TREE]   = {104, 177, 110},
        [TILE_ROCK]   = {153, 162, 174},
        [TILE_ICE]    = {149, 216, 246},
        [TILE_UP]     = {201, 162, 111},
        [TILE_DOWN]   = {185, 130,  89},
        [TILE_CACHE]  = {255, 195,  72},
        [TILE_HIDDEN] = { 70,  73,  82}
    };
    const unsigned char* rgb = colors[(unsigned char)tile];
    printf("\x1b[38;2;%d;%d;%dm", rgb[0] * brightness / PERCENT_TOTAL,
        rgb[1] * brightness / PERCENT_TOTAL, rgb[2] * brightness / PERCENT_TOTAL);
}

static void dungeon_draw(Dungeon* dungeon, Player* player, GameTime* time, bool use_colors)
{
    const Lantern* lantern = &player->lantern;
    bool bright = lantern_is_bright(lantern);
    int radius = LANTERN_OFF_RADIUS;
    if (lantern->oil > 0 && lantern->mode == LIGHT_NORMAL) radius = LANTERN_NORMAL_RADIUS;
    if (bright) radius = LANTERN_BRIGHT_RADIUS;
    int radius_squared = radius * radius;
    const int edge_dimming = 35;

    printf("\nHP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %u | Время: %02d:00\n",
        player->hp, player->max_hp, player->stamina, player->max_stamina,
        player->gold, dungeon->depth, time->hour);
    printf("Фонарь: %s | Масло: %d/%d | Видимость: %d\n",
        lantern_mode_name(lantern), lantern->oil, LANTERN_MAX_OIL, radius);
    puts("@ — герой | ░ — темнота | #, T, R — препятствия | . — пол | ~ — лёд | U — домой | D — вниз | $ — тайник");
    printf("Подземелье. Глубина: %u. Карта: %dx%d\n", dungeon->depth, dungeon->width, dungeon->height);
    for (int y = 0; y < dungeon->height; y++)
    {
        for (int x = 0; x < dungeon->width; x++)
        {
            int dx = x - player->x, dy = y - player->y;
            int distance_squared = dx * dx + dy * dy;
            char tile = dungeon->cells[y][x];
            if (tile == TILE_CACHE && !bright) tile = TILE_FLOOR;
            if (distance_squared > radius_squared) tile = TILE_HIDDEN;
            if (distance_squared == 0) tile = TILE_HERO;
            if (use_colors)
            {
                int brightness = PERCENT_TOTAL;
                if (tile != TILE_HIDDEN) brightness -= edge_dimming * distance_squared / radius_squared;
                set_tile_color(tile, brightness);
            }
            if (tile == TILE_HIDDEN) fputs("░", stdout);
            else putchar(tile);
        }
        putchar('\n');
    }
    if (use_colors) fputs("\x1b[0m", stdout);
}

void adventure_run(Dungeon** dungeon, Player* player, GameTime* time)
{
    if (!adventure_allowed(player, time)) return;
    dungeon_destroy(*dungeon);
    *dungeon = dungeon_generate(1);
    if (*dungeon == NULL)
    {
        puts("Не удалось выделить память для карты.");
        return;
    }
    player->x = START_X;
    player->y = START_Y;
    bool use_colors = terminal_enable_colors();
    while (true)
    {
        if (player->stamina <= 0 || player->hp <= 0 || time->hour == SLEEP_HOUR)
        {
            player_evacuate(player);
            break;
        }
        dungeon_draw(*dungeon, player, time, use_colors);
        int dx = (*dungeon)->slide_dx, dy = (*dungeon)->slide_dy;
        if ((*dungeon)->sliding)
        {
            puts("Скользкий лёд! Следующий шаг — в прежнем направлении.");
        }
        else
        {
            char command;
            if (!read_command(&command))
            {
                break;
            }
            if (command == 'L')
            {
                lantern_menu(player);
                continue;
            }
            if (command == 'Q')
            {
                if ((*dungeon)->cells[player->y][player->x] == TILE_UP)
                {
                    puts("Вы вернулись домой. Заработанное золото сохранено.");
                    break;
                }
                puts("Домой можно вернуться только с лестницы U.");
                continue;
            }
            dx = command == 'D' ? 1 : command == 'A' ? -1
                : 0;
            dy = command == 'S' ? 1 : command == 'W' ? -1
                : 0;
        }
        StepResult result = dungeon_step(*dungeon, player, dx, dy);
        if (result == STEP_BLOCKED)
        {
            puts("Путь закрыт стеной или препятствием.");
            continue;
        }
        if (result == STEP_DESCEND && !dungeon_descend(dungeon, player, time))
        {
            puts("Не удалось выделить память для нового этажа.");
            break;
        }
    }
    dungeon_destroy(*dungeon);
    *dungeon = NULL;
    player->x = START_X;
    player->y = START_Y;
}
