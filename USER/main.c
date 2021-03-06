#include "delay.h"
#include "sys.h"
#include "gsm.h"
#include "usart.h"
#include "ONENET.h"
#include "MQTT_TEST.h"
#include "ec20.h"
#include "imu901.h"
#include "beidou.h"
extern void TIM3_Int_Init(u16 arr,u16 psc);

u8 uart2_ok;

char mqtt_msg[600]= {0};

u16 json_len_watch;
u16 len_watch;//MQTT?????????
int main(void)
{

    char payload[500];

    int time = 0;

    delay_init();	    	 //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);

    TIM3_Int_Init(100-1,8400-1);	//10ms中断定义
    uart_init(115200);//115200波特率
    uart2_init(115200);//115200波特率
    uart3_init(9600);
    imu901_init();							/* IMU901模块初始 */
    EC20_Init();//EC20初始化以及各类参数定义
    // EC20Send_HttpPkt(REG_PKT_HEAD, REG_PKT_BODY,dev_id);//http格式注册

    EC20_MQTTCONNECTServer();//MQTT链接服务器
    len_watch=mqtt_connect_message(mqtt_msg,DEV_ID,PROD_ID,SN);//得到ID，产品号和SN
    sendMQTTPkt(mqtt_msg,len_watch);
    delay_ms(1000);
    delay_ms(1000);
    MqttSample_RecvPkt();//
    Clear_Buffer();
    clrStruct();
    while(1)
    {
        delay_ms(1);
        time++;
        if(time%100 == 0) //延时发送
        {
            json_len_watch=MqttOnenet_Savedata(payload, &attitude,&gyroAccData,&baroData,&Save_Data, &beidou);//json格式
            len_watch=mqtt_onenetpublish_message(mqtt_msg,"$dp",payload,json_len_watch,1);//数据发布
            sendMQTTPkt(mqtt_msg,len_watch);
        }
        parseGpsBuffer();
    }
}

