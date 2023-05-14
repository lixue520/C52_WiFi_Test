#include "Lcd.h"

//操作实例：在主函数上写入即可显示
//LCD_Initial();
//GotoXY(0,0);
//Print("1900800724qzw");
//GotoXY(1,0);
//Print("st=   ");
//GotoXY(1,8);
//Print("sp=   ");
//y=set;

//GotoXY(1,3);
//PutCh(y/1000+'0');
//PutCh((y/100)%10+'0');
//PutCh((y/10)%10+'0');
//PutCh(y%10+'0');
//y=y*500;

//m=pos;

//GotoXY(1,10);
//PutCh(m/1000+'0');
//PutCh((m/100)%10+'0');
//PutCh((m/10)%10+'0');
//PutCh(m%10+'0');









void delay(uint t)
{
    while(t--);
}


void time(unsigned int t)
{
    unsigned int i;
    for(i=0; i<t; i++);
}


void LCD_WRITE(unsigned char x,bit WS)
{
    P2=x;
    LcdRW=0;
    LcdRS=WS;
    LcdEN=1;
    time(50);
    LcdEN=0;
}

void LCD_Initial()
{
    LCD_WRITE(0x38,LCD_COM);
    time(120);
    LCD_WRITE(0x38,LCD_COM);
    time(120);
    LCD_WRITE(0x01,LCD_COM);
    time(120);
    LCD_WRITE(0x06,LCD_COM);
    time(120);
    LCD_WRITE(0x0c,LCD_COM);
    time(120);
}


void GotoXY(unsigned char x,unsigned char y)
{
    unsigned char code table[4]= {0x00,0x40,0x10,0x50};
    LCD_WRITE(0x80+table[x]+y, LCD_COM);
}
void PutCh(char m)
{
    LCD_WRITE(m,LCD_DAT);
}
void Print(unsigned char *str)
{
    while(*str!='\0')
    {
        PutCh(*str);
        str++;
    }
}