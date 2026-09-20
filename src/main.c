#include <stdio.h>
#include <Windows.h> //del on linux

#define _CRT_SECURE_NO_WARNINGS // ))))
#define INVENTORY_SIZE 10
int current_day = 1;
int current_hour = 8;

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
            return "Такого элемента нет";
    }
}

void print_menu(void)
{
    printf("[0] Выход\n");
    printf("[1] Посмотреть на часы\n");
    printf("[2] Промотать время\n");
    printf("[3] Посмотреть инвентарь\n");
    printf("[4] Положить предмет в слот\n");
    printf("[5] Выбросить предмет\n");
    printf("[6] Просмотр любимого ресурса\n");
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
            printf("Промотано %d\n", response);
            check_time();
            return;
        }
    }
}

void check_inventory(void)
{

}

int main()
{
    SetConsoleCP(CP_UTF8); // del on linux
    SetConsoleOutputCP(CP_UTF8); // ^
    int inventory[INVENTORY_SIZE] = {0, 1, 2, 3, 3, 3, 5, 6, 4, 7};
    print_menu();
	while (1)
	{
        int response = read_input(0);
        switch (response)
        {
        case 0: 
            printf("Выход из программы\n");
            return;
        case 1:
            check_time();
            break;
        case 2:
            forward_time();
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        default: 
            printf("Введите не отрицательное целое число меньше 7\n");
        }
	}
	return 0;
}