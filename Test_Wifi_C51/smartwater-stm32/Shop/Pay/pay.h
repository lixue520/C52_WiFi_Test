#ifndef __PAY_H
#define __PAY_H
#include "sys.h"
#include "rc522.h"
//读一次,拿走计算值==>阿里云==>mysql
//再读一次,拿走mysql==>mqtt==>IC卡
typedef struct {
    unsigned char ID[16];//用户的ID，在扇区1的，0块
    unsigned char name[16];//用户的姓名,在扇区1的，1块
    unsigned char money[16];//用户的余额，在扇区1的,2块
    unsigned char password[16];//用户密码,在扇区1的，3块
} IC;
//测试与注册，现在作为测试读写
void test_read_write(void);
void init_IC(unsigned char user_snr,unsigned char* user_id,unsigned char* user_name,unsigned char* user_money,unsigned char* user_password);//新卡注册
IC* ReadCard(unsigned char user_snr);//读卡
unsigned char writeCard(unsigned char user_snr,unsigned char user_n,unsigned char* data,unsigned char* password);//将数据填写到某一扇区的某块
char* add_money(unsigned char* money,double add);
char* sub_money(unsigned char* money,double sub);
IC* add_or_sub(unsigned char user_snr,unsigned char pay_type, double amount);//输入扇区号,支付操作类型,操作金额
void printIC(IC *ic);//打印读出值
void MoneyChangeToArr(void);//数值转换工具
void changeUserPwd(void);//修改密码
void wallet_Init(void);//初始化白卡,交钱默认设置金额10元
#endif


