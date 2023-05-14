#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"
#include "stdbool.h"


#define USART_DEBUG		USART1		//���Դ�ӡ��ʹ�õĴ�����
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART_RX_STA;         		//����״̬���
//����봮���жϽ��գ��벻Ҫע�����º궨��

void HMISends(char *buf1);
void HMISendb(u8 buf);

void HMISendstart(void);

void Usart1_Init(u32 bound);
void Usart2_Init(unsigned int baud);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

void DEBUG_LOG(char *fmt,...);


#endif

