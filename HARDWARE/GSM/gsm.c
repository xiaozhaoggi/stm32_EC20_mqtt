#include "stm32f10x.h"
#include "led.h"
#include <stdio.h>                    /* standard I/O .h-file                */
#include <ctype.h>                    /* character functions                 */
#include <string.h>                   /* string and memory functions         */
#include "delay.h"
#include "gsm.h"
#include "usart.h"
extern uchar phone_num[],TC_FSNR[];
void delay_GSM(uint i)
{
	unsigned int i_delay,j_delay;
   	for(i_delay=0;i_delay<i;i_delay++)
     	{   
       	for(j_delay=0;j_delay<1000;j_delay++){;}
        }
} 
//------------------------------------------------
//���ļ�Ϊ����Ӻ�������
//�Ӻ�������Ϊ��׼����ATָ���
//------------------------------------------------
//-----Ŀ���ֻ���ַ-------------------------------
//���������˭�����ţ���Ͱ�˭�ĺ���д�������Լ��ĺ�д���
//����ô˵�������˰�!
//------------------------------------------------


//�͸���2������Ŀǰ�İ汾��д���ĺ�Ҳ�ܷ����ţ�д���ԾͲ�д��
//---------------------------------------------------------------

//-----���ĺŵ�ַ---------------
//����Ϣ���ĺţ��ʹ�绰û��ϵ��д����Ҳ�ܴ�绰��
//���ֻ�ǹ�ϵ���Ͷ��ŵĺ��룬��Ҫ��10086���ƶ�������10010����ͨ����ѯ�����߰ٶȣ�GOOGLE������
//ǧ���д�Լ����ֻ����ˡ�
//��������Ϻ����ڹ�����Ŀ���ȥ������ʵ�飬��ô��д�Ǹ��������ĺţ�Ҳ���ǹ��ݵģ����Ǳ�����!!
//�Ҿ�����˵�Ĺ������˰ɡ�
//----------------------------------------------------------------------------------------------
uchar const TC_ZXH2[]={"+8613010395500"};        //�����ͨ�������ĺš�
//uchar code TC_ZXH2[]={"+8613800220500"};      //����ƶ��������ĺš�
//-------�����ط���Ҫ�Ҷ�-----------------------------------------------------
uchar const TC_MDSJ1[]={"AT+CMGS="};             //Ŀ���ֻ���ַ��
uchar const TC_ZXH1[]={"AT+CSCA="};              //�������ĺš�
//-------TEXT����������-----------------------------------------------------

//-------------------��ʱ-----------------------------------------------------
/*void delay_GSM(uint i)
{
	unsigned int i_delay,j_delay;
   	for(i_delay=0;i_delay<i;i_delay++)
     	{   
       	for(j_delay=0;j_delay<1000;j_delay++){;}
        }
} */
//-------------------���ý��ն�������------------------------------------------------
void SMS_CONFIG()
{
  	USART2_SendStr("AT+CNMI=2,1,0\r\n");//���úú�����жϽ��ն�����Ϣ
    GSM_CON=SMST;					   //SMST=0X09
} 
//-------------------�رջ���----------------------------
void ATE0()
{

		USART2_Write(USART2,"ATE0\r\n",strlen("ATE0\r\n"));// 
    GSM_CON=RST;          			   //RST=0X01
}
//-------------------д�ֻ���������-----------------------------
void ATNAME()
{//I AM OK
//uchar i;
    // 	delay_GSM(2000);
	USART2_SendStr(TC_FSNR);
//	USART1_SendStr(TC_FSNR);
//	USART2_Write(USART2,TC_FSNR,strlen(TC_FSNR));// 
//	xing_dim=0;

  	delay_GSM(100);

    USART2_SendOneChar(0X1A);//����Ϣ���ݷ��ͽ�����ʶ 
//    USART2_SendOneChar(0X0D);//�ո�
//    USART2_SendOneChar(0X0A);//���У��ո�+����=�س���
    GSM_CON=ATDAT;         //ATDAT=0X08
}
//----------------------------дĿ���ֻ���----------------------------------------
void AT_CMGS()
{
//AT+CMGS=13132061066
	//uchar i;
	delay_GSM(2000);
	//USART2_Write(USART2,"AT+CMGS=",strlen("AT+CMGS="));// 
	USART2_SendStr("AT+CMGS=");
  	USART2_SendOneChar(0X22);//˫����
	//USART2_Write(USART2,"15620323981",strlen("15620323981"));// 
	USART2_SendStr("18698322577");
// 	for(i=0;i<=sizeof(TC_MDSJ2)-1;i++)
//    	{
//    	USART2_SendOneChar(TC_MDSJ2[i]);//���������Ŀ���ֻ���
 //   	} 
	  	delay_GSM(2100);
  	USART2_SendOneChar(0X22);//˫���� 
  	USART2_SendOneChar(0X0D);//�ո�
  	USART2_SendOneChar(0X0A);//����
	//

 	GSM_CON=CMGS;			//CMGS=0X06
}
//----------------------------д��Ϣ���ĺ�-----------------------------------------
void AT_CSCA()
{//AT+CSCA=+8613010130500
	uchar i;
	
	USART2_Write(USART2,"AT+CSCA=",strlen("AT+CSCA="));// 
    USART2_SendOneChar(0X22);//˫���� 
 	for(i=0;i<sizeof(TC_ZXH2)-1;i++)
		{
 		USART2_SendOneChar(TC_ZXH2[i]);//���ö���Ϣ���ĺ�
 		} 
 	USART2_SendOneChar(0X22);//˫���� 
   	USART2_SendOneChar(0X0D);//�ո�
    USART2_SendOneChar(0X0A);//�س�
    GSM_CON=CSCA;			//CSCA=0X07
}

//----------------------------AT����״̬��ѯָ��-----------------------------------
void AT_CREG()
{

	USART2_Write(USART2,"AT+CREG?\r\n",strlen("AT+CREG?\r\n"));// 
  	GSM_CON=CREG;	//CREG=0X04
}
//----------------------------AT���ո�ʽָ��---------------------------------------
void AT_CMGF()
{//AT+CMGF=1
	
	USART2_Write(USART2,"AT+CMGF=1\r\n",strlen("AT+CMGF=1\r\n"));// 
  	GSM_CON=CMGF;//CMGF=0X02
}
//----------------------------AT����Ϣ��ȡָ��-------------------------------------
void AT_CMGR()
{//AT+CMGR=1
	
	USART2_Write(USART2,"AT+CMGR=1\r\n",strlen("AT+CMGR=1\r\n"));// 
  	GSM_CON=CMGR;//CBGR=0X05
}
//----------------------------AT����Ϣɾ��ָ��-------------------------------------
void AT_CMGD()
{//AT+CMGD=1
	
	USART2_Write(USART2,"AT+CMGD=1\r\n",strlen("AT+CMGD=1\r\n"));// 
  	GSM_CON=CMGD;//CMGD=OXO3
}










