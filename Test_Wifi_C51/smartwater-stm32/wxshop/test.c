#include "stm32f10x.h"
#include "delay.h"
#include "bsp_usart.h"
#include "RC522.h"
 
int main()
{
    char status;
	unsigned char snr, buf[16], TagType[2], SelectedSnr[4], DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    delay_init();
    usart_init();
    
    PcdInit();
	PcdReset();
	PcdAntennaOff();
	PcdAntennaOn();
	M500PcdConfigISOType( 'A' );
 
    printf( "init over!\n" );
	while(1){
        // 寻卡
		status= PcdRequest( REQ_ALL , TagType );    // REQ_ALL代表寻天线区内所有卡。TagType为返回的卡类型
		if(!status)
		{
            // 防冲突
			status = PcdAnticoll(SelectedSnr);
			if(!status)
			{
                // 选卡
				status=PcdSelect(SelectedSnr);
				if(!status)
				{
                    // 认证
					snr = 1;    // 扇区号1
					status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // 校验1扇区密码，密码位于每一扇区第3块
                                          // 验证A密钥，块地址，扇区密码，卡序列号
					{
						if(!status)
						{
                            //读写卡
							status = PcdRead((snr*4+0), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
//							status = PcdWrite((snr*4+0), "way2");   // 写卡，将buf[0]-buf[16]写入1扇区0块
							if(!status) 
							{
                                //读写成功
                                printf("read finish!\n");
                                printf("读到的值是： %s\n",buf);
								WaitCardOff();
							}
						}
					}
				}
			}
		}
	}
}
 