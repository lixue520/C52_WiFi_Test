#include "pay.h"
#include "usart.h"
//C语言库函数，用来实现数值转换以及加减操作
//实现逻辑，将IC卡中的数据读出，存到结构体,然后对里面的成员进行修改，这个要根据上位机的指令操作
//将上位机进行写后的操作存入IC卡，并将更新的数据发送到远端服务器进行统计生产图表数据，以便记录
//出水消费的记录，然后上位机也可以下发指令修改IC中的扇区数据，以及其他指令，这里开放了几个接口
//可以实现对卡的读写操作、对卡的初始化(出厂设置-用户需要购买本厂的卡，初始金额200元)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
unsigned char snr, buf[16],buf_name[16], TagType[2], SelectedSnr[4], DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char name[9]="DavidNan";//默认姓名
unsigned char money[9] = "200.0";//出厂金额
char* money_str = (char*)money;//解决编译器警告,这里用来缓存用户金额数据
unsigned char password[6]="123456";//待修改密码，可以利用其他扇区，不要直接修改，否则可能将扇区锁掉
unsigned char password_16[6] = {0x12, 0x34, 0x56, 0x00, 0x00, 0x00};//123456的16进制形式
extern unsigned char SN[4];//临时ID卡号，用于修改ID，一个ID确定一个用户，以便IC卡的回收
extern u8 kid[9];//卡的ID
unsigned char Write_Data_FAIL=2;
extern unsigned char user_id[16];
extern unsigned char user_name[16];
extern unsigned char user_money[16];
extern unsigned char user_password[16];
extern double spent;
extern u8 IC_recharge_money;
/* ---------------------------------------------------------------- */
/**
  * @Name    init_IC
  * @param   user_snr: 自定义扇区
**			 user_id: 自定义ID
**			 user_name: 自定义姓名
**			 user_money: 自定义金额
**			 user_password: 自定义密码
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> :实现远程或者可视化界面的注册与充值，具体接入看onenet的接收程序
 **/
/* ---------------------------------------------------------------- */

void init_IC(unsigned char user_snr,unsigned char* user_id,unsigned char* user_name,unsigned char* user_money,unsigned char* user_password) {
    unsigned char status;
// 寻卡
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL代表寻天线区内所有卡。TagType为返回的卡类型
    if(!status)
    {
        // 防冲突
        status = PcdAnticoll(SelectedSnr);
        ShowID(SelectedSnr);
        for(int i=0; i<9; i++) {
            buf[i]=kid[i];//读到了什么。。
            buf_name[i]=user_name[i];
        }

        if(!status)
        {
            // 选卡
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                // 认证
                snr = user_snr;    // 扇区号2
                //验证A密钥，块地址，扇区密码，卡序列号 ###读写卡
                status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // 校验1扇区密码，密码位于每一扇区第3块
//				status =PcdAuthState(KEYB, (snr*4+3), DefaultKey, SelectedSnr);
                // 验证A密钥，块地址，扇区密码，卡序列号
                {
                    if(!status)
                    {
                        //读写卡
                        //status = PcdRead((snr*4+0), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        DEBUG_LOG("注册中。。。。");
                        status = PcdWrite((snr*4+0), user_id);    // 写卡，ID
                        status = PcdWrite((snr*4+1), buf_name);   // 写卡，name
                        status = PcdWrite((snr*4+2), user_money); // 写卡，money
                        //status = PcdWrite((snr*4+3), DefaultKey); // 写卡，password

                        DEBUG_LOG("===========IC注册成功============");
                        status = PcdRead((snr*4+0), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("1.读到的ID值是： %s\n",buf);
                            //WaitCardOff();//等待卡离开
                        }
                        status = PcdRead((snr*4+1), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("2.读到的用户名是： %s\n",buf);
                            //WaitCardOff();//等待卡离开
                        }
                        status = PcdRead((snr*4+2), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("3.读到的余额是： %s\n",buf);
                            //WaitCardOff();//等待卡离开
                        }
                        status = PcdRead((snr*4+3), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("4.读到的卡密是： %s\n",buf);
                            WaitCardOff();//等待卡离开
                        }

                    }
                }
            }
        }
    }
}

/* ---------------------------------------------------------------- */
/**
  * @Name    test_read_write
  * @param   : [输入/出]
  * @author  DavidNan
  * @Data    2023-03-29
  * <description> :简单测试一下读写功能
  *snr*4是指扇区号（Sector Number）乘以4，其结果表示该扇区的第一个块号。在MIFARE Classic 1K卡片中，每个扇区包含4个块，
  *因此每个扇区的第一个块号可以表示为snr*4。通过计算得到该块号后，就可以根据需要进一步计算出需要读写的块号。
  *snr*4+n,其中n的范围0-3;若snr=1时表示的是扇区1，n=3表示的是1扇区的第三块，这里放的是控制块以及密码。
 **/
/* ---------------------------------------------------------------- */
void test_read_write() {
    unsigned char status;
// 寻卡
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL代表寻天线区内所有卡。TagType为返回的卡类型
    if(!status)
    {
        // 防冲突
        status = PcdAnticoll(SelectedSnr);
        ShowID(SelectedSnr);
        for(int i=0; i<9; i++) {
            buf[i]=kid[i];//读到了什么。。
            buf_name[i]=name[i];
        }


        if(!status)
        {
            // 选卡
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                // 认证
                snr = 2;    // 扇区号1
                //验证A密钥，块地址，扇区密码，卡序列号 ###读写卡
                status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // 校验1扇区密码，密码位于每一扇区第3块
//				status =PcdAuthState(KEYB, (snr*4+3), DefaultKey, SelectedSnr);
                // 验证A密钥，块地址，扇区密码，卡序列号
                {
                    if(!status)
                    {
                        //读写卡
                        //status = PcdRead((snr*4+0), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16


                        status = PcdWrite((snr*4+0), buf);   // 写卡，将buf[0]-buf[16]写入1扇区0块
                        status = PcdWrite((snr*4+1), buf_name);   // 写卡，将buf[0]-buf[16]写入1扇区0块
                        status = PcdWrite((snr*4+2), money); //填入初始金额
                        //status = PcdWrite((snr*4+3), password);

                        status = PcdRead((snr*4+0), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("1.读到的ID值是： %s\n",buf);
                            //WaitCardOff();//等待卡离开
                        }
                        status = PcdRead((snr*4+1), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("2.读到的用户名是： %s\n",buf);
                            //WaitCardOff();//等待卡离开
                        }
                        status = PcdRead((snr*4+2), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("3.读到的余额是： %s\n",buf);
                            //WaitCardOff();//等待卡离开
                        }
                        status = PcdRead((snr*4+3), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("4.读到的卡密是： %s\n",buf);
                            WaitCardOff();//等待卡离开
                        }

                    }
                }
            }
        }
    }
}

/* ---------------------------------------------------------------- */
/**
  * @Name    ReadCard
  * @param   unsigned char user_snr
  * @author  DavidNan
  * @Data    2023-03-29
  * <description> :读出IC卡中的数据，以便上传阿里云
  *   N扇区第N块的密码、卡序列号、金额存放在N扇区N块数据到buf[0]-buf[16]
 **/
/* ---------------------------------------------------------------- */

IC* ReadCard(unsigned char user_snr) {
    IC *ic = malloc(sizeof(IC));
    if (ic == NULL) {
        DEBUG_LOG("分配内存失败");
        WaitCardOff();
    }
    //使用malloc函数在堆上分配了一个IC结构体的内存空间，
    //并将指针ic指向该内存空间。我们使用memset函数将内存空间初始化为0
    //如果不给指向结构体的指针初始化内存可能会导致内存崩溃
    memset(ic, 0, sizeof(IC));
    unsigned char status;
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL代表寻天线区内所有卡。TagType为返回的卡类型
    if(!status)
    {
        // 防冲突
        status = PcdAnticoll(SelectedSnr);
        ShowID(SelectedSnr);
        if(!status)
        {
            // 选卡
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                // 认证
                snr =  user_snr;    // 扇区号
                status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // 校验1扇区密码，密码位于每一扇区第3块
                // 验证A密钥，块地址，扇区密码，卡序列号
                {
                    if(!status)
                    {
                        status = PcdRead((snr*4+0), ic->ID);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
                        status = PcdRead((snr*4+1), ic->name);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
                        status = PcdRead((snr*4+2), ic->money);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
                        status = PcdRead((snr*4+3), ic->password);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
						memcpy(user_id, ic->ID, sizeof(ic->ID));
						memcpy(user_name, ic->name, sizeof(ic->name));
						memcpy(user_money, ic->money, sizeof(ic->money));
						memcpy(user_password, ic->password, sizeof(ic->password));
                        if(!status)
                        {
                            printIC(ic);//打印全部
                            WaitCardOff();
                        } else {
                            DEBUG_LOG("IC卡读卡失败");
                        }
                    }
                }
            }
        }
    }
	return ic;
	//free(ic);
}


/* ---------------------------------------------------------------- */
/**
  * @Name    add_money
  * @param   add: [输入/出]
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> :拿到IC卡读卡后，将数据存储menoy全局变量中，对money值进行加法操作
 **/
/* ---------------------------------------------------------------- */

char* add_money(unsigned char* money,double add) {
	char* money_str = (char*)money;
    // 将字符串转换为浮点数
    double current_money = atof(money_str);
    // 将浮点数转换为整数，单位为分
    int current_money_cent = (int)(current_money * 100);
    int add_cent = (int)(add * 100);
    // 进行加法运算
    int result_cent = current_money_cent + add_cent;
    // 将整数转换为浮点数，单位为元
    double result = (double)result_cent / 100;
    // 将浮点数转换为字符串
    sprintf(money_str, "%.1f", result);
	return money_str;
}


/* ---------------------------------------------------------------- */
/**
  * @Name    sub_money
  * @param   sub: [输入/出]
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> :拿到IC卡读卡后，将数据存储menoy全局变量中，对money值进行减法操作
 **/
/* ---------------------------------------------------------------- */

char* sub_money(unsigned char* money,double sub) {
	char* money_str = (char*)money;
    // 将字符串转换为浮点数
    double current_money = atof(money_str);
    // 将浮点数转换为整数，单位为分
    int current_money_cent = (int)(current_money * 100);
    int sub_cent = (int)(sub * 100);
    // 进行减法运算
    int result_cent = current_money_cent - sub_cent;
    if(result_cent < 0) {
        printf("余额不足\n");
        
    }
    // 将整数转换为浮点数，单位为元
    double result = (double)result_cent / 100;
    // 将浮点数转换为字符串
    sprintf(money_str, "%.1f", result);
	return money_str;
}

/* ---------------------------------------------------------------- */
/**
  * @Name    printIC
  * @param   ic: [输入/出]
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> :打印读出的值
 **/
/* ---------------------------------------------------------------- */

void printIC(IC *ic) {
    DEBUG_LOG("ID: %s", ic->ID);
    DEBUG_LOG("Name: %s", ic->name);
    DEBUG_LOG("Money: %s ￥", ic->money);
    DEBUG_LOG("Password: %s", ic->password);
}

IC* add_or_sub(unsigned char user_snr,unsigned char pay_type,double amount){

	IC *ic = malloc(sizeof(IC));
    if (ic == NULL) {
        DEBUG_LOG("分配内存失败");
        WaitCardOff();
    }
    //使用malloc函数在堆上分配了一个IC结构体的内存空间，
    //并将指针ic指向该内存空间。我们使用memset函数将内存空间初始化为0
    //如果不给指向结构体的指针初始化内存可能会导致内存崩溃
    memset(ic, 0, sizeof(IC));
    unsigned char status;
	unsigned char* new_money;
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL代表寻天线区内所有卡。TagType为返回的卡类型
    if(!status)
    {
        // 防冲突
        status = PcdAnticoll(SelectedSnr);
        ShowID(SelectedSnr);
        if(!status)
        {
            // 选卡
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                // 认证
                snr =  user_snr;    // 扇区号
                status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // 校验1扇区密码，密码位于每一扇区第3块
                // 验证A密钥，块地址，扇区密码，卡序列号
                {
                    if(!status)
                    {
                        status = PcdRead((snr*4+0), ic->ID);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
                        status = PcdRead((snr*4+1), ic->name);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
                        status = PcdRead((snr*4+2), ic->money);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
                        status = PcdRead((snr*4+3), ic->password);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
						
						if(pay_type==0){
							new_money=(unsigned char*)sub_money(ic->money,amount);
							spent=0;
						}else{
							new_money=(unsigned char*)add_money(ic->money,amount);
							IC_recharge_money=0;//充值结束
							spent=0;
						}
						status = PcdWrite((snr*4+2), new_money);

                        if(!status)
                        {
                            printIC(ic);//打印全部
                            WaitCardOff();
                        } else {
                            DEBUG_LOG("IC卡读卡失败");
                        }
                    }
                }
            }
        }
    }
	return ic;
}

/* ---------------------------------------------------------------- */
/**
  * @Name    writeCard
  * @param   user_snr: [输入/出] 
**			 user_n: [输入/出] 
**			 data: [输入/出] 
**			 password: [输入/出] 
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> : 将data写到user_snr扇区的user_n块，密码暂时乱写，因为怕修改密码被锁
 **/
/* ---------------------------------------------------------------- */

unsigned char writeCard(unsigned char user_snr,unsigned char user_n,unsigned char* data,unsigned char* password){

	unsigned char status;
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL代表寻天线区内所有卡。TagType为返回的卡类型
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
                status = PcdAuthState(KEYA, (user_snr*4+3), DefaultKey, SelectedSnr);    // 校验1扇区密码，密码位于每一扇区第3块
                // 验证A密钥，块地址，扇区密码，卡序列号
                {
                    if(!status)
                    {
                        status = PcdWrite((user_snr*4+user_n), data);   
                       
                        if(!status)
                        {
                            WaitCardOff();
                        } else {
                            DEBUG_LOG("IC卡写卡失败");
                        }
                    }
                }
            }
        }
    }
	return Write_Data_FAIL;//写失败
}



