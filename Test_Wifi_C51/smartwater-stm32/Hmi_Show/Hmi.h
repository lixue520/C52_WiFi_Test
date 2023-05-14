#ifndef __HMI_H
#define __HMI_H
#include "stdio.h"
#include "sys.h"

void Hmi_Usart_init(u32 bound);
void HMISends(char *buf1);
void HMISendb(u8 k);
#endif