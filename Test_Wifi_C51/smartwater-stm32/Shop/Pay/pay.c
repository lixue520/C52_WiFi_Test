#include "pay.h"
#include "usart.h"
//C���Կ⺯��������ʵ����ֵת���Լ��Ӽ�����
//ʵ���߼�����IC���е����ݶ������浽�ṹ��,Ȼ�������ĳ�Ա�����޸ģ����Ҫ������λ����ָ�����
//����λ������д��Ĳ�������IC�����������µ����ݷ��͵�Զ�˷���������ͳ������ͼ�����ݣ��Ա��¼
//��ˮ���ѵļ�¼��Ȼ����λ��Ҳ�����·�ָ���޸�IC�е��������ݣ��Լ�����ָ����￪���˼����ӿ�
//����ʵ�ֶԿ��Ķ�д�������Կ��ĳ�ʼ��(��������-�û���Ҫ���򱾳��Ŀ�����ʼ���200Ԫ)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
unsigned char snr, buf[16],buf_name[16], TagType[2], SelectedSnr[4], DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char name[9]="DavidNan";//Ĭ������
unsigned char money[9] = "200.0";//�������
char* money_str = (char*)money;//�������������,�������������û��������
unsigned char password[6]="123456";//���޸����룬��������������������Ҫֱ���޸ģ�������ܽ���������
unsigned char password_16[6] = {0x12, 0x34, 0x56, 0x00, 0x00, 0x00};//123456��16������ʽ
extern unsigned char SN[4];//��ʱID���ţ������޸�ID��һ��IDȷ��һ���û����Ա�IC���Ļ���
extern u8 kid[9];//����ID
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
  * @param   user_snr: �Զ�������
**			 user_id: �Զ���ID
**			 user_name: �Զ�������
**			 user_money: �Զ�����
**			 user_password: �Զ�������
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> :ʵ��Զ�̻��߿��ӻ������ע�����ֵ��������뿴onenet�Ľ��ճ���
 **/
/* ---------------------------------------------------------------- */

void init_IC(unsigned char user_snr,unsigned char* user_id,unsigned char* user_name,unsigned char* user_money,unsigned char* user_password) {
    unsigned char status;
// Ѱ��
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL����Ѱ�����������п���TagTypeΪ���صĿ�����
    if(!status)
    {
        // ����ͻ
        status = PcdAnticoll(SelectedSnr);
        ShowID(SelectedSnr);
        for(int i=0; i<9; i++) {
            buf[i]=kid[i];//������ʲô����
            buf_name[i]=user_name[i];
        }

        if(!status)
        {
            // ѡ��
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                // ��֤
                snr = user_snr;    // ������2
                //��֤A��Կ�����ַ���������룬�����к� ###��д��
                status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // У��1�������룬����λ��ÿһ������3��
//				status =PcdAuthState(KEYB, (snr*4+3), DefaultKey, SelectedSnr);
                // ��֤A��Կ�����ַ���������룬�����к�
                {
                    if(!status)
                    {
                        //��д��
                        //status = PcdRead((snr*4+0), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        DEBUG_LOG("ע���С�������");
                        status = PcdWrite((snr*4+0), user_id);    // д����ID
                        status = PcdWrite((snr*4+1), buf_name);   // д����name
                        status = PcdWrite((snr*4+2), user_money); // д����money
                        //status = PcdWrite((snr*4+3), DefaultKey); // д����password

                        DEBUG_LOG("===========ICע��ɹ�============");
                        status = PcdRead((snr*4+0), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("1.������IDֵ�ǣ� %s\n",buf);
                            //WaitCardOff();//�ȴ����뿪
                        }
                        status = PcdRead((snr*4+1), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("2.�������û����ǣ� %s\n",buf);
                            //WaitCardOff();//�ȴ����뿪
                        }
                        status = PcdRead((snr*4+2), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("3.����������ǣ� %s\n",buf);
                            //WaitCardOff();//�ȴ����뿪
                        }
                        status = PcdRead((snr*4+3), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("4.�����Ŀ����ǣ� %s\n",buf);
                            WaitCardOff();//�ȴ����뿪
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
  * @param   : [����/��]
  * @author  DavidNan
  * @Data    2023-03-29
  * <description> :�򵥲���һ�¶�д����
  *snr*4��ָ�����ţ�Sector Number������4��������ʾ�������ĵ�һ����š���MIFARE Classic 1K��Ƭ�У�ÿ����������4���飬
  *���ÿ�������ĵ�һ����ſ��Ա�ʾΪsnr*4��ͨ������õ��ÿ�ź󣬾Ϳ��Ը�����Ҫ��һ���������Ҫ��д�Ŀ�š�
  *snr*4+n,����n�ķ�Χ0-3;��snr=1ʱ��ʾ��������1��n=3��ʾ����1�����ĵ����飬����ŵ��ǿ��ƿ��Լ����롣
 **/
/* ---------------------------------------------------------------- */
void test_read_write() {
    unsigned char status;
// Ѱ��
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL����Ѱ�����������п���TagTypeΪ���صĿ�����
    if(!status)
    {
        // ����ͻ
        status = PcdAnticoll(SelectedSnr);
        ShowID(SelectedSnr);
        for(int i=0; i<9; i++) {
            buf[i]=kid[i];//������ʲô����
            buf_name[i]=name[i];
        }


        if(!status)
        {
            // ѡ��
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                // ��֤
                snr = 2;    // ������1
                //��֤A��Կ�����ַ���������룬�����к� ###��д��
                status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // У��1�������룬����λ��ÿһ������3��
//				status =PcdAuthState(KEYB, (snr*4+3), DefaultKey, SelectedSnr);
                // ��֤A��Կ�����ַ���������룬�����к�
                {
                    if(!status)
                    {
                        //��д��
                        //status = PcdRead((snr*4+0), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16


                        status = PcdWrite((snr*4+0), buf);   // д������buf[0]-buf[16]д��1����0��
                        status = PcdWrite((snr*4+1), buf_name);   // д������buf[0]-buf[16]д��1����0��
                        status = PcdWrite((snr*4+2), money); //�����ʼ���
                        //status = PcdWrite((snr*4+3), password);

                        status = PcdRead((snr*4+0), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("1.������IDֵ�ǣ� %s\n",buf);
                            //WaitCardOff();//�ȴ����뿪
                        }
                        status = PcdRead((snr*4+1), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("2.�������û����ǣ� %s\n",buf);
                            //WaitCardOff();//�ȴ����뿪
                        }
                        status = PcdRead((snr*4+2), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("3.����������ǣ� %s\n",buf);
                            //WaitCardOff();//�ȴ����뿪
                        }
                        status = PcdRead((snr*4+3), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16
                        if(!status)
                        {
                            DEBUG_LOG("4.�����Ŀ����ǣ� %s\n",buf);
                            WaitCardOff();//�ȴ����뿪
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
  * <description> :����IC���е����ݣ��Ա��ϴ�������
  *   N������N������롢�����кš��������N����N�����ݵ�buf[0]-buf[16]
 **/
/* ---------------------------------------------------------------- */

IC* ReadCard(unsigned char user_snr) {
    IC *ic = malloc(sizeof(IC));
    if (ic == NULL) {
        DEBUG_LOG("�����ڴ�ʧ��");
        WaitCardOff();
    }
    //ʹ��malloc�����ڶ��Ϸ�����һ��IC�ṹ����ڴ�ռ䣬
    //����ָ��icָ����ڴ�ռ䡣����ʹ��memset�������ڴ�ռ��ʼ��Ϊ0
    //�������ָ��ṹ���ָ���ʼ���ڴ���ܻᵼ���ڴ����
    memset(ic, 0, sizeof(IC));
    unsigned char status;
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL����Ѱ�����������п���TagTypeΪ���صĿ�����
    if(!status)
    {
        // ����ͻ
        status = PcdAnticoll(SelectedSnr);
        ShowID(SelectedSnr);
        if(!status)
        {
            // ѡ��
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                // ��֤
                snr =  user_snr;    // ������
                status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // У��1�������룬����λ��ÿһ������3��
                // ��֤A��Կ�����ַ���������룬�����к�
                {
                    if(!status)
                    {
                        status = PcdRead((snr*4+0), ic->ID);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
                        status = PcdRead((snr*4+1), ic->name);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
                        status = PcdRead((snr*4+2), ic->money);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
                        status = PcdRead((snr*4+3), ic->password);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
						memcpy(user_id, ic->ID, sizeof(ic->ID));
						memcpy(user_name, ic->name, sizeof(ic->name));
						memcpy(user_money, ic->money, sizeof(ic->money));
						memcpy(user_password, ic->password, sizeof(ic->password));
                        if(!status)
                        {
                            printIC(ic);//��ӡȫ��
                            WaitCardOff();
                        } else {
                            DEBUG_LOG("IC������ʧ��");
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
  * @param   add: [����/��]
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> :�õ�IC�������󣬽����ݴ洢menoyȫ�ֱ����У���moneyֵ���мӷ�����
 **/
/* ---------------------------------------------------------------- */

char* add_money(unsigned char* money,double add) {
	char* money_str = (char*)money;
    // ���ַ���ת��Ϊ������
    double current_money = atof(money_str);
    // ��������ת��Ϊ��������λΪ��
    int current_money_cent = (int)(current_money * 100);
    int add_cent = (int)(add * 100);
    // ���мӷ�����
    int result_cent = current_money_cent + add_cent;
    // ������ת��Ϊ����������λΪԪ
    double result = (double)result_cent / 100;
    // ��������ת��Ϊ�ַ���
    sprintf(money_str, "%.1f", result);
	return money_str;
}


/* ---------------------------------------------------------------- */
/**
  * @Name    sub_money
  * @param   sub: [����/��]
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> :�õ�IC�������󣬽����ݴ洢menoyȫ�ֱ����У���moneyֵ���м�������
 **/
/* ---------------------------------------------------------------- */

char* sub_money(unsigned char* money,double sub) {
	char* money_str = (char*)money;
    // ���ַ���ת��Ϊ������
    double current_money = atof(money_str);
    // ��������ת��Ϊ��������λΪ��
    int current_money_cent = (int)(current_money * 100);
    int sub_cent = (int)(sub * 100);
    // ���м�������
    int result_cent = current_money_cent - sub_cent;
    if(result_cent < 0) {
        printf("����\n");
        
    }
    // ������ת��Ϊ����������λΪԪ
    double result = (double)result_cent / 100;
    // ��������ת��Ϊ�ַ���
    sprintf(money_str, "%.1f", result);
	return money_str;
}

/* ---------------------------------------------------------------- */
/**
  * @Name    printIC
  * @param   ic: [����/��]
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> :��ӡ������ֵ
 **/
/* ---------------------------------------------------------------- */

void printIC(IC *ic) {
    DEBUG_LOG("ID: %s", ic->ID);
    DEBUG_LOG("Name: %s", ic->name);
    DEBUG_LOG("Money: %s ��", ic->money);
    DEBUG_LOG("Password: %s", ic->password);
}

IC* add_or_sub(unsigned char user_snr,unsigned char pay_type,double amount){

	IC *ic = malloc(sizeof(IC));
    if (ic == NULL) {
        DEBUG_LOG("�����ڴ�ʧ��");
        WaitCardOff();
    }
    //ʹ��malloc�����ڶ��Ϸ�����һ��IC�ṹ����ڴ�ռ䣬
    //����ָ��icָ����ڴ�ռ䡣����ʹ��memset�������ڴ�ռ��ʼ��Ϊ0
    //�������ָ��ṹ���ָ���ʼ���ڴ���ܻᵼ���ڴ����
    memset(ic, 0, sizeof(IC));
    unsigned char status;
	unsigned char* new_money;
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL����Ѱ�����������п���TagTypeΪ���صĿ�����
    if(!status)
    {
        // ����ͻ
        status = PcdAnticoll(SelectedSnr);
        ShowID(SelectedSnr);
        if(!status)
        {
            // ѡ��
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                // ��֤
                snr =  user_snr;    // ������
                status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);    // У��1�������룬����λ��ÿһ������3��
                // ��֤A��Կ�����ַ���������룬�����к�
                {
                    if(!status)
                    {
                        status = PcdRead((snr*4+0), ic->ID);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
                        status = PcdRead((snr*4+1), ic->name);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
                        status = PcdRead((snr*4+2), ic->money);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
                        status = PcdRead((snr*4+3), ic->password);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
						
						if(pay_type==0){
							new_money=(unsigned char*)sub_money(ic->money,amount);
							spent=0;
						}else{
							new_money=(unsigned char*)add_money(ic->money,amount);
							IC_recharge_money=0;//��ֵ����
							spent=0;
						}
						status = PcdWrite((snr*4+2), new_money);

                        if(!status)
                        {
                            printIC(ic);//��ӡȫ��
                            WaitCardOff();
                        } else {
                            DEBUG_LOG("IC������ʧ��");
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
  * @param   user_snr: [����/��] 
**			 user_n: [����/��] 
**			 data: [����/��] 
**			 password: [����/��] 
  * @author  DavidNan
  * @Data    2023-03-30
  * <description> : ��dataд��user_snr������user_n�飬������ʱ��д����Ϊ���޸����뱻��
 **/
/* ---------------------------------------------------------------- */

unsigned char writeCard(unsigned char user_snr,unsigned char user_n,unsigned char* data,unsigned char* password){

	unsigned char status;
    status= PcdRequest( REQ_ALL, TagType );     // REQ_ALL����Ѱ�����������п���TagTypeΪ���صĿ�����
    if(!status)
    {
        // ����ͻ
        status = PcdAnticoll(SelectedSnr);
        if(!status)
        {
            // ѡ��
            status=PcdSelect(SelectedSnr);
            if(!status)
            {
                status = PcdAuthState(KEYA, (user_snr*4+3), DefaultKey, SelectedSnr);    // У��1�������룬����λ��ÿһ������3��
                // ��֤A��Կ�����ַ���������룬�����к�
                {
                    if(!status)
                    {
                        status = PcdWrite((user_snr*4+user_n), data);   
                       
                        if(!status)
                        {
                            WaitCardOff();
                        } else {
                            DEBUG_LOG("IC��д��ʧ��");
                        }
                    }
                }
            }
        }
    }
	return Write_Data_FAIL;//дʧ��
}



