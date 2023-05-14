#include "water.h"
//#include "led.h"
#include "delay.h"
#include "timer.h"
#include "usart.h"
#include "adc.h"
#include "stdio.h"
#include "math.h"
#define CAL_PPM 20		// 校准环境重的PMM值
#define RL 5			// RL阻值
static float R0 = 4.00; // 元件在洁净空气中的阻值 6.0
u16 times_mq = 1;
extern __IO uint16_t ADC_ConvertedValue[6];


// 得到ADC采样内部传感器的值
// 取10次，然后平均
u16 Get_ADCValue_Water(void)
{
    u32 val = 0;
    u8 times = 10;
    u8 count;
    for (count = 0; count < times; count++)
    {
        val += ADC_ConvertedValue[0]; // 获取DMA通道值
        delay_ms(5);
    }
    return val / times;
}

// 得到ADC采样内部传感器的值
// 取10次，然后平均
u16 Get_ADCValue_MQ2(void)
{
    u32 val = 0;
    u8 times = 10;
    u8 count;
    for (count = 0; count < times; count++)
    {
        val += ADC_ConvertedValue[1]; // 获取DMA通道的值
        delay_ms(5);
    }
    return val / times;
}

u16  Get_ADCValue_Comm(int i) {
    u32 val = 0;
    u8 times = 10;
    u8 count;
    for (count = 0; count < times; count++)
    {
        val += ADC_ConvertedValue[i]; // 获取DMA通道的值
        delay_ms(5);
    }
    return val / times;
}

u16  Get_ADCValue_Height_PB1(void) {
    u32 val = 0;
    u8 times = 10;
    u8 count;
    for (count = 0; count < times; count++)
    {
        val += ADC_ConvertedValue[2]; // 获取DMA通道的值
        delay_ms(5);
    }
    return val / times;
}


// 传感器校准函数
void MQ2_PPM_Calibration(float RS)
{
    R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
}

// MQ2传感器数据处理
float MQ2_GetPPM(void)
{
    float Vrl, RS, ppm;
    Vrl = 3.3f * Get_ADCValue_MQ2() / 4096.f;
    Vrl = ((float)((int)((Vrl + 0.005) * 100))) / 100;
    RS = (3.3f - Vrl) / Vrl * RL;

    if (times_mq == 1) // 获取系数执行时间，300ms前进行校准
    {
        R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
        times_mq = 0;
    }

    ppm = 613.9f * pow(RS / R0, -2.074f);
    // ppm = pow(11.5428 * 35.904 * Vrl/(25.5-5.1* Vrl),0.6549);
    // https://blog.csdn.net/qq_35952136/article/details/95589074
    return ppm;
}

// Water Sensor水位传感器不仅可以测量水位，还可以测量是否有水，当测量是否有水时，
// 直接检测输出端引脚，若检测为0，则显示没有水，若检测到1，则有水；

/* ---------------------------------------------------------------- */
/**
 * @Name    Water_GPIO_Config
 * @param   None
 * @author  DavidNan
 * @Data    2023-03-03
 * <description> :配置LED用到的I/O口
 **/
/* ---------------------------------------------------------------- */

void Water_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* ---------------------------------------------------------------- */
/**
 * @Name    Water_state
 * @param   GPIOx: [输入/出]
 **			 GPIO_Pin_x: [输入/出]
 * @author  DavidNan
 * @Data    2023-03-03
 * <description> :
 **/
/* ---------------------------------------------------------------- */

int Water_state(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin_x)
{

    if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin_x) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void Water_state_printf()
{
    if (Water_state(GPIOA, GPIO_Pin_0) == 0)
    {
        printf("\r\n有水");
//		LED2 = 0; // D2指示灯亮
    }
    else
    {
        printf("\r\n无水");
//		LED2 = 1; // D2指示灯灭
    }
}
//瞬时流量 = (Q_max - Q_min) / (V_max - V_min) * (电压 - V_min) + Q_min
//为了得到瞬时流量和电压之间的线性关系，我们可以利用最小输出时和最大输出时的数据来进行线性拟合。
// 假设我们得到的最小输出时的电压为 V_min，
// 对应的瞬时流量为 Q_min，最大输出时的电压为 V_max，对应的瞬时流量为 Q_max，那么可以使用以下公式进行线性拟
float Get_Rate_Of_Flow_1()
{

    float adc_value,DC,data,voltage,flow_rate ;
    adc_value = Get_ADCValue_Comm(0); // 获取ADC采集到的值
    
    // 将ADC值转换为电压
    voltage = (adc_value / 4095.0) * 3.3;
    flow_rate = 1.5 * voltage - 0.45;
    if(voltage>3){
        flow_rate=0;
    }
    if(voltage<0.4){
        flow_rate=0;
    }
	// DEBUG_LOG("SRC AD:%0.1f",adc_value);
    // DEBUG_LOG("DC:%0.1f",voltage);
     DEBUG_LOG("rate:%f mL/min ",flow_rate );
    return flow_rate; //mL/s
}

float Get_Rate_Of_Flow_2() {
    float temp1,DC;
    temp1 =Get_ADCValue_Comm(1); // 水流量2
    DC = temp1 * 0.0806;

    //DEBUG_LOG("rate:%f L/min ,", temp1);
    //DEBUG_LOG("DC:%f V ,", DC);
    return temp1;
}

float Water_Height_PA4() {

    float temp1,K,DC,Height;
    temp1 = Get_ADCValue_Comm(3); // 水位获取
    Height=0.087*temp1-59.590;
    if(Height<0){
        Height=0;
    }
    DEBUG_LOG("ADC:%0.1f",temp1);
    DEBUG_LOG("Height:%0.1f",Height);
    return Height;

}

float Water_Height_PB1() {

    float temp1,K,DC,Height;
    temp1 = Get_ADCValue_Comm(2); // 水位获取
    K = temp1 * 0.0806;

    //关于电压变换我选择简单的映射手段：以20cm的水桶深度来算
    //没放水时DC:34.738V,水涨到20cm时DC:44.894V
    //float value = (raw_value-34.738)/(44.894-34.738);
    //DC =value*5.0f;
    //由于准度可能不够，需要减去一点误差值
    DC=((K-34.738)/(44.894-34.738))*5.0f;
    Height = ((temp1-431)/(557-431))*20.0f;
    //DEBUG_LOG("Height2:%f mm ,", Height-1.65);
    //DEBUG_LOG("DC:%f V ,", DC-0.31);
    return temp1;

}

float kValue=1.0;
float TU=0.0;
float TU_value=0.0;
float TU_calibration=0.0;
float temp_data=0.0;
float K_Value=3047.19;

u8 Get_ADCValue_turbidity() {

    float temp,DC,tds;
    
	temp=(float)Get_ADCValue_Comm(2); // 读取转换的AD值
	TU =temp/4096*3.3;
	//DEBUG_LOG("src AD:%f",temp);
	//DEBUG_LOG("Turn AD:%f",TU);
    TU_calibration=-0.0192*(temp_data/10-25)+TU;//U+dU,dU为温度引起的电压差，TU为电压
    TU_value=-865.68*TU_calibration + K_Value-660;//TU = -865.68*U+K(浑浊度与模块输出电压关系，U为当前温度输出电压，K为结局)
	//DEBUG_LOG("DC:%f",TU_calibration);

    if(TU_value<=0) {
        TU_value=0;
    }
    if(TU_value>=3000) {
        TU_value=3000;
    }
	DEBUG_LOG("tds:%f",TU_value);
	//delay_ms(3500);
    return TU_value;

}

u8 Get_ADCValue_PH(void){
	float temp,DC,tds,PH_Value;
	temp=(float)Get_ADCValue_Comm(4); // 读取转换的AD值XX        
	DC =(temp*3.3)/4096;
	PH_Value=DC;
	//DEBUG_LOG("src AD:%f",temp);
	//DEBUG_LOG("Turn AD:%f",DC);
    PH_Value = -5.9647*PH_Value+22.255+5;//公式转换PH值
	if(PH_Value<=0) PH_Value=0;//PH值小于0校正为0
	else if(PH_Value>=14) PH_Value=14;//PH值大于14校正为14
	DEBUG_LOG("PH:%f",PH_Value);
	//delay_ms(6000);
    return PH_Value;
}


u8 Water_Height2() {//水深2
   float temp1,K,DC,Height;
    temp1 = Get_ADCValue_Comm(6); // 水位获取
    Height=0.087*temp1-59.590;
    if(Height<0){
        Height=0;
    }
    DEBUG_LOG("ADC:%0.1f",temp1);
    DEBUG_LOG("Height2:%0.1f",Height);
    return Height;

}


