#include "usart.h"
#include <intrins.h>
#include <string.h>
void UartInit(void)
{
	SCON = 0x50;//数据位8位，停止位1位，起始位1位怎么配！都要根据芯片手册来写 ,使能接收
	
	T2CON |= 0x1<<4 | 0x1<<5;  //TCLK = 1(TranmitCLock)   RCLK = 1
	TH2 = 0xFF;
	TL2 = 0xFD; 
	RCAP2H = 0xFF; //RCAP2*的值为TH*重载
  	RCAP2L = 0xFD;

	TR2 = 1; //启动定时器


	EA = 1;
	ES = 1;

}

void send_str(char *str)
{
    while(*str) {
        SBUF = *str++;
        while(!TI);
        TI = 0;
    }
}

