
#ifndef LED_H_
#define LED_H_
#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char
#define  uchar unsigned char
#define  ulong unsigned long
#define uchar unsigned char
#define uint unsigned int
#define LCD_COM	 0  // Command
#define LCD_DAT  1  // Data
sbit LcdRS=P1^7;
sbit LcdRW=P1^6;
sbit LcdEN=P1^5;



void delay(uint t);

void time(unsigned int t);

void LCD_WRITE(unsigned char x,bit WS);
void LCD_Initial();
void GotoXY(unsigned char x,unsigned char y);
void PutCh(char m);
void Print(unsigned char *str);
#endif /* LED_H_ */
