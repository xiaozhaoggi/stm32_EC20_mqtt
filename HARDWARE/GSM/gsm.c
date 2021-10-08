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
//本文件为存放子函数区域
//子函数功能为标准发送AT指令函数
//------------------------------------------------
//-----目的手机地址-------------------------------
//就是你想给谁发短信，你就把谁的号码写这里，别把自己的号写这里，
//我这么说能明白了吧!
//------------------------------------------------


//就改这2个就行目前的版本不写中心号也能发短信，写不对就不写了
//---------------------------------------------------------------

//-----中心号地址---------------
//短信息中心号，和打电话没关系，写错了也能打电话。
//这个只是关系发送短信的号码，需要打10086（移动）或者10010（联通）咨询，或者百度，GOOGLE都可以
//千万别写自己的手机号了。
//如果你是上海人在广州买的卡，去北京做实验，那么就写那个卡的中心号，也就是广州的，不是北京的!!
//我觉得我说的够明白了吧。
//----------------------------------------------------------------------------------------------
uchar const TC_ZXH2[]={"+8613010395500"};        //天津联通本地中心号。
//uchar code TC_ZXH2[]={"+8613800220500"};      //天津移动本地中心号。
//-------其他地方不要乱动-----------------------------------------------------
uchar const TC_MDSJ1[]={"AT+CMGS="};             //目的手机地址，
uchar const TC_ZXH1[]={"AT+CSCA="};              //本地中心号。
//-------TEXT短信内容区-----------------------------------------------------

//-------------------延时-----------------------------------------------------
/*void delay_GSM(uint i)
{
	unsigned int i_delay,j_delay;
   	for(i_delay=0;i_delay<i;i_delay++)
     	{   
       	for(j_delay=0;j_delay<1000;j_delay++){;}
        }
} */
//-------------------配置接收短信内容------------------------------------------------
void SMS_CONFIG()
{
  	USART2_SendStr("AT+CNMI=2,1,0\r\n");//设置好后可以中断接收短信信息
    GSM_CON=SMST;					   //SMST=0X09
} 
//-------------------关闭回显----------------------------
void ATE0()
{

		USART2_Write(USART2,"ATE0\r\n",strlen("ATE0\r\n"));// 
    GSM_CON=RST;          			   //RST=0X01
}
//-------------------写手机短信内容-----------------------------
void ATNAME()
{//I AM OK
//uchar i;
    // 	delay_GSM(2000);
	USART2_SendStr(TC_FSNR);
//	USART1_SendStr(TC_FSNR);
//	USART2_Write(USART2,TC_FSNR,strlen(TC_FSNR));// 
//	xing_dim=0;

  	delay_GSM(100);

    USART2_SendOneChar(0X1A);//短信息内容发送结束标识 
//    USART2_SendOneChar(0X0D);//空格
//    USART2_SendOneChar(0X0A);//换行（空格+换行=回车）
    GSM_CON=ATDAT;         //ATDAT=0X08
}
//----------------------------写目的手机号----------------------------------------
void AT_CMGS()
{
//AT+CMGS=13132061066
	//uchar i;
	delay_GSM(2000);
	//USART2_Write(USART2,"AT+CMGS=",strlen("AT+CMGS="));// 
	USART2_SendStr("AT+CMGS=");
  	USART2_SendOneChar(0X22);//双引号
	//USART2_Write(USART2,"15620323981",strlen("15620323981"));// 
	USART2_SendStr("18698322577");
// 	for(i=0;i<=sizeof(TC_MDSJ2)-1;i++)
//    	{
//    	USART2_SendOneChar(TC_MDSJ2[i]);//发送输入的目的手机号
 //   	} 
	  	delay_GSM(2100);
  	USART2_SendOneChar(0X22);//双引号 
  	USART2_SendOneChar(0X0D);//空格
  	USART2_SendOneChar(0X0A);//换行
	//

 	GSM_CON=CMGS;			//CMGS=0X06
}
//----------------------------写信息中心号-----------------------------------------
void AT_CSCA()
{//AT+CSCA=+8613010130500
	uchar i;
	
	USART2_Write(USART2,"AT+CSCA=",strlen("AT+CSCA="));// 
    USART2_SendOneChar(0X22);//双引号 
 	for(i=0;i<sizeof(TC_ZXH2)-1;i++)
		{
 		USART2_SendOneChar(TC_ZXH2[i]);//设置短信息中心号
 		} 
 	USART2_SendOneChar(0X22);//双引号 
   	USART2_SendOneChar(0X0D);//空格
    USART2_SendOneChar(0X0A);//回车
    GSM_CON=CSCA;			//CSCA=0X07
}

//----------------------------AT网络状态查询指令-----------------------------------
void AT_CREG()
{

	USART2_Write(USART2,"AT+CREG?\r\n",strlen("AT+CREG?\r\n"));// 
  	GSM_CON=CREG;	//CREG=0X04
}
//----------------------------AT接收格式指令---------------------------------------
void AT_CMGF()
{//AT+CMGF=1
	
	USART2_Write(USART2,"AT+CMGF=1\r\n",strlen("AT+CMGF=1\r\n"));// 
  	GSM_CON=CMGF;//CMGF=0X02
}
//----------------------------AT短消息读取指令-------------------------------------
void AT_CMGR()
{//AT+CMGR=1
	
	USART2_Write(USART2,"AT+CMGR=1\r\n",strlen("AT+CMGR=1\r\n"));// 
  	GSM_CON=CMGR;//CBGR=0X05
}
//----------------------------AT短消息删除指令-------------------------------------
void AT_CMGD()
{//AT+CMGD=1
	
	USART2_Write(USART2,"AT+CMGD=1\r\n",strlen("AT+CMGD=1\r\n"));// 
  	GSM_CON=CMGD;//CMGD=OXO3
}










