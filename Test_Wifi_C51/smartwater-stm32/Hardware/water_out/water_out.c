#include "water_out.h"

void WATER_OUT_Init(){
	
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //BEEP-->PA.6 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.0
	
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //BEEP-->PA.6 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.0
	
	 GPIO_SetBits(GPIOB,GPIO_Pin_8);  //�����ĵ�ŷ����ϵ缴��
	 GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //���յĵ�ŷ�,�ϵ缴
}
void water_open_1(){
	GPIO_ResetBits(GPIOC,GPIO_Pin_14);
}

void water_close_1(){
	GPIO_SetBits(GPIOC,GPIO_Pin_15); 
}

void water_open_2(){
	GPIO_SetBits(GPIOC,GPIO_Pin_14); 
}

void water_close_2(){
	GPIO_ResetBits(GPIOC,GPIO_Pin_15); 
}
	