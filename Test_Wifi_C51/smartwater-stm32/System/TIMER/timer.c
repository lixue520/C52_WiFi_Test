#include "timer.h"
#include "stdio.h"
#include "usart.h"
#include <stdlib.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//ͨ�ö�ʱ�� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
char oledBuf[20];
int k=0;//�ۻ�����
extern u8 humidityH;	  //ʪ����������
extern u8 humidityL;	  //ʪ��С������
extern u8 temperatureH;   //�¶���������
extern u8 temperatureL;   //�¶�С������
extern float Light; //���ն�
extern double spent;//���ѽ��
extern u8 alarmFlag;//�Ƿ񱨾��ı�־
extern float total_ml;
extern float Flow;
extern unsigned char user_money[16];
extern int price;
extern float total_ml_all;
//float k_money=0;
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	//TIM3_Start();
	//TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}

void TIM3_IRQHandler(void)
{
    static int counter = 0;  // ��̬������ֻ��ʼ��һ�Σ����ڼ���

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  // �ж��Ƿ�Ϊ�����ж�
    {
        counter++;  // ��������1
		if(counter ==2){//1s�ӽ��в���
			
			calculate_Total_Money(Flow,price);
			counter=0;
		}

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  // ����жϱ�־λ
    }
}

void TIM3_Start(void) {
    TIM_Cmd(TIM3, ENABLE);
	//DEBUG_LOG("Timer3 start");
}

void TIM3_Stop(void) {
    TIM_Cmd(TIM3, DISABLE);
	//DEBUG_LOG("Timer3 stop");
}

void calculate_Total_Money(float Flow,int price){
	int k_money=(int)str_to_float(user_money); //��moneyת����int
	DEBUG_LOG("money: %d �� Flow: %0.1f ml/s price: %d ml/��",k_money,Flow,price);
	total_ml += Flow;
	if(total_ml>=10){
		total_ml-=10;
		k_money -=1;
		spent++;
		k++;
	}
	if(k_money<=1){
		total_ml_all=k*100;
		k=0;
	}
	int_to_str(k_money, user_money);
	//memcpy(user_money, ic->money, sizeof(ic->money));
}

void int_to_str(int num, unsigned char *str) {
    sprintf((char*)str, "%d", num);
}


float str_to_float(unsigned char *str) {
    float result = 0.0;
    int integer_part = 0;
    int decimal_part = 0;
    int decimal_count = 0;
    int i = 0;
    int is_negative = 0;

    // �ж��Ƿ�Ϊ����
    if (str[0] == '-') {
        is_negative = 1;
        i++;
    }

    // ת����������
    while (str[i] != '.' && str[i] != '\0') {
        integer_part = integer_part * 10 + (str[i] - '0');
        i++;
    }

    // ת��С������
    if (str[i] == '.') {
        i++;
        while (str[i] != '\0') {
            decimal_part = decimal_part * 10 + (str[i] - '0');
            decimal_count++;
            i++;
        }
    }

    // ���㸡����
    result = integer_part + (float)decimal_part / (float)(10 * decimal_count);

    // �ж��Ƿ�Ϊ����
    if (is_negative) {
        result = -result;
    }

    return result;
}









//void TIM2_Int_Init(u16 arr,u16 psc)
//{
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

//	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
// 
//	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
//		TIM2, //TIM2
//		TIM_IT_Update ,
//		ENABLE  //ʹ��
//		);
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
//	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

//	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
//							 
//}

//void TIM2_IRQHandler(void)   //TIM2�ж�
//{
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
//		{
//			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
//			
////				sprintf(oledBuf,"Welcome");
////				OLED_ShowString(32,0,(u8*)oledBuf,16,1);//8*16 ��ABC��
////				sprintf(oledBuf,"Hum:%d.%d %%",humidityH,humidityL);
////				OLED_ShowString(0,16,(u8*)oledBuf,16,1);//8*16 ��ABC��
////				sprintf(oledBuf,"Temp:%d.%d C",temperatureH,temperatureL);
////				OLED_ShowString(0,32,(u8*)oledBuf,16,1);//8*16 ��ABC��
////				sprintf(oledBuf,"Light:%.1f Lx",Light);
////				OLED_ShowString(0,48,(u8*)oledBuf,16,1);//8*16 ��ABC��
////				OLED_Refresh();
//			
//		}
//}










