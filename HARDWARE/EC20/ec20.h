#ifndef __EC20_H
#define __EC20_H
#include "sys.h"
#include "delay.h"
#include "usart.h"
void EC20_Init(void);
void EC20Send_StrData(char *bufferdata);
void EC20Send_MultiStrData(u8 channel ,char *bufferdata);
void EC20Send_HexData(char *bufferdata);
void EC20Send_RecAccessMode(void);
void EC20Send_ChangeMode(u8 data);
void EC20Send_RecAccessMode(void);
void Clear_Buffer(void);//��ջ���

#endif
