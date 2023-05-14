#include "stm32f10x.h"

int main()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能PA3端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置PA3为输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 使能PD12端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // 配置PD12为输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    while (1)
    {
        // 如果PA3端口有信号
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 1)
        {
            // 让LED亮一秒
            GPIO_SetBits(GPIOD, GPIO_Pin_12);
            delay_ms(1000);
            GPIO_ResetBits(GPIOD, GPIO_Pin_12);
        }
    }

    return 0;
}