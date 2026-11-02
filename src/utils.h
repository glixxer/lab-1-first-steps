#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "player.h"

typedef struct {
    int day;
    int hour;
} GameTime;

bool read_line(char* buffer, size_t size);
bool read_int(char* prompt, int min, int max, int* value);
int read_key(void);
bool read_command(char* command);
void time_print(GameTime* time);
int time_hours_til_sleep(GameTime* time);
bool time_wait(GameTime* time, int hours);
bool time_sleep(GameTime* time);
bool load_inventory(Player* player);
void inventory_default_init(Player* player);
void write_diary(Player* player, GameTime* time);
void npc_shedule(GameTime* time);
#endif
