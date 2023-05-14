#include "usart.h"
#include <intrins.h>
#include <string.h>
void UartInit(void)
{
	SCON = 0x50;//����λ8λ��ֹͣλ1λ����ʼλ1λ��ô�䣡��Ҫ����оƬ�ֲ���д ,ʹ�ܽ���
	
	T2CON |= 0x1<<4 | 0x1<<5;  //TCLK = 1(TranmitCLock)   RCLK = 1
	TH2 = 0xFF;
	TL2 = 0xFD; 
	RCAP2H = 0xFF; //RCAP2*��ֵΪTH*����
  	RCAP2L = 0xFD;

	TR2 = 1; //������ʱ��


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

