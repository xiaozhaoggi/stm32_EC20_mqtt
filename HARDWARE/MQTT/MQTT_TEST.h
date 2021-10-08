#ifndef __MQTT_FUN_H
#define __MQTT_FUN_H
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "imu901.h"
#include "beidou.h"
#define PROD_ID     "403501"             //产品ID
#define SN          "20210126"      //标记名称也是鉴权
#define REG_CODE    "bjiSo9w3b8VF61Ja"  //注册码
#define API_ADDR    "api.heclouds.com"//登录的API
#define DEV_ID			"684732077"
#define DEVICE_NAME     "WDCS_"SN//设备名称
//WDCS_20121227
#define REG_PKT_HEAD    "POST http://"API_ADDR"/register_de?register_code="REG_CODE" HTTP/1.1\r\n"\
                        "Host: "API_ADDR"\r\n"\
                        "Content-Length: "//http post数据头
                    
#define REG_PKT_BODY    "{\"title\":\""DEVICE_NAME"\",\"sn\":\""SN"\"}"//数据内容

void EC20Send_HttpPkt(char *phead, char *pbody,char*dev_id);
void EC20_MQTTCONNECTServer(void);
void sendMQTTPkt(char *p, int len);
int MqttSample_RecvPkt(void);
char *uartDataParse(char *buffer, int32_t *plen);
u8 MqttOnenet_Savedata(char *t_payload, attitude_t *attitude,gyroAcc_t *gyroAccData,baro_t *baroData,_SaveData *Save_Data,	_BeiDou *beidou);
void MQTTOnent_Recdata(void);
struct MqttContext 
{
 u8 device_id[20];
};

#endif
