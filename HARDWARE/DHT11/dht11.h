#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   


//IO��������
//#define DHT11_IO_IN()  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)//PD13����ģʽ
//#define DHT11_IO_OUT() PDin(13) 	//PD13���ģʽ 
////IO��������											   
#define	DHT11_DQ_OUT PDout(13) //���ݶ˿�	PD13
#define	DHT11_DQ_IN  PDin(13)  //���ݶ˿�	PD13

u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    
#endif















