#include "SPI2.h"

//SPI2的初始化
void SPI2_Init(void)
{
//	RCC->APB2ENR|=(1<<3); //GPIOB引脚时钟
//	RCC->APB1ENR|=(1<<14);//开启SPI2时钟
//	GPIOB->CRH&=0x000FFFFF;
//	GPIOB->CRH|=0xB4B00000;//配置PB13、PB14、PB15
//	GPIOB->BSRR|=7<<13;  //上拉
//	GPIOF->CRL&=0xFFFFFF00;
//	GPIOF->CRL|=0x0000033;  //PF0\PF1浮空输出，输出50MHZ
//	GPIOF->BRR=0X02;   //下拉
//	SPI2->CR1|= (0<<11)| //8位数据帧
//				(0<<10)| //全双工模式
//				(1<<9)|  //启用软件从设备管理	另外生成CS
//				(1<<8)|  //选择从设备
//				(0<<7)|  //先左移MSB
//				(1<<2)|  //配置为主设备
//				(0<<1)|  //CPOL：时钟极性
//				(0<<0);  //CPHA：时钟相位
	    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能
    RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB

    GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB13/14/15上拉

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI2, ENABLE); //使能SPI外设

    SPI2_ReadWriteByte(0xff);//启动传输
}
//SPI2速度的设置 ，传入的SPEED为一个数值
//000： fPCLK/2   001： fPCLK/4   010： fPCLK/8    011： fPCLK/16
//100： fPCLK/32  101： fPCLK/64  110： fPCLK/128  111： fPCLK/256
//SPI2接在APB1总线上，最大时钟频率为36MHZ，fPCLK=fAPB1=36MHZ
void SPI2_SpeedSet(u8 speed)
{
	speed&=0x07;
	SPI2->CR1&=0xFFC7;
	SPI2->CR1|=speed<<3;
	SPI2->CR1|=(1<<6);
}
//对spi2的读写操作
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u16 retry=0;				 
	while((SPI2->SR&1<<1)==0)		//等待发送区空	
	{
		retry++;
		if(retry>=0XFFFE)return 0; 	//超时退出
	}			  
	SPI2->DR=TxData;	 	  		//发送一个byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) 		//等待接收完一个byte  
	{
		retry++;
		if(retry>=0XFFFE)return 0;	//超时退出
	}	  						    
	return SPI2->DR;          		//返回收到的数据				    
}

u8 SPIWriteByte(u8 Byte)
{
	while((SPI2->SR&0X02)==0);		//等待发送区空	  
	SPI2->DR=Byte;	 	            //发送一个byte   
	while((SPI2->SR&0X01)==0);      //等待接收完一个byte  
	return SPI2->DR;          	    //返回收到的数据			
}










