#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <Windows.h> 
    #define _CRT_SECURE_NO_WARNINGS
    #pragma warning(disable : 4996)
#else
    #include <locale.h>
#endif

#define MAX_NAME_LENGTH 33

#define MAX_ITEMS 10
#define MAX_NAME_LEN 32
#define INVENTORY_SIZE 10
#define EMPTY 0
#define WOOD 1
#define STONE 2
#define SEEDS 3
#define WATERING_CAN 4
#define HOE 5
#define FERTILIZER 6
#define WHEAT 7
#define CARROT 8
#define POTATO 9

char farmer_name[MAX_NAME_LENGTH];
int current_day = 1;
int current_hour = 8;
char item_names[MAX_ITEMS][MAX_NAME_LEN] = { 0 };
int inventory[INVENTORY_SIZE] = { 
    EMPTY,
    WOOD,
    STONE,
    SEEDS,
    SEEDS,
    SEEDS,
    HOE,
    FERTILIZER,
    WATERING_CAN,
    WHEAT
};

char* get_item_name(int id);
void print_menu(void);
void clear_buffer(void);
int read_input(int err);
void check_time(void);
void forward_time(void);
void check_inventory(void);
void put_item(void);
void delete_item(void);
void favorite_resource(void);
void get_farmer_name(void);
int load_inventory(void);
void remove_newline(char* str);
void pause(void);
void write_diary(void);

int main()
{
#ifdef _WIN32
    SetConsoleCP(CP_UTF8); 
    SetConsoleOutputCP(CP_UTF8); 
#else
    setlocale(LC_ALL, "");
#endif
    get_farmer_name();
    printf("Привет, %s! Добро пожаловать в игру!\n", farmer_name);

    if (load_inventory())
    {
        for (int i = 0; i < INVENTORY_SIZE; i++)
        {
            inventory[i] = item_names[i][0];
        }
    }
    
    while (1)
    {
        print_menu();
        int response = read_input(0);
        switch (response)
        {
        case 0:
            printf("Выход из программы\n");
            return 0;
        case 1:
            check_time();
            break;
        case 2:
            forward_time();
            break;
        case 3:
            check_inventory();
            break;
        case 4:
            put_item();
            break;
        case 5:
            delete_item();
            break;
        case 6:
            favorite_resource();
            break;
        case 7:
            search_item();
            break;
        case 8:
            write_diary();
            break;
        case 9:
            
            break;
        default:
            printf("Введите целое число от 0 до 9\n");
        }
    }
    return 0;
}

char* get_item_name(int id)
{
    switch (id)
    {
        case 0:
            return "Пусто";
        case 1:
            return "Дерево";
        case 2:
            return "Камень";
        case 3:
            return "Семена";
        case 4:
            return "Лейка";
        case 5:
            return "Мотыга";
        case 6:
            return "Удобрение";
        case 7:
            return "Пшеница";
        case 8:
            return "Морковь";
        case 9:
            return "Картошка";
        default:
            return "NULL";
    }
}

void print_menu(void)
{
    printf("\n[0] Выход\n");
    printf("[1] Посмотреть на часы\n");
    printf("[2] Промотать время\n");
    printf("[3] Посмотреть инвентарь\n");
    printf("[4] Положить предмет в слот\n");
    printf("[5] Выбросить предмет\n");
    printf("[6] Просмотр любимого ресурса\n");
    printf("[7] Поиск предмета в рюкзаке по названию \n");
    printf("[8] Записать состояние в дневник фермера (diary.txt) \n");
    printf("[9] Расшифровать старые записи (Задание по варианту) DEP//\n");
}

void clear_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return;
}

int read_input(int err) // 1 - с ошибкой для дебила, 0 - без ошибки
{
    while (1)
    {
        int response = -1;
        int res_check = scanf("%d", &response);
        if (res_check != 1 && err)
        {
            printf("Введите целое число!\n");
            clear_buffer();
        }
        else
        {
            clear_buffer();
            return response;
        }
    } 
}

void check_time(void)
{
    printf("Текущее время: День %d, %02d:00\n", current_day, current_hour);
    return;
}

void forward_time(void)
{
    printf("Сколько часов перемотать?\n");
    while (1)
    {
        int response = read_input(1);
        if (response < 0)
        {
            printf("Количество часов не может быть отрицательным!\n");
        }
        else
        {
            current_hour += response;
            while (current_hour >= 24)
            {
                current_hour -= 24;
                current_day++;
            }
            printf("Промотано %d час(а)\n", response);
            check_time();
            return;
        }
    }
}

void check_inventory(void)
{
    for (int item = 0; item < INVENTORY_SIZE; item++)
    {
        printf("Слот %d: [%d] (%s)\n", item, inventory[item], get_item_name(inventory[item]));
    }
    return;
}

void put_item(void)
{
    printf("В какой слот положить предмет?\n\n");
    check_inventory();
    while (1)
    {
        int slot = read_input(1);
        if (slot < 0 || slot > 9)
        {
            printf("Слот выходит за границы!\n");
        }
        else
        {
            printf("Какой предмет положить?\n\n");
            for (int i = 0; i < INVENTORY_SIZE; i++)
            {
                printf("%d - %s\n", i, get_item_name(i));
            }
            while (1)
            {
                int item = read_input(1);
                if (item < 0 || item > 9)
                {
                    printf("Предмет выходит за границы!\n");
                }
                else
                {
                    inventory[slot] = item;
                    printf("Слот заменен!\n\n");
                    return;
                }
            }
        }
    }
}

void delete_item(void)
{
    printf("Какой слот выбросить?\n");
    check_inventory();
    while (1)
    {
        int slot = read_input(1);
        if (slot > 9 || slot < 0)
        {
            printf("Слот выходит за границы!\n");
        }
        else
        {     
          inventory[slot] = 0;
          printf("Предмет выброшен!\n\n");
          return;         
        }
    }
}

void favorite_resource(void) 
{ 
    int count[INVENTORY_SIZE] = { 0 }; 
    int favorite_id = 0; 
    int max_count = 0; 
    for (int i = 0; i < INVENTORY_SIZE; i++) 
    { 
        if (inventory[i] != 0) 
        { 
            count[inventory[i]]++; 
        } 
    } 
    for (int i = 1; i < INVENTORY_SIZE; i++) 
    { 
        if (count[i] > max_count)
        { 
            max_count = count[i];
            favorite_id = i;
        } 
    } 
    if (max_count == 0)
    { 
        printf("Инвентарь пуст\n\n"); 
    } 
    else 
    { 
        printf("Любимый ресурс: %d - %s, он встречается %d раз(а)\n\n", favorite_id, get_item_name(favorite_id), max_count);
        return;
    } 
}

void get_farmer_name(void)
{
    char input[256];
    while (1)
    {
        printf("Введите имя персонажа: \n");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("Ошибка ввода. Повторите попытку.\n");
            continue;
        }

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
        {
            input[len - 1] = '\0';
            len--;
        }
        else
        {
            int ch;
            int overflow = 0;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                overflow = 1;
            }
            if (overflow)
            {
                printf("Имя слишком длинное. Максимум %d символов.\n", MAX_NAME_LENGTH - 1);
                continue;
            }
        }

        if (len == 0)
        {
            printf("Имя не может быть пустым. Попробуйте ещё раз.\n");
            continue;
        }

        if (len > MAX_NAME_LENGTH - 1)
        {
            printf("Имя слишком длинное. Максимум %d символов.\n", MAX_NAME_LENGTH - 1);
            continue;
        }

        strncpy(farmer_name, input, len + 1);
        return;
    }
}

int load_inventory(void)
{
    FILE* file = fopen("items.txt", "r");
    if (file == NULL)
    {
        printf("Файл items.txt не найден.\n Используется инвентарь по умолчанию.\n");
        return 0;
    }
    char line[MAX_NAME_LEN];
    int loaded = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        int id;
        char name[MAX_NAME_LEN];

        sscanf(line, "%d %s", &id, name);
        item_names[loaded][0] = id;
        for (int i = 1; i < MAX_NAME_LEN + 1; i++)
        {
            item_names[loaded][i] = name[i - 1];
        }
        loaded++;
    }
    fclose(file);
    return 1;
}

int search_item(void)
{
    char buffer[MAX_NAME_LEN];
    int item_id = -1;
    int found = 0;

    printf("Введите название предмета для поиска: \n");
    fgets(buffer, sizeof(buffer), stdin);
    remove_newline(buffer);

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (strcmp(buffer, get_item_name(i)) == 0)
        {
            item_id = i;
            break;
        }
    }

    if (item_id == -1)
    {
        printf("Предмет не найден.\n");
        pause();
        return;
    }

    char item_name[MAX_NAME_LEN];
    strncpy(item_name, get_item_name(item_id), MAX_NAME_LEN - 1);
    item_name[MAX_NAME_LEN - 1] = '\0';

    printf("Предмет %s находится в: \n", item_name);    

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        if (inventory[i] == item_id)
        {
            printf("Слоте %d.\n", i);
        }
    }
    pause();
    return;
}

void remove_newline(char* str)
{
    str[strcspn(str, "\r\n")] = '\0';
}

void pause(void)
{ 
    printf("\nНажмите Enter для продолжения...");
    getchar();
}

void write_diary(void)
{
    FILE* file = fopen("diary.txt", "a");
    if (file == NULL)
    {
        printf("Не удалось открыть diary.txt для записи\n");
        return;
    }

    fprintf(file, "===== Дневник фермера =====\n");
    fprintf(file, "Фермер: %s\n", farmer_name);
    fprintf(file, "День: %d, час: %02d:00\n", current_day, current_hour);
    fprintf(file, "Инвентарь:\n");

    for (int i = 0; i < INVENTORY_SIZE; i++)
    {
        fprintf(file, "Слот %d: [%d] - %s\n", i, inventory[i], get_item_name(inventory[i]));
    }     
    fprintf(file, "===========================\n\n");

    fclose(file);
    printf("Состояние записано в diary.txt\n");
    pause();
    return;
}