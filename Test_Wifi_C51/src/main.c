#include "reg52.h"
#include "usart.h"
#include "delay.h"
#include "Lcd.h"
typedef unsigned int u16;	 //���������ͽ�����������
typedef unsigned char u8;
// ��������������������ʼ��Ϊ0
unsigned int counter = 0;
sbit led1 = P1^0;
sbit pulse_out = P3^3; // ����������� P3.3
sbit pulse_in = P3^5;  //T1����
sbit COUNT=P1^4; //����1 ����ģʽ��FZ��ת10�δ���һ�η�ת����ʾ������ʾ
sbit FZ = P1^2; //��ʱ0  1s��ת���źŷ�������Ϊpulse_in�źŵ�����
static unsigned int cnt = 0; // �������������
static unsigned char count = 0; //����һ����̬���������ڼ�¼��������
static u16 i;
char* timer0_int_1s="timer_0_interrupt_1s\r\n";

// ��ʱ��0��ʼ����������ģʽ������Ƶ

//����ʱ�� = (TH0 �� 256 + TL0) �� 12 / 11.0592 MHz

//�������е����ݴ�����㹫ʽ�У�

//����ʱ�� = (0xFC �� 256 + 0x18) �� 12 / 11.0592 MHz �� 1ms
void timer0_init(void)
{
    TMOD|=0X01;              //ѡ��Ϊ��ʱ��0ģʽ��������ʽ1��16λ��������TR0������
    TH0=0XFC;	             //����ʱ������ֵ����ʱ1ms  ��8λ
    TL0=0X18;                //                       ��8λ�������������㣬��˾�ο����棩
    ET0=1;                   //������ʱ��0�ж�
    EA=1;                    //�����ж�
    TR0=1;                   //�򿪶�ʱ��0������T/C0����

}
// ��ʱ��1��ʼ��������ģʽ��10�ξ��ж�
void timer1_init(void)
{
	TMOD &= 0x0F;  // ���T1����λ
    TMOD |= 0x10;  // ����T1����ģʽΪģʽ1
    TH1 = 0xFD;    // ������ʼֵ����ʱ5ms
    TL1 = 0xFD;
    TR1 = 1;       // ����T1������
    ET1 = 1;       // ����T1�ж�
    EA = 1;        // �����ж��ܿ���
}
void main()
{   IT1 = 1; // P3.5�ⲿ�ж�1�½��ش���
    EX1 = 1; // ����P3.5�ⲿ�ж�1
    UartInit();
    timer0_init(); // ��ʼ����ʱ��0
    timer1_init(); // ��ʼ����ʱ��1
    FZ=0;
	COUNT=0;
	LCD_Initial();
	GotoXY(0,0);
	Print("ds=   ");
	GotoXY(1,0);
	Print("js=   ");
	GotoXY(1,8);
	Print("fs=   ");
	

	

    while(1) {
		GotoXY(0,3);
		PutCh(i);
		GotoXY(1,3);
		PutCh(count);
		GotoXY(1,8);
		PutCh(timer0_int_1s);
		
    }
}


// ��ʱ��0�жϷ�����
void timer0_isr() interrupt 1
{
    TH0=0XFC;	            //�������Ĵ��8λ��������
    TL0=0X18;               //�������Ĵ��8λ��������
    i++;
    if(i==1000) { //1s��һ��
		 send_str(timer0_int_1s);
        if(FZ==1) {
            FZ=0;
        } else {
            FZ=1;
        }
    }
    if(i==2000)//1ms*1000=2s,��̫�죬���򴮿ڴ򲻳���
    {
        i=0;
        send_str("timer_0_interrupt_2s\r\n");
    }

}

// ��ʱ��1�жϷ�����
void timer1_isr() interrupt 3
{
	
	if(!pulse_in){//�͵�ƽ��ʼ����
		count++; //ÿ���жϣ�������1
		if(count == 10) //��������ﵽ10
		{
			count = 0; //�����������
			COUNT=~COUNT;
			send_str("timer_interrupt_count_10\r\n"); // ͨ�������������ֵ
		}
	}
}


void button_isr(void) interrupt 2
{
    //delay_ms(200); // ��ʱ����
    if(FZ == 0) // ����Ƿ��°�ť
    {
        cnt++; // ��ť��������1
        if(cnt == 10) // ������ֵ�ﵽ10ʱ
        {
            cnt = 0; // ���¼���
            send_str("ext_interrupt_count_10\r\n"); // ͨ�������������ֵ
        }
    }
}



void Uart_server() interrupt 4
{
    if(RI) {
        RI = 0;
        if(SBUF == 'o') {
            while(!RI);
            RI = 0;
            if(SBUF == 'p') {
                led1 = 0;
                send_str("open_ok\r\n");
            }
        } else if(SBUF == 'c') {
            while(!RI);
            RI = 0;
            if(SBUF == 'l') {
                while(!RI);
                RI = 0;
                if(SBUF == 'o') {
                    while(!RI);
                    RI = 0;
                    if(SBUF == 's') {
                        while(!RI);
                        RI = 0;
                        if(SBUF == 'e') {
                            led1 = 1;
                            send_str("close_ok\r\n");
                        }
                    }
                }
            }
        }
    }
}