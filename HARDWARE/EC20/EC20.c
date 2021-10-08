#include "ec20.h"
#include "stdlib.h"

#include "string.h"
#include "usart.h"
extern int Rxcouter;
extern char RxBuffer[];
char *strx,*extstrx,*Readystrx;
void Clear_Buffer(void)//��ջ���
{
    int i;
//   printf(RxBuffer);
//	USART1_Write(USART1,RxBuffer,Rxcouter);

    for(i=0; i<Rxcouter; i++)
        RxBuffer[i]=0;//����
    Rxcouter=0;
    //	IWDG_Feed();//ι��
}
void MQTTClear_Buffer(void)//��ջ���
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
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    while(strx==NULL)
    {
        Clear_Buffer();
        USART2_Write(USART2,"AT\r\n",strlen("AT\r\n"));
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    }
    //printf("ATE0\r\n"); //�رջ���
    //	delay_ms(500);
    //Clear_Buffer();
    USART2_Write(USART2,"AT+CSQ\r\n",strlen("AT+CSQ\r\n"));

    delay_ms(500);

    USART2_Write(USART2,"ATi\r\n",strlen("ATi\r\n"));
    delay_ms(500);
    /////////////////////////////////
    USART2_Write(USART2,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));

    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"+CPIN: READY");//�鿴�Ƿ񷵻�ready
    while(strx==NULL)
    {
        Clear_Buffer();
        USART2_Write(USART2,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"+CPIN: READY");//���SIM���Ƿ���λ���ȴ�����λ�������ʶ�𲻵���ʣ��Ĺ�����û������
    }
    Clear_Buffer();
    /////////////////////////////////////

    USART2_Write(USART2,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,1");//��������Ҫ��ֻ��ע��ɹ����ſ��Խ���GPRS���ݴ��䡣
    extstrx=strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,5");//��������������
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        //		printf("AT+CGREG?\r\n");//�鿴�Ƿ�ע��GPRS����
        USART2_Write(USART2,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,1");//��������Ҫ��ֻ��ע��ɹ����ſ��Խ���GPRS���ݴ��䡣
        extstrx=strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,5");//��������������
    }
    Clear_Buffer();
    //	printf("AT+COPS?\r\n");//�鿴ע�ᵽ�ĸ���Ӫ�̣�֧���ƶ� ��ͨ ����
    USART2_Write(USART2,"AT+COPS?\r\n",strlen("AT+COPS?\r\n"));
    delay_ms(500);
    Clear_Buffer();
//		printf("AT+QICLOSE=0\r\n");//�ر�socket����
    USART2_Write(USART2,"AT+QICLOSE=0\r\n",strlen("AT+QICLOSE=0\r\n"));
    delay_ms(500);
    Clear_Buffer();
    //printf("AT+QICSGP=1,1,\042CMNET\042,\042\042,\042\042,0\r\n");//����APN�����û���������
    USART2_Write(USART2,"AT+QICSGP=1,1,\042CMNET\042,\042\042,\042\042,0\r\n",strlen("AT+QICSGP=1,1,\042CMNET\042,\042\042,\042\042,0\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//�����ɹ�
    while(strx==NULL)
    {
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");////�����ɹ�
    }
    Clear_Buffer();
    //	printf("AT+QIDEACT=1\r\n");//ȥ����
    USART2_Write(USART2,"AT+QIDEACT=1\r\n",strlen("AT+QIDEACT=1\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//�����ɹ�
    while(strx==NULL)
    {
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//�����ɹ�
    }
    Clear_Buffer();
    //	printf("AT+QIACT=1\r\n");//����
    USART2_Write(USART2,"AT+QIACT=1\r\n",strlen("AT+QIACT=1\r\n"));
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//�����ɹ�
    while(strx==NULL)
    {
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//�����ɹ�
    }
    Clear_Buffer();
    //	printf("AT+QIACT?\r\n");//��ȡ��ǰ����IP��ַ
    USART2_Write(USART2,"AT+QIACT?\r\n",strlen("AT+QIACT?\r\n"));
    delay_ms(500);
    Clear_Buffer();

}