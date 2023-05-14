#ifndef __PAY_H
#define __PAY_H
#include "sys.h"
#include "rc522.h"
//��һ��,���߼���ֵ==>������==>mysql
//�ٶ�һ��,����mysql==>mqtt==>IC��
typedef struct {
    unsigned char ID[16];//�û���ID��������1�ģ�0��
    unsigned char name[16];//�û�������,������1�ģ�1��
    unsigned char money[16];//�û�����������1��,2��
    unsigned char password[16];//�û�����,������1�ģ�3��
} IC;
//������ע�ᣬ������Ϊ���Զ�д
void test_read_write(void);
void init_IC(unsigned char user_snr,unsigned char* user_id,unsigned char* user_name,unsigned char* user_money,unsigned char* user_password);//�¿�ע��
IC* ReadCard(unsigned char user_snr);//����
unsigned char writeCard(unsigned char user_snr,unsigned char user_n,unsigned char* data,unsigned char* password);//��������д��ĳһ������ĳ��
char* add_money(unsigned char* money,double add);
char* sub_money(unsigned char* money,double sub);
IC* add_or_sub(unsigned char user_snr,unsigned char pay_type, double amount);//����������,֧����������,�������
void printIC(IC *ic);//��ӡ����ֵ
void MoneyChangeToArr(void);//��ֵת������
void changeUserPwd(void);//�޸�����
void wallet_Init(void);//��ʼ���׿�,��ǮĬ�����ý��10Ԫ
#endif


