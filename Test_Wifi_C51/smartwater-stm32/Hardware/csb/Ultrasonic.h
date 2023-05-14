
#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "stm32f10x.h"




void Ultrasonic_Config(void);
void check_person(void);
void Timer2_Config(void);
u32 Distance_Calculate(u32 count);
#endif
