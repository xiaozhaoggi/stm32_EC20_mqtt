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
                    errorLog(1);	//解析错误
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
                        break;	//获取UTC时间
                    case 2:
                        memcpy(usefullBuffer, subString, subStringNext - subString);
                        break;	//获取UTC时间
                    case 3:
                        memcpy(Save_Data.latitude, subString, subStringNext - subString);
                        break;	//获取纬度信息
                    case 4:
                        memcpy(Save_Data.N_S, subString, subStringNext - subString);
                        break;	//获取N/S
                    case 5:
                        memcpy(Save_Data.longitude, subString, subStringNext - subString);
                        break;	//获取经度信息
                    case 6:
                        memcpy(Save_Data.E_W, subString, subStringNext - subString);
                        break;	//获取E/W

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
                    errorLog(2);	//解析错误
                }
            }


        }
    }
}


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
/*#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
	int handle;

};

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式
void  _sys_exit(int x)
{
	x = x;
}

//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕
    USART3->DR = (u8) ch;
	return ch;
}
#endif
*/


/*使用microLib的方法*/
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

#if EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
char USART3_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;       //接收状态标记

void uart3_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    //声明一个结构体变量，用来初始化GPIO
    NVIC_InitTypeDef NVIC_InitStructure;
    //使能串口的RCC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //使能UART3所在GPIOB的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    //串口使用的GPIO口配置
    // Configure USART3 Rx (PB.11) as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure USART3 Tx (PB.10) as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    //配置串口
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


    // Configure USART3
    USART_Init(USART3, &USART_InitStructure);//配置串口3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
    // Enable the USART3
    USART_Cmd(USART3, ENABLE);//使能串口3



    CLR_Buf();//清空缓存
}

void USART3_IRQHandler(void)                	//串口1中断服务程序
{
    u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
    OSIntEnter();
#endif
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        Res =USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据

        if(Res == '$')
        {
            point1 = 0;
        }


        USART3_RX_BUF[point1++] = Res;

        if(USART3_RX_BUF[0] == '$' && USART3_RX_BUF[4] == 'M' && USART3_RX_BUF[5] == 'C')			//确定是否收到"GPRMC/GNRMC"这一帧数据
        {
            if(Res == '\n')
            {
                memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
                memcpy(Save_Data.GPS_Buffer, USART3_RX_BUF, point1); 	//保存数据
                Save_Data.isGetData = true;
                point1 = 0;
                memset(USART3_RX_BUF, 0, USART_REC_LEN);      //清空
            }

        }

        if(point1 >= USART_REC_LEN)
        {
            point1 = USART_REC_LEN;
        }






// 		USART3_RX_STA|=0x8000;	//接收完成了
// 		if((USART3_RX_STA&0x8000)==0)//接收未完成
// 		{
// 			if(USART3_RX_STA&0x4000)//接收到了0x0d
// 			{
// 				if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
// 				else USART3_RX_STA|=0x8000;	//接收完成了 			//bit31表明是否接收到0x0a(\n)
// 			}
// 			else //还没收到0X0D
// 			{
// 				if(Res==0x0d)USART3_RX_STA|=0x4000;						//bit30表明是否接收到0x0d(\r)
// 				else
// 				{
// 					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
// 					USART3_RX_STA++;
// 					if(USART3_RX_STA>(USART_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收
// 				}
// 			}
// 		}
    }
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
    OSIntExit();
#endif
}


u8 Hand(char *a)                   // 串口命令识别函数
{
    if(strstr(USART3_RX_BUF,a)!=NULL)
        return 1;
    else
        return 0;
}

void CLR_Buf(void)                           // 串口缓存清理
{
    memset(USART3_RX_BUF, 0, USART_REC_LEN);      //清空
    point1 = 0;
}

void clrStruct()
{
    Save_Data.isGetData = false;
    Save_Data.isParseData = false;
    Save_Data.isUsefull = false;
    memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
    memset(Save_Data.UTCTime, 0, UTCTime_Length);
    memset(Save_Data.latitude, 0, latitude_Length);
    memset(Save_Data.N_S, 0, N_S_Length);
    memset(Save_Data.longitude, 0, longitude_Length);
    memset(Save_Data.E_W, 0, E_W_Length);

}

#endif

