#ifndef __GSM_H
#define __GSM_H	 
#include "sys.h"
#include "delay.h"
#include "usart.h"

//-----Ŀ���ֻ���ַ-------------------------------
//���������˭�����ţ���Ͱ�˭�ĺ���д�������Լ��ĺ�д���
//����ô˵�������˰�!
#define RST      0X01
#define CMGF     0X02
#define CMGD     0X03
#define CREG     0X04
#define CMGR     0X05
#define CMGS     0X06
#define CSCA     0X07
#define ATDAT    0X08
#define SMST     0X09
#define GSMRET  0X00
#define uchar unsigned char
#define uint unsigned int
	#define  TELNUM 18698322577
 void USART2_SendOneChar(u8);
void USART2_SendStr(u8*);  
	void USART1_SendOneChar(u8);
void USART1_SendStr(u8*); 
//------------------------------------------------
//uchar  TC_MDSJ2[]={"15620323981"};        //
//uchar  TC_FSNR[]={"GSM SEND OK!"};	            //���Ͷ�������
extern uchar GSM_CON,SMS_NUM,SMS_RECEIVE,xing_dim;
extern uchar read_flag,FASONGCIXU;//���ʹ���;
extern uchar phone_num[],TC_FSNR[];
extern uchar i_receive,
      GSM_RE[],//��������
      i_GSM_RECE,
	  GSM_MAX,   //�������ֵ
	  i_GSM;
extern uchar i_receive2,
      GSM_RE2[],//��������
      i_GSM_RECE2,
	  GSM_MAX2,   //�������ֵ
	  i_GSM2;
void delay_GSM(uint);
#endif
