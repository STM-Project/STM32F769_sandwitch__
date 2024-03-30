/*
 * wifi.h
 *
 *  Created on: 11.12.2020
 *      Author: RafalMar
 */

#ifndef WIFI_ESP32WROOM_H_
#define WIFI_ESP32WROOM_H_

#include "stm32f7xx_hal.h"

#define ESP_UART_TYPE		USART6
#define ESP_UART_HANDLE		huart6
#define ESP_UART_DMA_RX		hdma_usart6_rx
#define ESP_UART_BUADRATE	1000000

#define ESP_EMAIL_CHANNEL	4

#define ESP_EN_GPIO_TYPE 	GPIOA
#define ESP_EN_GPIO_PIN 	GPIO_PIN_9

#define WIFI_AP_MAX	3
#define WIFI_STA_MAX	3

typedef struct __attribute__ ((packed))
{
	uint64_t mac;
	uint32_t ip;
	uint32_t mask;
	uint32_t gate;
	uint16_t port;
	char STA_ssid[64];
 	char STA_pass[64];
 	char AP_ssid[64];
 	char AP_pass[64];
	uint8_t dhcp;
	uint8_t sta;
	uint8_t ap;

}s_net_wifi;

typedef struct __attribute__ ((packed))
{
	uint64_t mac;
	uint32_t ip;
	uint32_t mask;
	uint32_t gate;
	uint16_t port;
	uint8_t dhcp;
 	char name[64];
 	char pass[64];

}s_wifi_AP;

typedef struct __attribute__ ((packed))
{
	uint64_t mac;
	uint32_t ip;
	uint32_t mask;
	uint32_t gate;
	uint16_t port;
	uint8_t dhcp;
	char name[64];
 	char pass[64];

}s_wifi_STA;

typedef struct __attribute__ ((packed))
{
	 int nrAP;
	 int nrSTA;
	 uint8_t mode;

}s_wifi_select;

void CreateWifiTask(void);
void CloseWifiTask(void);
void RestartWifiTask(void);

void WIFI_RxCallbackService(void);
void ESP_FlagIdleLineSet(void);
void WIFI_UartErrorService(void);
int GetEmailStatus(void);
void vRestartWifiServer(void);
void vTestATcommand(void);

#endif /* WIFI_ESP32WROOM_H_ */
