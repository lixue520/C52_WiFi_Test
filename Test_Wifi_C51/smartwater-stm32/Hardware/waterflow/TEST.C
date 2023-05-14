#include "stm32f10x.h"
#include <stdio.h>

#define FLOW_SENSOR_PIN GPIO_Pin_0
#define FLOW_SENSOR_PORT GPIOA
#define FLOW_SENSOR_EXTI_LINE EXTI_Line0
#define FLOW_SENSOR_EXTI_IRQ EXTI0_IRQn

// 声明全局变量
volatile uint32_t pulse_count = 0;
volatile uint32_t last_capture = 0;
float flow_rate = 0.0f;

// 配置GPIO引脚和定时器
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    // 使能GPIOA和TIM2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 配置PA0为输入引脚
    GPIO_InitStructure.GPIO_Pin = FLOW_SENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FLOW_SENSOR_PORT, &GPIO_InitStructure);

    // 配置TIM2为输入捕获模式
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;  // 72MHz计数频率，1us计数周期
    TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF; // 最大计数值
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);

    // 配置PA0为TIM2的输入捕获通道
    GPIO_InitStructure.GPIO_Pin = FLOW_SENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FLOW_SENSOR_PORT, &GPIO_InitStructure);

    // 配置TIM2中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 配置PA0为外部中断线路，并将其映射到TIM2的输入捕获通道1
    EXTI_InitStructure.EXTI_Line = FLOW_SENSOR_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    // 启动TIM2
    TIM_Cmd(TIM2, ENABLE);
}

// 处理TIM2中断
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        // 定时器溢出事件
        pulse_count = 0;
        last_capture = 0;
        flow_rate = 0.0f;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }

    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
    {
        // 捕获事件
        uint32_t capture = TIM_GetCapture1(TIM2);
        uint32_t pulse_duration = capture - last_capture;
        last_capture = capture;
        pulse_count++;
        // 计算流量值
        if (pulse_duration != 0)
        {
            float time_elapsed = pulse_duration * 1e-6f; // 将us转换为秒
            float flow_rate_inst = 1.0f / time_elapsed;
            flow_rate = (flow_rate + flow_rate_inst) / 2.0f; // 取平均值
        }

        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
    }
}

// 配置USART串口
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 使能USART2和GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 配置PA9为USART1的TX引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART1
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 启动USART1
    USART_Cmd(USART1, ENABLE);
}

int main(void)
{
    GPIO_Config();
    USART_Config();

    while (1)
    {
        char buf[64];
        // snprintf(buf, sizeof(buf), "Flow rate: %.2f L/min\r\n", flow_rate);
        // USART_SendString(USART1, buf);
    }
}
