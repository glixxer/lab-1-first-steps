#include <stdio.h>
#include <Windows.h> //del on linux

#define _CRT_SECURE_NO_WARNINGS // ))))
#define INVENTORY_SIZE 10
int current_day = 1;
int current_hour = 8;
int inventory[INVENTORY_SIZE] = { 0, 1, 2, 3, 3, 3, 5, 6, 4, 7 };

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
    printf("[6] Просмотр любимого ресурса\n\n");
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

void clear_console(void)
{
    printf("\033[H\033[J");
    return;
}

void check_time(void)
{
    if (current_hour < 10)
    {
        printf("Текущее время: День %d, 0%d:00\n", current_day, current_hour);
    }
    else
    {
        printf("Текущее время: День %d, %d:00\n", current_day, current_hour);
    }
    return;
}

void forward_time(void)
{
    printf("Сколько часов перемотать?\n");
    while (1)
    {
        int response = read_input(1);
        if (response > 120)
        {
            printf("Больше 4 дней перемотать нельзя!\n");
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

void put_item()
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

int main()
{
    SetConsoleCP(CP_UTF8); // del on linux
    SetConsoleOutputCP(CP_UTF8); // ^
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
        default: 
            printf("Введите не отрицательное целое число меньше 7\n");
        }
	}
	return 0;
}