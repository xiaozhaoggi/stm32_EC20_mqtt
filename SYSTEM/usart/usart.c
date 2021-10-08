#include "sys.h"
#include "usart.h"	 
#include "imu901.h"
#include "string.h"
extern uchar xing_dim,GSM_RE2[];//��������
char RxBuffer[600];
int Rxcouter;
extern u8 uart2_ok;
extern u8 Timeout;
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void  _sys_exit(int x)
{ 
	x = x; 
} 
void _ttywrch(int ch)
{
ch=ch;
}
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
ringbuffer_t uart1RxFifo;
#define UART1_RX_BUFFER_SIZE	256
uint8_t uart1RxBuffer[UART1_RX_BUFFER_SIZE]; 

void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
	
  ringbuffer_init(&uart1RxFifo, uart1RxBuffer, UART1_RX_BUFFER_SIZE);
}
  
void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 

}

//--------����һ���ַ�------------------
int USART1_SendOneChar(u8 ch)  {
  
  while (!(USART1->SR & USART_FLAG_TXE));
  USART1->DR = ch;
  USART1->SR &= ~USART_FLAG_TXE;
  return (ch);
}

//------ �����ַ���--------------
void USART1_SendStr(u8 *str)   
{   
    while( (*str)!='\0' )   
    	{   
        USART1_SendOneChar(*str);   
        str++;   
    	}   
}   


int USART2_SendOneChar(u8 ch)  {
  
  while (!(USART2->SR & USART_FLAG_TXE));
  USART2->DR = ch;
  USART2->SR &= ~USART_FLAG_TXE;
  return (ch);
}

void USART2_SendStr(u8 *str)   
{   
    while( (*str)!='\0' )   
    	{   
        USART2_SendOneChar(*str);   
        str++;   
    	}   
}   




void USART1_IRQHandler(void)                	//����1�жϷ������
	{
		u8 Res;
    u8 ch;
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
		{
			Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
			USART_ClearITPendingBit(USART1,USART_IT_RXNE);
			ringbuffer_in_check(&uart1RxFifo, (uint8_t *)&Res, 1); /*!< �����յ������ݷ���FIFO */
			
		if (imu901_uart_receive(&ch, 1)) 	/*!< ��ȡ����fifoһ���ֽ� */
		{
				if (imu901_unpack(ch)) 			/*!< ��������Ч���ݰ� */
				{
						if (rxPacket.startByte2 == UP_BYTE2) 			/*!< �����ϴ������ݰ� */
						{
								atkpParsing(&rxPacket);
						}
				}
		}			
     } 

} 
	void USART1_Write(USART_TypeDef* USARTx, uint8_t *Data, uint8_t len)
{
    uint8_t i;
    USART_ClearFlag(USARTx, USART_FLAG_TC);
    for(i = 0; i < len; i++)
    {
        USART_SendData(USARTx, *Data++);
        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
    }
}

uint16_t usart1_getRxData(uint8_t *buf, uint16_t len)
{
    return ringbuffer_out(&uart1RxFifo, buf, len);
}

void USART2_Write(USART_TypeDef* USARTx, char *Data, uint8_t len)
{
    uint8_t i;
    USART_ClearFlag(USARTx, USART_FLAG_TC);
    for(i = 0; i < len; i++)
    {
        USART_SendData(USARTx, *Data++);
        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
    }
}
void  USART2_GetRcvData(char *buf, uint32_t rcv_len)
{
	//u8 i;
	u8 len;
	char *p;
	len=Rxcouter;
	p=RxBuffer;
    if(buf)
    {
        memcpy(buf, RxBuffer, Rxcouter);//????????????
    }
  }
void USART2_IRQHandler(void)                	//??1??????
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //????(?????????0x0d 0x0a??)
	{
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//????????
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        RxBuffer[Rxcouter++]=Res;//
		if(Rxcouter>=500)
			Rxcouter=0;
        TIM_Cmd(TIM3,ENABLE);
        Timeout=0;
        uart2_ok=0;
    } 

} 


#endif	

