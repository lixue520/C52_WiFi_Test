#include "adc.h"

#include "delay.h"

// ALIENTEK miniSTM32开发板
// ADC多通道 代码

// #define SEND_BUF_SIZE 8200	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.
// u8 SendBuff[SEND_BUF_SIZE];	//发送数据缓冲区
__IO uint16_t ADC_ConvertedValue[6];

// u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度

// DMA1的各通道配置
// 这里的传输形式是固定的,这点要根据不同的情况来修改
// 从存储器->外设模式/8位数据宽度/存储器增量模式
// DMA_CHx:DMA通道CHx
// cpar:外设地址
// cmar:存储器地址
// cndtr:数据传输量
void MYDMA_Config()
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 使能DMA传输

	DMA_DeInit(DMA1_Channel1); // 将DMA的通道1寄存器重设为缺省值

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));	 // DMA外设基地址   存储器到存储器
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue; // DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					 // 数据传输方向
	DMA_InitStructure.DMA_BufferSize = 7;								 // DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	 // 外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 数据16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		  // 是否开启循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // DMA通道  拥有中优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		  // DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);		  // 根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器

	// DMA_Cmd(DMA1_Channel1, DISABLE );  //关闭USART1 TX DMA1 所指示的通道
	// DMA_SetCurrDataCounter(DMA1_Channel1,6);//DMA通道的DMA缓存的大小
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

// 初始化ADC
// 这里我们仅以规则通道为例
// 我们默认将开启通道0~3
void Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	 // 使能GPIOB时钟
	// 使能GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1通道时钟

	MYDMA_Config();

	RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	// PA0 PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	ADC_DeInit(ADC1); // 复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	   // 模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				// ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 7;								// 顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);									// 根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE); // 使能指定的ADC1

	ADC_ResetCalibration(ADC1); // 使能复位校准

	while (ADC_GetResetCalibrationStatus(ADC1))
		; // 等待复位校准结束

	ADC_StartCalibration(ADC1); // 开启AD校准

	while (ADC_GetCalibrationStatus(ADC1))
		; // 等待校准结束

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);//流量
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);//流量
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 3, ADC_SampleTime_55Cycles5);//水深2
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_55Cycles5);//水深1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 5, ADC_SampleTime_55Cycles5);//PH
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 6, ADC_SampleTime_55Cycles5);//TDS
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 7, ADC_SampleTime_55Cycles5);//Temp
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 使能指定的ADC1的软件转换启动功能
}
/*
#if 0
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)
{
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}
#endif
*/

//	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道
// 	DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
// 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道
