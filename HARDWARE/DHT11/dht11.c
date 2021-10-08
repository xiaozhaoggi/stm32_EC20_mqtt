#include "dht11.h"
#include "delay.h" 
//??DHT11
void DHT11_Rst(void)	   
{                 
	//DHT11_IO_OUT(); 	//SET OUTPUT
  DHT11_DQ_OUT=0; 	//??DQ
  delay_ms(20);    	//????18ms
  DHT11_DQ_OUT=1; 	//DQ=1 
	delay_us(30);     	//????20~40us
}
//??DHT11???
//??1:????DHT11???
//??0:??
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	//DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN&&retry<100)//DHT11???40~80us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11????????40~80us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}
//?DHT11?????
//???:1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)//???????
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//??????
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//??40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}
//?DHT11??????
//???:?????
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}
//?DHT11??????
//temp:???(??:0~50°)
//humi:???(??:20%~90%)
//???:0,??;1,????
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//??40???
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}else return 1;
	return 0;	    
}
//???DHT11?IO? DQ ????DHT11???
//??1:???
//??0:??    	 
u8 DHT11_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
  
   	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
   	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //LED0-->PB.5 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOB.5
 GPIO_SetBits(GPIOD,GPIO_Pin_13);						 //PB.5 Êä³ö¸ß
    DHT11_Rst();
    return DHT11_Check();
}
