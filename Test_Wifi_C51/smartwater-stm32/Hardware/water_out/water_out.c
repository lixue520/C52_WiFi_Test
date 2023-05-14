#include "water_out.h"

void WATER_OUT_Init(){
	
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //BEEP-->PA.6 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.0
	
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //BEEP-->PA.6 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.0
	
	 GPIO_SetBits(GPIOB,GPIO_Pin_8);  //常开的电磁阀，上电即关
	 GPIO_ResetBits(GPIOC,GPIO_Pin_13);  //常闭的电磁阀,上电即
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
	