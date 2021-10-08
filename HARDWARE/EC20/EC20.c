#include "ec20.h"
#include "stdlib.h"

#include "string.h"
#include "usart.h"
extern int Rxcouter;
extern char RxBuffer[];
char *strx,*extstrx,*Readystrx;
void Clear_Buffer(void)//清空缓存
{
    int i;
//   printf(RxBuffer);
//	USART1_Write(USART1,RxBuffer,Rxcouter);

    for(i=0; i<Rxcouter; i++)
        RxBuffer[i]=0;//缓存
    Rxcouter=0;
    //	IWDG_Feed();//喂狗
}
void MQTTClear_Buffer(void)//清空缓存
{

    //   printf(RxBuffer);
//	USART1_Write(USART1,RxBuffer,Rxcouter);
//	printf("\r\n");
// printf("Rxcouter=%d\r\n",Rxcouter);

    Rxcouter=0;

}
void  EC20_Init(void)
{
    //	printf("AT\r\n");
    USART2_Write(USART2,"AT\r\n",strlen("AT\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
        Clear_Buffer();
        USART2_Write(USART2,"AT\r\n",strlen("AT\r\n"));
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    }
    //printf("ATE0\r\n"); //关闭回显
    //	delay_ms(500);
    //Clear_Buffer();
    USART2_Write(USART2,"AT+CSQ\r\n",strlen("AT+CSQ\r\n"));

    delay_ms(500);

    USART2_Write(USART2,"ATi\r\n",strlen("ATi\r\n"));
    delay_ms(500);
    /////////////////////////////////
    USART2_Write(USART2,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));

    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"+CPIN: READY");//查看是否返回ready
    while(strx==NULL)
    {
        Clear_Buffer();
        USART2_Write(USART2,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"+CPIN: READY");//检查SIM卡是否在位，等待卡在位，如果卡识别不到，剩余的工作就没法做了
    }
    Clear_Buffer();
    /////////////////////////////////////

    USART2_Write(USART2,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,1");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
    extstrx=strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,5");//返回正常，漫游
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        //		printf("AT+CGREG?\r\n");//查看是否注册GPRS网络
        USART2_Write(USART2,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,1");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
        extstrx=strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,5");//返回正常，漫游
    }
    Clear_Buffer();
    //	printf("AT+COPS?\r\n");//查看注册到哪个运营商，支持移动 联通 电信
    USART2_Write(USART2,"AT+COPS?\r\n",strlen("AT+COPS?\r\n"));
    delay_ms(500);
    Clear_Buffer();
//		printf("AT+QICLOSE=0\r\n");//关闭socket连接
    USART2_Write(USART2,"AT+QICLOSE=0\r\n",strlen("AT+QICLOSE=0\r\n"));
    delay_ms(500);
    Clear_Buffer();
    //printf("AT+QICSGP=1,1,\042CMNET\042,\042\042,\042\042,0\r\n");//接入APN，无用户名和密码
    USART2_Write(USART2,"AT+QICSGP=1,1,\042CMNET\042,\042\042,\042\042,0\r\n",strlen("AT+QICSGP=1,1,\042CMNET\042,\042\042,\042\042,0\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//开启成功
    while(strx==NULL)
    {
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");////开启成功
    }
    Clear_Buffer();
    //	printf("AT+QIDEACT=1\r\n");//去激活
    USART2_Write(USART2,"AT+QIDEACT=1\r\n",strlen("AT+QIDEACT=1\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//开启成功
    while(strx==NULL)
    {
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//开启成功
    }
    Clear_Buffer();
    //	printf("AT+QIACT=1\r\n");//激活
    USART2_Write(USART2,"AT+QIACT=1\r\n",strlen("AT+QIACT=1\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//开启成功
    while(strx==NULL)
    {
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//开启成功
    }
    Clear_Buffer();
    //	printf("AT+QIACT?\r\n");//获取当前卡的IP地址
    USART2_Write(USART2,"AT+QIACT?\r\n",strlen("AT+QIACT?\r\n"));
    delay_ms(500);
    Clear_Buffer();

}
