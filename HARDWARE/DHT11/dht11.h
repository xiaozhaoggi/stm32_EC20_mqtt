#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   


//IO方向设置
//#define DHT11_IO_IN()  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)//PD13输入模式
//#define DHT11_IO_OUT() PDin(13) 	//PD13输出模式 
////IO操作函数											   
#define	DHT11_DQ_OUT PDout(13) //数据端口	PD13
#define	DHT11_DQ_IN  PDin(13)  //数据端口	PD13

u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11    
#endif















