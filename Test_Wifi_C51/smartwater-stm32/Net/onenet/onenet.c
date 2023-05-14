/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"

//C��
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "Pump.h"
#include "beep.h"
#define PROID		"huaqing_smarthome"

#define AUTH_INFO	"huaqing_smarthome_zui_jiandan"

#define DEVID		"huaqing_smarthome"


extern unsigned char esp8266_buf[128];
extern u8 alarmFlag;//�Ƿ񱨾��ı�־
extern u8 alarm_is_free;
extern double spent;//���ѽ��
extern u8 IC_recharge_money;//��ֵ��־,����λ�����ߴ�����Ļ����
extern double recharge;//��ֵ���
extern unsigned char user_name[16]; //IC��������


//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
    char ProductID[64];
    char Auth_Info[64];
    char DeviceID[64];

    u32 MCU_ID;

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

    unsigned char *dataPtr;

    _Bool status = 1;

    MCU_ID = *(vu32*)(0x1FFFF7E8);	//��ȡMCU��Ψһ��ʶ
   sprintf(ProductID,"%s_%X",PROID,MCU_ID);
	sprintf(Auth_Info,"%s_%X",AUTH_INFO,MCU_ID);
	sprintf(DeviceID,"%s_%X",DEVID,MCU_ID);

    UsartPrintf(USART_DEBUG,"OneNet_DevLink\r\n"
                "PROID: %s,	AUIF: %s,	DEVID:%s	--- ������....\r\n"
                ,ProductID, Auth_Info, DeviceID);

    if(MQTT_PacketConnect(ProductID, Auth_Info, DeviceID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
    {
        ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨

        dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
        if(dataPtr != NULL)
        {
            if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
            {
                switch(MQTT_UnPacketConnectAck(dataPtr))
                {
                case 0:
                    DEBUG_LOG("Tips:	���ӳɹ�\r\n");
                    status = 0;
                    break;

                case 1:
                    DEBUG_LOG("WARN:	����ʧ�ܣ�Э�����\r\n");
                    break;
                case 2:
                    DEBUG_LOG("WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");
                    break;
                case 3:
                    DEBUG_LOG("WARN:	����ʧ�ܣ�������ʧ��\r\n");
                    break;
                case 4:
                    DEBUG_LOG("WARN:	����ʧ�ܣ��û������������\r\n");
                    break;
                case 5:
                    DEBUG_LOG("WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");
                    break;

                default:
                    DEBUG_LOG("ERR:	����ʧ�ܣ�δ֪����\r\n");
                    break;
                }
            }
        }

        MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
    }
    else
        DEBUG_LOG("WARN:	MQTT_PacketConnect Failed\r\n");

    return status;

}

//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{

    unsigned char i = 0;

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���

    for(; i < topic_cnt; i++)
        DEBUG_LOG("Subscribe Topic: %s\r\n", topics[i]);

    if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
    {
        ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������

        MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
    }

}

//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_PUBLISH-��Ҫ����
//
//	˵����
//==========================================================
//void OneNet_Publish(const char *topic, const char *msg)
//{

//	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
//
//	DEBUG_LOG("Publish Topic: %s,Msg:%s", topic, msg);
//
//	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
//	{
//		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
//
//		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
//	}

//}
void OneNet_Publish(const char *topic, const char *msg)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};    //Э���
    const char *new_msg = msg + 2;                          //���Ե�ǰ�������ֽ�
    DEBUG_LOG("Publish Topic: %s,Msg:%s", topic, new_msg);
    if (MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, new_msg, strlen(new_msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0) {
        ESP8266_SendData(mqttPacket._data, mqttPacket._len);    //��ƽ̨���Ͷ�������
        MQTT_DeleteBuffer(&mqttPacket);                         //ɾ��
    }
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص����� Ҳ����*cmd
//
//	���ز�����	��
//
//	˵����
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���

    char *req_payload = NULL;
    char *cmdid_topic = NULL;

    unsigned short topic_len = 0;
    unsigned short req_len = 0;

    unsigned char type = 0;
    unsigned char qos = 0;
    static unsigned short pkt_id = 0;

    short result = 0;

    char *dataPtr = NULL;
    char numBuf[10];
    int num = 0;

    cJSON *json, *json_value,*some_value;

    type = MQTT_UnPacketRecv(cmd);
    switch(type)
    {
    case MQTT_PKT_CMD:															//�����·�

        result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
        if(result == 0)
        {
            DEBUG_LOG("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

            MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
        }
        break;

    case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ

        result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
        if(result == 0)
        {
            DEBUG_LOG("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
                      cmdid_topic, topic_len, req_payload, req_len);
            // �����ݰ�req_payload����JSON��ʽ����
            json = cJSON_Parse(req_payload);
            if (!json)UsartPrintf(USART_DEBUG,"Error before: [%s]\n",cJSON_GetErrorPtr());
            else
            {
                json_value = cJSON_GetObjectItem(json, "target");
                UsartPrintf(USART_DEBUG,"json_value: [%s]\r\n",json_value->string);
                UsartPrintf(USART_DEBUG,"json_value: [%s]\r\n",json_value->valuestring);
				
				if(strstr(json_value->valuestring,"money") != NULL)
                {
                    json_value = cJSON_GetObjectItem(json, "value");
					some_value = cJSON_GetObjectItem(json, "name");
                    if(strcmp(some_value->valuestring,(char *)user_name)==0) {
                     
						DEBUG_LOG("\r\n");
						DEBUG_LOG("%s   %s",some_value->valuestring,(char *)user_name);
						DEBUG_LOG(" user_name: %s add_money: %d",some_value->valuestring,json_value->valueint);
						recharge=json_value->valueint;
                        DEBUG_LOG("Tips:	money is added OK\r\n");
						IC_recharge_money=1;

                    }
                }
					if(strstr(json_value->valuestring,"pay") != NULL)
                {
                    json_value = cJSON_GetObjectItem(json, "value");
                    if(json_value->valueint) {
                     
						DEBUG_LOG("\r\n");
						DEBUG_LOG("pay %d",json_value->valueint);
						spent=json_value->valueint;
                        DEBUG_LOG("Tips:	money is paied OK\r\n");

                    }
                }

                if(strstr(json_value->valuestring,"ZW_Led") != NULL)
                {
                    json_value = cJSON_GetObjectItem(json, "value");
                    if(json_value->valueint) {
                        GPIO_ResetBits(GPIOB,GPIO_Pin_7); //�����߿�
                        DEBUG_LOG("Tips:	ZW_LED is oprn OK\r\n");

                    }
                    else {

                        GPIO_SetBits(GPIOB,GPIO_Pin_7); //�����߿�
                        DEBUG_LOG("Tips:	ZW_LED is close OK\r\n");
                    }
                }

                if(strstr(json_value->valuestring,"Beep") != NULL)
                {
                    json_value = cJSON_GetObjectItem(json, "value");
                    if(json_value->valueint) {
                        GPIO_SetBits(GPIOB, GPIO_Pin_5);//��������
                        DEBUG_LOG("Tips:	Beep is oprn OK\r\n");

                    }
                    else {

                        GPIO_ResetBits(GPIOB, GPIO_Pin_5);//��������
                        DEBUG_LOG("Tips:	Beep is close OK\r\n");
                    }
                }


                if(strstr(json_value->valuestring,"DZF") != NULL)
                {
                    json_value = cJSON_GetObjectItem(json, "value");
                    if(json_value->valueint) {
                        GPIO_SetBits(GPIOB,GPIO_Pin_8);//��ŷ�1
                        DEBUG_LOG("Tips:	DZF is oprn OK\r\n");

                    }
                    else {

                       GPIO_ResetBits(GPIOB,GPIO_Pin_8);//��ŷ�1 
                        DEBUG_LOG("Tips:	DZF is close OK\r\n");
                    }
                }

                if(strstr(json_value->valuestring,"DZF1") != NULL)
                {
                    json_value = cJSON_GetObjectItem(json, "value");
                    if(json_value->valueint) {
                        GPIO_SetBits(GPIOC,GPIO_Pin_13);//��ŷ�2
                        DEBUG_LOG("Tips:	DZF1 is oprn OK\r\n");

                    }
                    else {

                        GPIO_ResetBits(GPIOC,GPIO_Pin_13);//��ŷ�2
                        DEBUG_LOG("Tips:	DZF1 is close OK\r\n");
                    }
                }

                if(strstr(json_value->valuestring,"Pump") != NULL)
                {
                    json_value = cJSON_GetObjectItem(json, "value");
                    if(json_value->valueint) {

                        GPIO_SetBits(GPIOA,GPIO_Pin_11);//ˮ��1����
                        GPIO_ResetBits(GPIOA,GPIO_Pin_12);
                        DEBUG_LOG("Tips:	Pump is oprn OK\r\n");

                    }
                    else {

                        GPIO_ResetBits(GPIOA,GPIO_Pin_11);//ˮ��1����
                        GPIO_ResetBits(GPIOA,GPIO_Pin_12);
                        DEBUG_LOG("Tips:	Pump is close OK\r\n");
                    }
                }
                if(strstr(json_value->valuestring,"Pump1") != NULL)
                {
                    json_value = cJSON_GetObjectItem(json, "value");
                    if(json_value->valueint) {

                        GPIO_SetBits(GPIOB,GPIO_Pin_3);//ˮ��2����
                        GPIO_ResetBits(GPIOB,GPIO_Pin_4);
                        DEBUG_LOG("Tips:	Pump1 is oprn OK\r\n");
                    }
                    else {

                        GPIO_ResetBits(GPIOB,GPIO_Pin_3);//ˮ��2����
                        GPIO_ResetBits(GPIOB,GPIO_Pin_4);
                        DEBUG_LOG("Tips:	Pump1 is close OK\r\n");
                    }
                    alarm_is_free=0;//��λ�����Ƹ��ֶ����ƾ���һ�������ȼ� �������Զ����ƣ����Ҫ��alarm_is_free ��0
                }
//					if(json_value->valueint)//json_value > 0��Ϊ����
//					{
//						LED0 = 0;//��LED0
//					}
//					else
//					{
//						LED0 = 1;//�ر�LED0
//					}
            }
            cJSON_Delete(json);
        }
        break;

    case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack

        if(MQTT_UnPacketPublishAck(cmd) == 0)
            DEBUG_LOG("Tips:	MQTT Publish Send OK\r\n");

        break;

    case MQTT_PKT_PUBREC:														//����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ

        if(MQTT_UnPacketPublishRec(cmd) == 0)
        {
            DEBUG_LOG("Tips:	Rev PublishRec\r\n");
            if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
            {
                DEBUG_LOG("Tips:	Send PublishRel\r\n");
                ESP8266_SendData(mqttPacket._data, mqttPacket._len);
                MQTT_DeleteBuffer(&mqttPacket);
            }
        }

        break;

    case MQTT_PKT_PUBREL:														//�յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp

        if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
        {
            DEBUG_LOG("Tips:	Rev PublishRel\r\n");
            if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
            {
                DEBUG_LOG("Tips:	Send PublishComp\r\n");
                ESP8266_SendData(mqttPacket._data, mqttPacket._len);
                MQTT_DeleteBuffer(&mqttPacket);
            }
        }

        break;

    case MQTT_PKT_PUBCOMP:														//����Publish��Ϣ��ƽ̨����Rec���豸�ظ�Rel��ƽ̨�ٷ��ص�Comp

        if(MQTT_UnPacketPublishComp(cmd) == 0)
        {
            DEBUG_LOG("Tips:	Rev PublishComp\r\n");
        }

        break;

    case MQTT_PKT_SUBACK:														//����Subscribe��Ϣ��Ack

        if(MQTT_UnPacketSubscribe(cmd) == 0)
            DEBUG_LOG("Tips:	MQTT Subscribe OK\r\n");
        else
            DEBUG_LOG("Tips:	MQTT Subscribe Err\r\n");

        break;

    case MQTT_PKT_UNSUBACK:														//����UnSubscribe��Ϣ��Ack

        if(MQTT_UnPacketUnSubscribe(cmd) == 0)
            DEBUG_LOG("Tips:	MQTT UnSubscribe OK\r\n");
        else
            DEBUG_LOG("Tips:	MQTT UnSubscribe Err\r\n");

        break;

    default:
        result = -1;
        break;
    }

    ESP8266_Clear();									//��ջ���

    if(result == -1)
        return;

    dataPtr = strchr(req_payload, '}');					//����'}'

    if(dataPtr != NULL && result != -1)					//����ҵ���
    {
        dataPtr++;

        while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
        {
            numBuf[num++] = *dataPtr++;
        }

        num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ

    }

    if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }

}