#include "dungeon.h"
#include <stdlib.h>
#include <stdbool.h>

int get_random_number(int min, int max) //случайное число
{
    return rand() % (max - min + 1) + min;
}

void free_map(char **map, int height) // освобождение памяти для карты
{
    if (map == NULL) return;
    for (int i = 0; i < height; i++) 
    {
        free(map[i]);
    }
free(map);
}

char** allocate_map(int width, int height) // выделение памяти для карты
{
    char** map = (char**)malloc(width * sizeof(char));
    if (map == NULL) free_map(map, height); return NULL;

    for (size_t row = 0; row < height; row++)
    {
        map[row] = (char*)malloc(width * sizeof(char));
        if (map[row] == NULL) free_map(map, height); return NULL;
    }
    return map;
}

