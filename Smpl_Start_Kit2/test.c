#include <stdio.h>
#include <stdlib.h>
#include "NUC1xx.h"
#include "Driver/DrvGPIO.h"
#include "Driver/DrvSYS.h"
#include "LCD_Driver.h"
#include "Seven_Segment.h"

// ======================================
// 按鍵初始化 (K1 = GPB14)
// ======================================
void init_key(void)
{
    DrvGPIO_Open(E_GPB, 14, E_IO_INPUT);      // K1 真實腳位
}

// ======================================
// 按鍵讀取 (K1 按下為 0)
// ======================================
int read_key(void)
{
    if(DrvGPIO_GetBit(E_GPB, 14) == 0)        // 按鍵按下
    {
        DrvSYS_Delay(8000);                   // 去抖
        if(DrvGPIO_GetBit(E_GPB, 14) == 0)
        {
            while(DrvGPIO_GetBit(E_GPB, 14) == 0);   // 等放開
            return 1;
        }
    }
    return 0;
}

// ======================================
// 七段顯示器初始化
// ======================================
void init_7seg(void)
{
    int i;

    for(i = 0; i < 15; i++)
        DrvGPIO_Open(E_GPE, i, E_IO_OUTPUT);

    for(i = 4; i <= 7; i++)
        DrvGPIO_Open(E_GPC, i, E_IO_OUTPUT);
}

// ======================================
// 顯示骰子數字
// ======================================
void show_dice(int n)
{
    close_seven_segment();
    show_seven_segment(0, n);
}

// ======================================
// 骰子滾動動畫
// ======================================
void dice_roll_animation(void)
{
    int seq[6] = {1,2,3,4,5,6};
    int i;

    for(i = 0; i < 25; i++)
    {
        show_dice(seq[i % 6]);
        DrvSYS_Delay(50000); // 50ms
    }
}

// ======================================
// 主程式
// ======================================
int main(void)
{
    int result;
    char buf[20];

    // 系統初始化
    UNLOCKREG();
    DrvSYS_SetOscCtrl(E_SYS_XTL12M, 1);
    SysTimerDelay(5000);
    DrvSYS_SelectHCLKSource(0);
    LOCKREG();

    Initial_pannel();
    clr_all_pannal();

    init_key();
    init_7seg();

    print_lcd(0, "Electronic Dice");
    print_lcd(1, "Press K1 to Roll");

    while(1)
    {
        if(read_key())           // 按 K1 (PB14)
        {
						srand(SysTick->VAL);
            print_lcd(3, "Rolling...");
            dice_roll_animation();
						
            result = rand() % 6 + 1;

            show_dice(result);

            sprintf(buf, "Result: %d", result);
            print_lcd(3, buf);

            DrvSYS_Delay(800000); // 停留 0.8 秒
        }
    }
}
