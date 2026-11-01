#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include "player.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

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