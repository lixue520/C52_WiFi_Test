#ifndef _water_H
#define _water_H


#include "stm32f10x.h"

void Water_GPIO_Config(void);
int Water_state(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_x);
void Water_state_printf(void);
float Get_LiquidHeight(void);
float Get_Rate_Of_Flow(void);//����
u16 Get_ADCValue_Water(void);
u16 Get_ADCValue_MQ2(void);
u16  Get_ADCValue_Height_PB1();//��ȡˮλ
void MQ2_PPM_Calibration(float RS);
float MQ2_GetPPM(void);
void Water_GPIO_Config(void);
float Water_Height_PB1(void);
u8 Get_ADCValue_turbidity(void);//���Ƕȴ�����
u8 Get_ADCValue_PH(void);//��ȡPHֵ
u8 Get_ADCValue_Temperature(void);//��ȡ�¶�
u16  Get_ADCValue_Comm(int i);//����ͨ��ֵ��ȡAD
float Get_Rate_Of_Flow_1();
float Get_Rate_Of_Flow_2();
float  Water_Height_PA4();
u8  Water_Height2() ;
#endif
