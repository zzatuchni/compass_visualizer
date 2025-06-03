#ifndef ESP8266_H
#define ESP8266_H

#include "basic.h"
#include "gpio.h"
#include "uart.h"
#include "debug.h"

#define DMA1_CH3_INTERRUPT_NUM 13

#define AT_UART_TEST_CMD "12345678\r\n"

#define AT_RST_CMD "AT+RST\r\n"

#define ATE0_CMD "ATE0\r\n"
#define ATE1_CMD "ATE1\r\n"

#define AT_RST_CMD "AT+RST\r\n"

#define AT_TEST_CMD "AT\r\n"

#define AT_CONF_GET_APS_CMD "AT+CWLAPOPT=1,127\r\n"
#define AT_GET_APS_CMD "AT+CWLAP=\"WiFi\"\r\n"

#define AT_WIFI_MODE_CMD "AT+CWMODE=1\r\n"

#define AT_STATIC_IP_CMD "AT+CIPSTA=\"192.168.100.100\",\r\n"

#define AT_CONNECT_TO_AP_CMD "AT+CWJAP_CUR=\"ZZ\",\"Crashingthisplane_1\"\r\n"

#define AT_MULTIPLE_CONNECTIONS_CMD "AT+CIPMUX=1\r\n"

#define AT_SET_SERVER_CMD "AT+CIPSERVER=1,1001\r\n"

#define AT_SEND_DATA_CMD "AT+CIPSEND=0,4\r\n"

Result esp8266_dma_init();

Result esp8266_init();

#endif