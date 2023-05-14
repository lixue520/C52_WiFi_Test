#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "stm32f10x.h"
#include "delay.h"


/*  DS18B20ʱ�Ӷ˿ڡ����Ŷ��� */
#define DS18B20_PORT 			GPIOA  
#define DS18B20_PIN 			(GPIO_Pin_15)
#define DS18B20_PORT_RCC		RCC_APB2Periph_GPIOA


////IO��������											   
#define	DS18B20_DQ_OUT PAout(15) //���ݶ˿�	
#define	DS18B20_DQ_IN  PAin(15)  //���ݶ˿�	 
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
float DS18B20_GetTemperture(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Reset(void);			//��λDS18B20   

#endif

