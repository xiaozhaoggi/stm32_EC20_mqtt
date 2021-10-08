#include "sys.h"
#include "beidou.h"
#include "stdio.h"
#include "stdlib.h"
char rxdatabufer;
u16 point1 = 0;

_SaveData Save_Data;
_BeiDou beidou;

void errorLog(int num)
{

    while (1)
    {
        printf("ERROR%d\r\n",num);
    }
}

void parseGpsBuffer()
{
    char *subString;
    char *subStringNext;
    char i = 0;
    if (Save_Data.isGetData)
    {
        Save_Data.isGetData = false;
        printf("**************\r\n");
        printf(Save_Data.GPS_Buffer);


        for (i = 0 ; i <= 6 ; i++)
        {
            if (i == 0)
            {
                if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
                    errorLog(1);	//��������
            }
            else
            {
                subString++;
                if ((subStringNext = strstr(subString, ",")) != NULL)
                {
                    char usefullBuffer[2];
                    switch(i)
                    {
                    case 1:
                        memcpy(Save_Data.UTCTime, subString, subStringNext - subString);
                        break;	//��ȡUTCʱ��
                    case 2:
                        memcpy(usefullBuffer, subString, subStringNext - subString);
                        break;	//��ȡUTCʱ��
                    case 3:
                        memcpy(Save_Data.latitude, subString, subStringNext - subString);
                        break;	//��ȡγ����Ϣ
                    case 4:
                        memcpy(Save_Data.N_S, subString, subStringNext - subString);
                        break;	//��ȡN/S
                    case 5:
                        memcpy(Save_Data.longitude, subString, subStringNext - subString);
                        break;	//��ȡ������Ϣ
                    case 6:
                        memcpy(Save_Data.E_W, subString, subStringNext - subString);
                        break;	//��ȡE/W

                    default:
                        break;
                    }
                    char jingdu_l[6],jingdu_r[7];
                    int a,c,i;
                    float b,d;
                    for(i=0; i<5; i++)
                    {
                        jingdu_l[i]=Save_Data.longitude[i];

                    }
                    for(i=6; i<13; i++)
                    {
                        jingdu_r[i-6]=Save_Data.longitude[i];
                    }
                    a=atoi(jingdu_l);
                    b=atoi(jingdu_r);
                    c=a/100;
                    d=(a-c*100+b/10000)/60;
                    beidou.jingdu=c+d;

                    char weidu_l[6],weidu_r[7];
                    int a1,c1;
                    float b1,d1;
                    for(i=0; i<5; i++)
                    {
                        weidu_l[i]=Save_Data.latitude[i];

                    }
                    for(i=6; i<13; i++)
                    {
                        weidu_r[i-6]=Save_Data.latitude[i];
                    }
                    a1=atoi(weidu_l);
                    b1=atoi(weidu_r);
                    c1=a1/100;
                    d1=(a1-c1*100+b1/10000)/60;
                    beidou.weidu=c1+d1;

                    subString = subStringNext;
                    Save_Data.isParseData = true;
                    if(usefullBuffer[0] == 'A')
                        Save_Data.isUsefull = true;
                    else if(usefullBuffer[0] == 'V')
                        Save_Data.isUsefull = false;

                }
                else
                {
                    errorLog(2);	//��������
                }
            }


        }
    }
}


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
/*#if 1
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

//�ض���fputc����
int fputc(int ch, FILE *f)
{
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������
    USART3->DR = (u8) ch;
	return ch;
}
#endif
*/


/*ʹ��microLib�ķ���*/
/*
int fputc(int ch, FILE *f)
{
USART_SendData(USART3, (uint8_t) ch);

while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) {}

   return ch;
}
int GetKey (void)  {

   while (!(USART3->SR & USART_FLAG_RXNE));

   return ((int)(USART3->DR & 0x1FF));
}
*/

#if EN_USART3_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
char USART3_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;       //����״̬���

void uart3_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    //����һ���ṹ�������������ʼ��GPIO
    NVIC_InitTypeDef NVIC_InitStructure;
    //ʹ�ܴ��ڵ�RCCʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //ʹ��UART3����GPIOB��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    //����ʹ�õ�GPIO������
    // Configure USART3 Rx (PB.11) as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure USART3 Tx (PB.10) as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //Usart3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
    //���ô���
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


    // Configure USART3
    USART_Init(USART3, &USART_InitStructure);//���ô���3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    // Enable the USART3
    USART_Cmd(USART3, ENABLE);//ʹ�ܴ���3



    CLR_Buf();//��ջ���
}

void USART3_IRQHandler(void)                	//����1�жϷ������
{
    u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
    OSIntEnter();
#endif
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        Res =USART_ReceiveData(USART3);//(USART3->DR);	//��ȡ���յ�������

        if(Res == '$')
        {
            point1 = 0;
        }


        USART3_RX_BUF[point1++] = Res;

        if(USART3_RX_BUF[0] == '$' && USART3_RX_BUF[4] == 'M' && USART3_RX_BUF[5] == 'C')			//ȷ���Ƿ��յ�"GPRMC/GNRMC"��һ֡����
        {
            if(Res == '\n')
            {
                memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //���
                memcpy(Save_Data.GPS_Buffer, USART3_RX_BUF, point1); 	//��������
                Save_Data.isGetData = true;
                point1 = 0;
                memset(USART3_RX_BUF, 0, USART_REC_LEN);      //���
            }

        }

        if(point1 >= USART_REC_LEN)
        {
            point1 = USART_REC_LEN;
        }






// 		USART3_RX_STA|=0x8000;	//���������
// 		if((USART3_RX_STA&0x8000)==0)//����δ���
// 		{
// 			if(USART3_RX_STA&0x4000)//���յ���0x0d
// 			{
// 				if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
// 				else USART3_RX_STA|=0x8000;	//��������� 			//bit31�����Ƿ���յ�0x0a(\n)
// 			}
// 			else //��û�յ�0X0D
// 			{
// 				if(Res==0x0d)USART3_RX_STA|=0x4000;						//bit30�����Ƿ���յ�0x0d(\r)
// 				else
// 				{
// 					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
// 					USART3_RX_STA++;
// 					if(USART3_RX_STA>(USART_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����
// 				}
// 			}
// 		}
    }
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
    OSIntExit();
#endif
}


u8 Hand(char *a)                   // ��������ʶ����
{
    if(strstr(USART3_RX_BUF,a)!=NULL)
        return 1;
    else
        return 0;
}

void CLR_Buf(void)                           // ���ڻ�������
{
    memset(USART3_RX_BUF, 0, USART_REC_LEN);      //���
    point1 = 0;
}

void clrStruct()
{
    Save_Data.isGetData = false;
    Save_Data.isParseData = false;
    Save_Data.isUsefull = false;
    memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //���
    memset(Save_Data.UTCTime, 0, UTCTime_Length);
    memset(Save_Data.latitude, 0, latitude_Length);
    memset(Save_Data.N_S, 0, N_S_Length);
    memset(Save_Data.longitude, 0, longitude_Length);
    memset(Save_Data.E_W, 0, E_W_Length);

}

#endif
