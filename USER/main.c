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

    delay_init();	    	 //��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);

    TIM3_Int_Init(100-1,8400-1);	//10ms�ж϶���
    uart_init(115200);//115200������
    uart2_init(115200);//115200������
    uart3_init(9600);
    imu901_init();							/* IMU901ģ���ʼ */
    EC20_Init();//EC20��ʼ���Լ������������
    // EC20Send_HttpPkt(REG_PKT_HEAD, REG_PKT_BODY,dev_id);//http��ʽע��

    EC20_MQTTCONNECTServer();//MQTT���ӷ�����
    len_watch=mqtt_connect_message(mqtt_msg,DEV_ID,PROD_ID,SN);//�õ�ID����Ʒ�ź�SN
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
        if(time%100 == 0) //��ʱ����
        {
            json_len_watch=MqttOnenet_Savedata(payload, &attitude,&gyroAccData,&baroData,&Save_Data, &beidou);//json��ʽ
            len_watch=mqtt_onenetpublish_message(mqtt_msg,"$dp",payload,json_len_watch,1);//���ݷ���
            sendMQTTPkt(mqtt_msg,len_watch);
        }
        parseGpsBuffer();
    }
}
