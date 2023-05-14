#include "pump.h"
 
void Pump_Init()
{
    
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*ʹ��SWD ����JTAG*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12; // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_7; // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
    GPIO_ResetBits(GPIOA, GPIO_Pin_11); 
    GPIO_ResetBits(GPIOA, GPIO_Pin_12); //IN1��IN2ͬʱ����ߵ�ƽ���ߵ͵�ƽ���ֹͣת��
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_3); 
    GPIO_ResetBits(GPIOB, GPIO_Pin_4); 
	
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
	
	
}

void pump_start_1()
{
	//�����ת
    GPIO_SetBits(GPIOA, GPIO_Pin_11);
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);	
	
}

void pump_stop_1()
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_11); 
    GPIO_ResetBits(GPIOA, GPIO_Pin_12); 
}

void pump_start_2() {
    GPIO_SetBits(GPIOB, GPIO_Pin_3);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);	
   
}
void pump_stop_2() {
    GPIO_ResetBits(GPIOB, GPIO_Pin_3); 
    GPIO_ResetBits(GPIOB, GPIO_Pin_4);
}