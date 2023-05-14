#include "timer.h"
#include "stdio.h"
#include "usart.h"
#include <stdlib.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//通用定时器 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/03
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
char oledBuf[20];
int k=0;//累积次数
extern u8 humidityH;	  //湿度整数部分
extern u8 humidityL;	  //湿度小数部分
extern u8 temperatureH;   //温度整数部分
extern u8 temperatureL;   //温度小数部分
extern float Light; //光照度
extern double spent;//消费金额
extern u8 alarmFlag;//是否报警的标志
extern float total_ml;
extern float Flow;
extern unsigned char user_money[16];
extern int price;
extern float total_ml_all;
//float k_money=0;
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	//TIM3_Start();
	//TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}

void TIM3_IRQHandler(void)
{
    static int counter = 0;  // 静态变量，只初始化一次，用于计数

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  // 判断是否为更新中断
    {
        counter++;  // 计数器加1
		if(counter ==2){//1s钟进行操作
			
			calculate_Total_Money(Flow,price);
			counter=0;
		}

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  // 清除中断标志位
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
	int k_money=(int)str_to_float(user_money); //将money转换成int
	DEBUG_LOG("money: %d ￥ Flow: %0.1f ml/s price: %d ml/￥",k_money,Flow,price);
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

    // 判断是否为负数
    if (str[0] == '-') {
        is_negative = 1;
        i++;
    }

    // 转换整数部分
    while (str[i] != '.' && str[i] != '\0') {
        integer_part = integer_part * 10 + (str[i] - '0');
        i++;
    }

    // 转换小数部分
    if (str[i] == '.') {
        i++;
        while (str[i] != '\0') {
            decimal_part = decimal_part * 10 + (str[i] - '0');
            decimal_count++;
            i++;
        }
    }

    // 计算浮点数
    result = integer_part + (float)decimal_part / (float)(10 * decimal_count);

    // 判断是否为负数
    if (is_negative) {
        result = -result;
    }

    return result;
}









//void TIM2_Int_Init(u16 arr,u16 psc)
//{
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

//	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
// 
//	TIM_ITConfig(  //使能或者失能指定的TIM中断
//		TIM2, //TIM2
//		TIM_IT_Update ,
//		ENABLE  //使能
//		);
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

//	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
//							 
//}

//void TIM2_IRQHandler(void)   //TIM2中断
//{
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
//		{
//			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
//			
////				sprintf(oledBuf,"Welcome");
////				OLED_ShowString(32,0,(u8*)oledBuf,16,1);//8*16 “ABC”
////				sprintf(oledBuf,"Hum:%d.%d %%",humidityH,humidityL);
////				OLED_ShowString(0,16,(u8*)oledBuf,16,1);//8*16 “ABC”
////				sprintf(oledBuf,"Temp:%d.%d C",temperatureH,temperatureL);
////				OLED_ShowString(0,32,(u8*)oledBuf,16,1);//8*16 “ABC”
////				sprintf(oledBuf,"Light:%.1f Lx",Light);
////				OLED_ShowString(0,48,(u8*)oledBuf,16,1);//8*16 “ABC”
////				OLED_Refresh();
//			
//		}
//}










