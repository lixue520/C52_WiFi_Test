#include "delay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys.h"
#include "usart.h"
#include "rc522.h"
#include "pump.h"
#include "beep.h"
#include "timer.h"
#include "exti.h"
#include "stdio.h"
#include "esp8266.h"
#include "onenet.h"
#include "led.h"
#include "pay.h"
#include "adc.h"
#include "water.h"
#include "water_out.h"
#include "ds18b20.h"
/*SP2连线说明：方案优化-->ADC采集口预留
*1--NSS(SDA/CS)  <----->PB12
*2--SCK <----->PB13
*3--MOSI<----->PB15
*4--MISO<----->PB14
*5--悬空
*6--GND <----->GND
*7--RST <----->PB9
*8--VCC <----->VCC
*/
u8 alarmFlag = 0;//是否报警的标志
u8 alarm_is_free = 10;//报警器是否被手动操作，如果被手动操作即设置为0
char PUB_BUF[456];//上传数据的buf
char PUB_BUF1[256];
char devicd_id[]="qzw10086"; //设备号
char types[]="Esp8266"; //网络类型
u32 Server_Esponse=0;//服务器响应标志，用来重连服务器的，每发50次就重连服务器
const char *devSubTopic[] = {"smartwater/sub"};//控制机1的话题
const char devPubTopic[] = "smartwater/pub";
const char *devSubTopic1[] = {"smartwater/sub1"};//扩展机
const char devPubTopic1[] = "smartwater/pub1";
u8 ESP8266_INIT_OK = 0;//esp8266初始化完成标志
u8 IC_flag=0; //刷卡标志位，当刷卡一次位1，刷卡第二次位0，默认位0
unsigned char user_id[16]="1";  //IC卡卡号
unsigned char user_name[16]="1"; //IC卡持有者
unsigned char user_money[16]="1";//IC卡存款
unsigned char user_password[16]="1"; //IC卡密码
double spent=0.0;//消费金额
u8 IC_recharge_money=0;//充值标志,有上位机或者串口屏幕控制
double recharge=0.0;//充值金额
int price=100;//水价50ml/yuan,什么水这么贵？？？原来是神仙水
float Height; //水箱液位高度0
float Height1; //水箱液位高度1
float Flow;   //水流量0
float Flow1;  //水流量1
float total_ml=0;//100ml以内扣费出水量
float total_ml_all=0;//标准统计出水量
u8 Led_Status = 0;
u8 BEEP=0; //报警器
u8 Pump=0; //水泵0
u8 Pump1=0;//水泵1
u8 DZF=0; //电磁阀0
u8 DZF1=0;//电磁阀1
u8 ZW_LED=0;//紫外线灯
u8 Bottle = 0;//水杯液满标志位，当Bottle为1时水杯满了需要控制电子阀门
float ph=7;//ph值
float tds=0;//tds
float temp=20;//水温
u32 time = 0;//IC卡刷卡支付时:电磁阀门打开-电子阀关闭
int main(void)
{
    unsigned short timeCount = 0;	//发送间隔变量
    unsigned char *dataPtr = NULL;  //接收平台数据
    static u8 lineNow;
    unsigned char status;
    unsigned char s=0x08;
    delay_init();	    	 //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    Usart1_Init(115200);	 	//串口初始化为115200(调试输出)
    DEBUG_LOG("\r\n");
    DEBUG_LOG("0.UART1初始化			[OK]");
    WATER_OUT_Init();
    GPIO_SetBits(GPIOB,GPIO_Pin_8);//电磁阀1,
    GPIO_ResetBits(GPIOC,GPIO_Pin_13);//电磁阀2,常开电磁阀门关闭
    DEBUG_LOG("1.电磁阀初始化			[OK]");

    delay_init();	    	 //延时函数初始化
    DEBUG_LOG("2.延时函数初始化			[OK]");

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
    DEBUG_LOG("3.中断优先初始化			[OK]");

    EXTIX_Init();		//外部中断初始化
    DEBUG_LOG("4.外部中断初始化			[OK]");

    BEEP_Init();
    DEBUG_LOG("5.蜂鸣报警器初始化		[OK]");
    Pump_Init();
    DEBUG_LOG("6.水泵控制器初始化		[OK]");

    Usart2_Init(115200);//stm32-8266通讯串口

    DEBUG_LOG("7.UART2初始化			[OK]");

    LED_Init();
    DEBUG_LOG("8.LED指示灯初始化		[OK]");
    InitRc522();

    DEBUG_LOG("9.RC522初始化		    [OK]");
    Adc_Init();
    DEBUG_LOG("10.AD通道初始化		    [OK]");
    HMISendstart();          //为确保串口HMI正常通信
    DEBUG_LOG("11.HMI启动		        [OK]");
    //Ultrasonic_Config();
    DEBUG_LOG("12.超声波启动		    [OK]");
    //DS18B20_Init();不懂为啥不初始化也能用，那就不管了，不要解开注释否则ST-LINK烧写会失败
    DEBUG_LOG("12.水温传感器启动		    [OK]");
    DEBUG_LOG("-------基础硬件初始化完成--------");

#if 0
    DEBUG_LOG("初始化ESP8266 WIFI模块...");
    if(!ESP8266_INIT_OK) {}
    ESP8266_Init();					//初始化ESP8266
    while(OneNet_DevLink()) { //接入OneNET
        delay_ms(500);
    }
#endif

//    TIM2_Int_Init(4999,7199);
    TIM3_Int_Init(2499,7199);
    DEBUG_LOG("初始化定时器3...");
#if 0
    OneNet_Subscribe(devSubTopic, 1);//订阅服务器话题管道
#endif



    while(1)
    {
 
#if 0
        //防止服务器断开连接，发50次后主动断开并重连
        if(Server_Esponse==30) {
            Server_Esponse=0;
            if(!ESP8266_INIT_OK) {}
            ESP8266_Init();					//初始化ESP8266
            while(OneNet_DevLink()) { //接入OneNET
                delay_ms(500);
            }
            OneNet_Subscribe(devSubTopic, 1);//订阅服务器话题管道
        }
#endif
#if 0
        //RC522_Handle();
        //GPIO_SetBits(GPIOC,GPIO_Pin_13);测试电磁阀门
        IC* k= {0};  //读卡上传
        IC* wic= {0}; //写卡上传
        if(IC_flag==0&&IC_recharge_money==0) {//读卡
            k= ReadCard(2);
            GPIO_ResetBits(GPIOB,GPIO_Pin_8);//打开
            free(k);
        }
        if(IC_flag==1&&IC_recharge_money==0) {//消费
            //DEBUG_LOG("%0.1f",spent);
            wic=add_or_sub(2,0,spent);
            free(wic);
        }
        if(IC_recharge_money==1) {//充值
            wic=add_or_sub(2,1,recharge);
            free(wic);
        }

        Flow=Get_Rate_Of_Flow_1();
        if((IC_flag==1&&Flow!=0&&IC_recharge_money==0)&&(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)==0||GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)==1)) {
            TIM3_Start();//刷卡且流量不为0时打开
        } 
        else {
            TIM3_Stop();
        }
#endif
#if 1
        Get_Rate_Of_Flow_1();//rate
        Water_Height_PA4();//Height
        Get_ADCValue_turbidity();//tds
        Get_ADCValue_PH();//ph
        Water_Height2();//Height2
        //check_person();
        DS18B20_GetTemperture();


#endif

#if 0

        GPIO_SetBits(GPIOA,GPIO_Pin_11);//水泵1，开
        GPIO_ResetBits(GPIOA,GPIO_Pin_12);


        GPIO_SetBits(GPIOB,GPIO_Pin_3);//水泵2，开
        GPIO_ResetBits(GPIOB,GPIO_Pin_4);

        GPIO_SetBits(GPIOB, GPIO_Pin_5);//报警，开
//        GPIO_SetBits(GPIOB,GPIO_Pin_7);//紫外线
//        delay_ms(2500);
        GPIO_ResetBits(GPIOB,GPIO_Pin_7); //紫外线开


#endif

#if 0
        //用来测试继电器/驱动模块
        pump_start_1();
        pump_start_2();
        //pump_stop_2();
#endif

#if 0
        //IC卡读写测试
        IC*ric= {0};
        IC*wic= {0};
        //IC卡操作接口测试
        //PcdReset();复位
        //test_read_write();测试读写1
        //RC522_Handle();测试读写2
        //init_IC(2,"123456","DavidNan","220","000000");//初始化卡
        ric=ReadCard(2);//读取扇区2数据到ic
        free(ric);//一定要合理释放内存
        wic=add_or_sub(2,1,10);//消费10块
        free(wic);
//			IC*ric={0};
//			ric=ReadCard(2);
//			memcpy(user_id, ric->ID, sizeof(ric->ID));
//			memcpy(user_name, ric->name, sizeof(ric->name));
//			memcpy(user_money, ric->money, sizeof(ric->money));
//			memcpy(user_password, ric->password, sizeof(ric->password));
//			free(ric);
#endif
#if 0
        //esp8266-->阿里云
        if(timeCount % 20 == 0)//1000ms / 25 = 40 一秒执行一次
        {
            //Get_Rate_Of_Flow_1();
            //Get_Rate_Of_Flow_2();
            //Water_Height_PA4();
            //Water_Height_PB1();
            //Get_ADCValue_turbidity();
            //Get_ADCValue_PH();
            //test_Temp();
            //check_person();

            /********** 读取液位传感器数据**************/
            Height=Water_Height_PA4();
            Height1=20;//取最大值20cm
            if(Height<=5) {
                GPIO_SetBits(GPIOB,GPIO_Pin_8);//电磁阀1,关闭
                GPIO_SetBits(GPIOC,GPIO_Pin_13);//电磁阀2,常开电磁阀门打开
                GPIO_ResetBits(GPIOB, GPIO_Pin_3);
                GPIO_ResetBits(GPIOB, GPIO_Pin_4);
                GPIO_SetBits(GPIOA, GPIO_Pin_11);
                GPIO_ResetBits(GPIOA, GPIO_Pin_12);

            }
            if(Height1<=5) {

                GPIO_ResetBits(GPIOB,GPIO_Pin_8);//电磁阀1,打开
                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//电磁阀2,常开电磁阀门关闭
                GPIO_ResetBits(GPIOA, GPIO_Pin_11);
                GPIO_ResetBits(GPIOA, GPIO_Pin_12);
                GPIO_SetBits(GPIOB, GPIO_Pin_3);
                GPIO_ResetBits(GPIOB, GPIO_Pin_4);

            }
            /********** 读取流量传感器数据**************/
            Flow=Get_Rate_Of_Flow_1();
            Flow1=0;
            /********** 读取水泵状态 **************/

            if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)==1&&GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)==0) {
                Pump=1;
            } else {
                Pump=0;
            }
            if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)==1&&GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)==0) {
                Pump1=1;
            } else {
                Pump1=0;
            }


            /********** 读取蜂鸣器状态 **************/
            BEEP=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);;
            /********** 获取IC卡数据 **************/
//			IC*ric={0};
//			ric=ReadCard(2);
//			free(ric);
            /********** 读取紫外线状态 **************/
            ZW_LED=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
            /********** 读取PH值数据 **************/
            ph=Get_ADCValue_PH();
            /********** 读取TDS数据 **************/
            tds=Get_ADCValue_turbidity();
            /********** 读取Temp(水温) **************/
            temp=test_Temp();
            /********** 读取出水时间**************/
            time=0;

            /********** 报警逻辑 **************/
            if(alarm_is_free == 10)//报警器控制权是否空闲 alarm_is_free == 10 初始值为10
            {

            }
            if(alarm_is_free < 10)alarm_is_free++;

        }
        if(++timeCount >= 100)	// 5000ms / 25 = 200 发送间隔5000ms
        {
            DEBUG_LOG("发布数据 ----- OneNet_Publish");
            sprintf(PUB_BUF,"  {\"BEEP\":%d,\"time\":%d,\"DZF\":%d,\"DZF1\":%d,\"types\":\"%s\",\"status\":%d,\"user_id\":\"%s\",\"user_name\":\"%s\",\"user_money\":\"%s\",\"user_password\":\"%s\",\"devicd_id\":\"%s\",\"Bottle\":%d,\"Pump\":%d,\"Height\":%.1f,\"Height1\":%.1f,\"Flow\":%.1f,\"Flow1\":%.1f,\"Temp\":%.1f,\"ph\":%.1f,\"tds\":%.1f,\"Pump1\":%d,\"ZW_LED\":%d}",
                    BEEP?1:0,time,DZF?1:0,DZF1?1:0,types,ESP8266_INIT_OK,user_id,user_name,user_money,user_password,devicd_id,Bottle?1:0,Pump?1:0,Height,Height1,Flow,Flow1,temp,ph,tds,Pump1?1:0,ZW_LED?1:0);
            OneNet_Publish(devPubTopic, PUB_BUF);

            Server_Esponse++;
            DEBUG_LOG("<==================================================================================>");
            timeCount = 0;
            ESP8266_Clear();
        }

        dataPtr = ESP8266_GetIPD(3);
        if(dataPtr != NULL)
            OneNet_RevPro(dataPtr);
        delay_ms(10);
    
#endif
    }

}
