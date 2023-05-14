//#include "reg52.h"

//sbit led1 = P1^0;

//void UartInit(void)
//{
//    SCON = 0x50;
//    TMOD &= 0x0F;
//    TMOD |= 0x20;
//    TH1 = 0xFD;
//    TL1 = 0xFD;
//    TR1 = 1;
//    ES = 1;
//    EA = 1;
//}

//void send_str(char *str)
//{
//    while(*str){
//        SBUF = *str++;
//        while(!TI);
//        TI = 0;
//    }
//}

//void Uart_server() interrupt 4
//{
//    if(RI){
//        RI = 0;
//        if(SBUF == 'o'){
//            while(!RI);
//            RI = 0;
//            if(SBUF == 'p'){
//                led1 = 0;
//                send_str("open_ok");
//            }
//        } else if(SBUF == 'c'){
//            while(!RI);
//            RI = 0;
//            if(SBUF == 'l'){
//                while(!RI);
//                RI = 0;
//                if(SBUF == 'o'){
//                    while(!RI);
//                    RI = 0;
//                    if(SBUF == 's'){
//                        while(!RI);
//                        RI = 0;
//                        if(SBUF == 'e'){
//                            led1 = 1;
//                            send_str("close_ok");
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

//void main()
//{
//    UartInit();
//    while(1);
//}
