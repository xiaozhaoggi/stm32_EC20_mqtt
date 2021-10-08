#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h" 	
extern u8 Timeout;
extern u8 uart2_ok;
void TIM3_Int_Init(u16 arr,u16 psc);
#endif
