#ifndef __MQTT_DEMO_H
#define __MQTT_DEMO_H	 
#include "sys.h"
#include "stdlib.h"
#include "string.h"
u16 mqtt_connect_message(char *mqtt_message,char *client_id,char *username,char *password);
u16 mqtt_subscribe_message(u8 *mqtt_message,char *topic,u8 qos,u8 whether);
u8 mqtt_ping_message(u8 *mqtt_message);
u8 mqtt_disconnect_message(u8 *mqtt_message);
u8 mqtt_disconnect_message(u8 *mqtt_message);
u16 mqtt_publish_message(u8 *mqtt_message, char * topic, char * message, u8 qos);
u16 mqtt_onenetpublish_message(char *mqtt_message, char * topic, char * message,u16 len, u8 qos);
#endif

