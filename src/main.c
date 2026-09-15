#include <stdio.h>
#include <Windows.h> //del on linux

#define _CRT_SECURE_NO_WARNINGS // ))))
#define INVENTORY_SIZE 10
/*
#define EMPTY 0
#define WOOD 1
#define STONE 2
#define SEED 3
#define WCAN 4
#define HOE 5
#define FERT 6
#define WHEAT 7
#define CARROT 8
#define POTATO 9
*/

char get_item_name(int id)
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
}

int main(void)
{
    SetConsoleCP(CP_UTF8); // del on linux
    SetConsoleOutputCP(CP_UTF8); // ^
	int current_day = 1;
	int current_hour = 8;
    int response = -1;
    int inventory[INVENTORY_SIZE] = {0, 1, 2, 3, 3, 3, 5, 6, 4, 7};
    print_menu();
	while (1)
	{
        int res_check = scanf_s("%d",&response);
        if (res_check != 1)
        {
            printf("Введите целое число!\n");
        }
        clear_buffer();
        
        switch (response)
        {
        case 0: 
            printf("Выход из программы\n");
            return;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        }
	}
	return 0;
}