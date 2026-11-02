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

bool dungeon_can_enter(Dungeon* dungeon, int x, int y)
{
    if (dungeon == NULL || x < 0 || y < 0 || x >= dungeon->width || y >= dungeon->height) return false;
    char tile = dungeon->cells[y][x];
    return tile == TILE_FLOOR || tile == TILE_ICE || tile == TILE_UP || tile == TILE_DOWN;
}

bool dungeon_stairs_reachable(Dungeon* dungeon)
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

