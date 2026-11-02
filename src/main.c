#include "dungeon.h"
#include "mainUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <locale.h>
#endif


int main(void)
{
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#else
    setlocale(LC_ALL, "");
#endif

    srand((unsigned int)time(NULL));
    Player* player = NULL;
    Dungeon* dungeon = NULL;
    GameTime time = { 1, MORNING_HOUR };
    run_menu(&player, &dungeon, &time);
    dungeon_destroy(dungeon);
    player_destroy(player);
    return 0;
}
