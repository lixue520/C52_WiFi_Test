#ifndef USART_H
#define USART_H
#include <stdio.h>
#include <reg52.h>
void UartInit(void);

void send_str(char *str);

#endif