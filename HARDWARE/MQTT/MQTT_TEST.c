#include "MQTT_TEST.h"
#include "ec20.h"
#include "string.h"
#include "stdlib.h"
#include "usart.h"
extern 	char RxBuffer[600];
extern 	int Rxcouter;
///发送字符型数据
void EC20Send_HttpPkt(char *phead, char *pbody,char*dev_id)
{
    //char sendBuf0[20];
    char sendBuf1[500];
    char *strx;
    char device_id[20] = {0};//设备的ID
    USART2_Write(USART2,"AT+QIOPEN=1,0,\042TCP\042,\042183.230.40.33\042,80,0,1\r\n",strlen("AT+QIOPEN=1,0,\042TCP\042,\042183.230.40.33\042,80,0,1\r\n"));//HTTP模式创建
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"+QIOPEN: 0,0");//检查是否登陆成功
    while(strx==NULL)
    {
        strx=strstr((const char*)RxBuffer,(const char*)"+QIOPEN: 0,0");//检查是否登陆成功
        delay_ms(100);
    }
    delay_ms(500);
    Clear_Buffer();//服务器登录成功
    sprintf(sendBuf1, "%s%d\r\n\r\n%s", phead, strlen(pbody), pbody);
//   printf("send HTTP pkt:\r\n%s\r\n", sendBuf1);//将要发送的数据
    USART2_Write(USART2,"AT+QISEND=0\r\n",strlen("AT+QISEND=0\r\n"));//发送数据
    delay_ms(100);
    USART2_Write(USART2,sendBuf1,strlen(sendBuf1));//
    delay_ms(100);
    USART_SendData(USART2, (u8) 0x1a);//发送完成函数
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {
    }
    Clear_Buffer();
    delay_ms(100);
    delay_ms(1000);
    delay_ms(1000);//等待ONENET返回数据
    /* 分析id */
    if(Rxcouter > 0)//读取到数据了
    {
        char *p = NULL, *pend = NULL;
//        printf("rcv response:\r\n%s\r\n", RxBuffer);

        /* 获取设备ID */
        if((p=strstr(RxBuffer, "device_id")) != NULL)
        {
            p += strlen("device_id\":\"");
            if((pend=strstr(p, "\",")) != NULL)
            {
                memcpy(device_id, p, pend-p);
                memcpy(dev_id, p, pend-p);//获取ID号
//                printf("get device id: %s\r\n", device_id);
                //      ctx->devid = device_id;
                //	MqttContex.device_id=12;
            }
        }
    }

    else if(Rxcouter == 0)
    {
//        printf("device regist over time!\r\n");
        while(1);
    }
    else if(device_id[0] == 0)
    {
//        printf("device regist fail!\r\n");
        while(1);
    }
    Clear_Buffer();
}

void EC20_MQTTCONNECTServer(void)//连接MQTT服务器
{
    char *strx;
    USART2_Write(USART2,"AT+QICLOSE=0\r\n",strlen("AT+QICLOSE=0\r\n"));
    delay_ms(1000);
    delay_ms(1000);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//检查是否关闭成功
    while(strx==NULL)
    {
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//检查是否关闭成功
        delay_ms(100);
    }
    Clear_Buffer();
    delay_ms(1000);
    USART2_Write(USART2,"AT+QIOPEN=1,0,\042TCP\042,\042183.230.40.39\042,6002,0,1\r\n",strlen("AT+QIOPEN=1,0,\042TCP\042,\042183.230.40.39\042,6002,0,2\r\n"));//TCP模式创建.采用MQTT方式去连接ONENET
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"+QIOPEN: 0,0");//检查是否登陆成功
    while(strx==NULL)
    {
        strx=strstr((const char*)RxBuffer,(const char*)"+QIOPEN: 0,0");//检查是否登陆成功
        delay_ms(100);
    }

    delay_ms(500);
    Clear_Buffer();
}
//发送MQTT数据包
void sendMQTTPkt(char *p, int len)
{
    char sendBuf[30] = {0};
    char *strx;
    //char untildata;
    Clear_Buffer();
    // sprintf(sendBuf0, "AT+CIPSEND=%d\r\n", strlen(sendBuf1));
    sprintf(sendBuf, "AT+QISEND=0,%d\r\n", len);
    USART2_Write(USART2,sendBuf,strlen(sendBuf));//固定长度发数据
    delay_ms(100);
    strx=strstr((char*)RxBuffer,(char*)">");//返回數據可發送
    while(strx==NULL)
        strx=strstr((char*)RxBuffer,(char*)">");
// Clear_Buffer();
    USART2_Write(USART2,p,len);
    delay_ms(100);


}
//对MQTT接收到的数据进行解析
int MqttSample_RecvPkt(void)
{
    char buffer[200];
    char *p = NULL;
    //int rcv_len = 0;
    int onenetdata_len = 0,i;
    if(Rxcouter> 0) //接收到数据了
    {
        USART2_GetRcvData(buffer, Rxcouter);

        /* 对接收到的数据进行解析 */
        if((p = uartDataParse(buffer, &onenetdata_len)) == NULL)//如果没有收到任何数据，表明失败
        {
//            printf("No server Data\r\n");
            /* 返回0 */
            return 0;
        }
        memcpy(buffer, p, onenetdata_len);
        if(onenetdata_len > 0)
        {
//            printf("Rcv: \r\n");
            for(i=0; i<onenetdata_len; i++)
            {
//                printf("%02X ", ((unsigned char *)buffer)[i]);
            }
//            printf("\r\n");
        }
        if(onenetdata_len==4)
        {
            if(buffer[0]==0x20&&buffer[1]==0x02)//返回CONNECT ACK连接状态
            {
//             printf("Connect ONENET OK!\r\n");
                //				LEDStatus=0;//灯点亮
            }
            if(buffer[0]==0x40&&buffer[1]==0x02)//返回PUB ACK确认状态
            {
//             printf("REC OK,PUB NO ACK !\r\n");
                //				LEDStatus=!LEDStatus;//灯闪烁
            }
        }
    }
    return onenetdata_len;
}
char *uartDataParse(char *buffer, int32_t *plen)
{
    char *p;
    char *pnum;
    int32_t len;
    if((p = strstr(buffer, "CLOSED")) != NULL)//表明MQTT连接失败
    {
//        printf("tcp connection closed\r\n");
    }
    if((p = strstr(buffer, "recv")) != NULL)//有数据产生
    {
        p = strstr(p, ","); //判断第一个逗号的位置
        pnum = p + 3;       //获取数据长度
        p = strstr(p+1, "\n"); //获取第二个逗号的位置
        *(p++) = '\0';      //添加结束符
        len = atoi(pnum);
//        printf("rcv %d data from OneNET:\r\n", len);
        *plen = len;
        if(strstr(buffer, "mzh_002") != NULL)//识别到数据主题
        {
//				   printf(buffer);
        }
        return p;
    }

    return NULL;
}

/*char json1[]="{\"datastreams\":[{\"id\":\"roll\",\"datapoints\":[{\"value\":%-6.1f}]},{\"id\":\"pitch\",\"datapoints\":[{\"value\":%-6.1f}]}]}";
char json2[]="{\"datastreams\":[{\"id\":\"yaw\",\"datapoints\":[{\"value\":%-6.1f}]},{\"id\":\"fgyroD_X\",\"datapoints\":[{\"value\":%-6.1f}]}]}";
char json3[]="{\"datastreams\":[{\"id\":\"fgyroD_Y\",\"datapoints\":[{\"value\":%-6.1f}]},{\"id\":\"fgyroD_Z\",\"datapoints\":[{\"value\":%-6.1f}]}]}";
char json4[]="{\"datastreams\":[{\"id\":\"pressure\",\"datapoints\":[{\"value\":%-6d}]},{\"id\":\"longitude\",\"datapoints\":[{\"value\":%-6.1f}]}]}";*/

//char json[400]="{\"datastreams\":[,,,]}";
//访问ONENET需要提交JSON数据
u16 payload_len;
u8 MqttOnenet_Savedata(char *t_payload, attitude_t *attitude,gyroAcc_t *gyroAccData,baro_t *baroData,_SaveData *Save_Data,	_BeiDou *beidou)
{
    //Save_Data=(_SaveData *)malloc(sizeof(char));
    static u8 group = 1;
    //int err;
    //uint16_t pkt_id = 1;
    char t_json[400];

    u16 json_len;

    if(group == 1)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"roll\",\"datapoints\":[{\"value\":%-6.1f}]}]}]}", attitude->roll);
    if(group == 2)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"pitch\",\"datapoints\":[{\"value\":%-6.1f}]}]}", attitude->pitch);
    if(group == 3)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"yaw\",\"datapoints\":[{\"value\":%-6.1f}]}]}", attitude->yaw );
    if(group == 4)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"fgyroD_X\",\"datapoints\":[{\"value\":%-6.1f}]}]}", gyroAccData->fgyroD[0]);
    if(group == 5)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"fgyroD_Y\",\"datapoints\":[{\"value\":%-6.1f}]}]}", gyroAccData->fgyroD[1]);
    if(group == 6)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"fgyroD_Z\",\"datapoints\":[{\"value\":%-6.1f}]}]}", gyroAccData->fgyroD[2]);
    if(group == 7)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"pressure\",\"datapoints\":[{\"value\":%-6.1d}]}]}", baroData->pressure );
    if(group == 8)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"temperature\",\"datapoints\":[{\"value\":%-6.1f}]}]}", baroData->temp );
    if(group == 9)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"E_W\",\"datapoints\":[{\"value\":%s}]}]}", Save_Data->E_W);
    if(group == 10)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"longitude\",\"datapoints\":[{\"value\":%-10.6f}]}]}", beidou->jingdu);
    if(group == 11)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"N_S\",\"datapoints\":[{\"value\":%s}]}]}", Save_Data->N_S);
    if(group == 12)
        sprintf(t_json, "{\"datastreams\":[{\"id\":\"latitude\",\"datapoints\":[{\"value\":%-10.6f}]}]}", beidou->weidu);

    group++;
    if(group>12)
        group = 1;

    payload_len = 1 + 2 + strlen(t_json)/sizeof(char);
    json_len = strlen(t_json)/sizeof(char);
    //type
    t_payload[0] = '\x01';
    //length
    t_payload[1] = (json_len & 0xFF00) >> 8;
    t_payload[2] = json_len & 0xFF;
    //json
    memcpy(t_payload+3, t_json, json_len);
    //free(Save_Data);
    return json_len+3;

}


void MQTTOnent_Recdata(void)
{
    u8 i,j=0;
    const char cmpstr[]="mzh_test";//比较函数
    for(i=0; i<Rxcouter; i++)
        if(RxBuffer[i]==cmpstr[j])
        {
            j++;
            if(j==strlen(cmpstr))//如果长度一致
            {
//		  printf("REC ONENET OK!\r\n");
                break;
            }
        }
}

