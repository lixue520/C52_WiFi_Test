#ifndef _waterflow_H
#define _waterflow_H
#include "stm32f10x.h"

#define FLOW_SENSOR_PIN GPIO_Pin_1 //将它作为TIM2的输入捕获通道
#define FLOW_SENSOR_PORT GPIOA
#define FLOW_SENSOR_EXTI_LINE EXTI_Line0
#define FLOW_SENSOR_EXTI_IRQ EXTI0_IRQn


void GPIO_Water_Config(void);

#endif
