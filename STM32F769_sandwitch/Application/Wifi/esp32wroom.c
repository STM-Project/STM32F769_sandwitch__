/*
 * wifi.c
 *
 *  Created on: 11.12.2020
 *      Author: RafalMar
 */
#include <HTTP/wwwPages.h>
#include <string.h> /* memset */
#include <stdlib.h> /* atoi */
#include "esp32wroom.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "FreeRTOS.h"
#include "task.h"
#include "dbg.h"
#include "mini_printf.h"
#include "sdram.h"
#include "main.h"
#include "SDCard.h"
#include "smtp.h"
#include "variables.h"
#include "float.h"
#include "def.h"
#include "sntp_dns.h"

#define ESP_RECV_BUFF_SIZE		10000
#define PACKET_SEND_LEN 		2048

#define HTTP_ANSWER_DELAY_MS		200
#define SMTP_CONNECTION_DELAY_MS		15000
#define SMTP_ANSWER_DELAY_MS		10000
#define CONNECTION_TIMEOUT_MS		30000
#define TCP_SERVER_TIMEOUT_S		10
#define SNTP_SERVER_TIMEOUT_MS		15000
#define DNS_SERVER_TIMEOUT_MS		15000

#define DBG		1

#define ESP_ON 		HAL_GPIO_WritePin(ESP_EN_GPIO_TYPE, ESP_EN_GPIO_PIN, GPIO_PIN_SET)
#define ESP_OFF		HAL_GPIO_WritePin(ESP_EN_GPIO_TYPE, ESP_EN_GPIO_PIN, GPIO_PIN_RESET)

#define RecvFromEsp(txt)   strstr(RecvBuffer,txt)

typedef enum
{
	INIT_CONNECTION, HTTP_CONNECTION, SMTP_CONNECTION
} CONNECTION_TYPE;

typedef enum
{
	ESP_CONNECTION_OK,
	ESP_CONNECTION_TIMEOUT,
	ESP_WRONG_PASSWORD,
	ESP_CANNOT_FIND_THE_TARGET_AP,
	ESP_CONNECTION_FAILED,
	ESP_UNKNOW_ERROR_OCCURRED
}CODES_FOR_ESP_CONNECTION;

typedef enum
{
	WIFI_MODE_DISABLED,
	WIFI_MODE_STA,
	WIFI_MODE_AP,
	WIFI_MODE_AP_STA
}WIFI_MODE;

typedef enum
{
	ESPANSWER_OK=0,
	ESPANSWER_HAL_ERROR_SEND_COMM=1,
	ESPANSWER_HAL_ERROR_SEND_DATA=3,
	ESPANSWER_TIMEOUT_TO_GET_SIGN=2,
	ESPANSWER_TIMEOUT_TO_GET_SEND_OK=4,
	ESPANSWER_ERROR_AFTER_SEND_COMM=6,
	ESPANSWER_ERROR_AFTER_SEND_DATA=8,
	ESPANSWER_CLOSED_AFTER_SEND_COMM=5,
	ESPANSWER_CLOSED_AFTER_SEND_DATA=7,
	ESPANSWER_TIMEOUT_FOR_SMTPCOMM_TO_GET_SEND_OK=104,
	ESPANSWER_TIMEOUT_FOR_SMTPCOMM_TO_GET_SIGN=102

}ESP_ANSWER;

static uint8_t connectionType;

extern UART_HandleTypeDef ESP_UART_HANDLE;
extern DMA_HandleTypeDef ESP_UART_DMA_RX;
static xTaskHandle vtaskWifiHandle;
static int resetDMA=0;

static SRAM char RecvBuffer[ESP_RECV_BUFF_SIZE];
SRAM static char sendBuff[PACKET_SEND_LEN];

extern Email_Send_Param EmailSendParam;

void DefaultSettingsWIFI(void)
{
	int i;
	for (i=0; i<WIFI_AP_MAX; ++i)
	{
		SetVal64(Const_wifiAP_mac, i, 0x1122334455);
		SetTabVal(Const_wifiAP_ip, i, LWIP_MAKEU32(192,168,7,1));
		SetTabVal(Const_wifiAP_mask, i, LWIP_MAKEU32(255,255,255,0));
		SetTabVal(Const_wifiAP_gate, i, LWIP_MAKEU32(192,168,7,1));
		SetTabVal(Const_wifiAP_port, i, 80);  //WYprobowac port 8080 !!!!!
		SetTabVal(Const_wifiAP_dhcp, i, 0);
		SetStr(Const_wifiAP_name, i, "NazwaAPPPPX");
		SetStr(Const_wifiAP_pass, i, "markielowski123");
	}

	for (i=0; i<WIFI_STA_MAX; ++i)
	{
		SetVal64(Const_wifiSTA_mac, i, 0x1122334455);
		SetTabVal(Const_wifiSTA_ip, i, LWIP_MAKEU32(192,168,1,71));
		SetTabVal(Const_wifiSTA_mask, i, LWIP_MAKEU32(255,255,255,0));
		SetTabVal(Const_wifiSTA_gate, i, LWIP_MAKEU32(192,168,1,1));
		SetTabVal(Const_wifiSTA_port, i, 80);
		SetTabVal(Const_wifiSTA_dhcp, i, 0);
		SetStr(Const_wifiSTA_name, i, "NETGEAR");
		SetStr(Const_wifiSTA_pass, i, "123456789abc");
	}
	SetTabVal(Const_wifiGeneral_nrAP,NO_TAB,0);
	SetTabVal(Const_wifiGeneral_nrSTA,NO_TAB,0);
	SetTabVal(Const_wifiGeneral_mode,NO_TAB,WIFI_MODE_AP_STA);
}

static int GetAnswerDelay(void)
{
	switch (connectionType)
	{
	case SMTP_CONNECTION:
		switch (EmailSendParam.start)
		{
		case 3:
			return SMTP_CONNECTION_DELAY_MS;
		case 4:
		default:
			return SMTP_ANSWER_DELAY_MS;
		}
	case HTTP_CONNECTION:
	default:
		return HTTP_ANSWER_DELAY_MS;
	}
}

static void ChangeUartBuadRate(int baudRate)
{
	HAL_UART_DMAStop(&ESP_UART_HANDLE);

	HAL_UART_MspDeInit(&ESP_UART_HANDLE);
	HAL_UART_MspInit(&ESP_UART_HANDLE);

	ESP_UART_HANDLE.Instance= ESP_UART_TYPE;
	ESP_UART_HANDLE.Init.BaudRate=baudRate;
	if (HAL_UART_Init(&ESP_UART_HANDLE)!=HAL_OK)
	{
		Error_Handler();
	}
}

static void StartDMA(void)
{
	memset(RecvBuffer, 0, ESP_RECV_BUFF_SIZE);
	HAL_UART_Receive_DMA(&ESP_UART_HANDLE, (uint8_t*) RecvBuffer, ESP_RECV_BUFF_SIZE);
}

static void RestartDMA(void)
{
	HAL_UART_DMAStop(&ESP_UART_HANDLE);
	memset(RecvBuffer, 0, ESP_RECV_BUFF_SIZE);
	HAL_UART_Receive_DMA(&ESP_UART_HANDLE, (uint8_t*) RecvBuffer, ESP_RECV_BUFF_SIZE);
}

static int SendToEsp(char *txt)
{
	RestartDMA();
	int result= HAL_UART_Transmit(&ESP_UART_HANDLE, (uint8_t*) txt, mini_strlen(txt), 1000);
	__HAL_UART_CLEAR_FEFLAG(&ESP_UART_HANDLE);
	__HAL_UART_CLEAR_PEFLAG(&ESP_UART_HANDLE);
	__HAL_UART_CLEAR_OREFLAG(&ESP_UART_HANDLE);
	__HAL_UART_FLUSH_DRREGISTER(&ESP_UART_HANDLE);
	return result;
}

static int SendToEsp_DMA(char *pData, int lenData)
{
	RestartDMA();
	return HAL_UART_Transmit_DMA(&ESP_UART_HANDLE, (uint8_t*) pData, lenData);
}

static bool isAnythingRecv(void)
{
	if (RecvBuffer[0]>0)
		return true;
	else
		return false;
}

static void DisplayRequestGET(char *pBuf, int bytesDisp)
{
	int offs=10;
	if(bytesDisp+offs<sizeof(sendBuff))
	{
		StrBuffCopylimit(sendBuff, pBuf-offs, bytesDisp+offs);
		Dbg(DBG, "\r\n");
		Dbg(DBG, sendBuff);
	}
}

void vTestATcommand(void)  //+DST do czasu    //AT+SYSTEMP?
{
	SendToEsp("AT+SYSTIMESTAMP?\r\n");  //rozlaczyc riuter i sprawdzic   //AT+MDNS=1,"espressif","_iot",8080
	while (RecvFromEsp("\r\nOK")==0)
		vTaskDelay(1);
	Dbg(DBG, RecvBuffer);
}

void vDNSdomain(void)
{
	SendToEsp("AT+CIPDOMAIN=\"smtp.poczta.onet.pl\"\r\n");
	while (RecvFromEsp("\r\nOK")==0)
		vTaskDelay(1);
	Dbg(DBG, RecvBuffer);
}

void vRestoreESP(void)
{
	SendToEsp("AT+RESTORE\r\n");
	while (RecvFromEsp("\r\nOK")==0)
		vTaskDelay(1);
	Dbg(DBG, RecvBuffer);
}

static void GetSizeAndChannel(char *pBuf, int *channel, int *size)
{
	int i=1, j;
	*channel=0;
	*size=0;
	while ((*(pBuf-i)>0x2F)&&(*(pBuf-i)<0x3A))
	{
		switch (i)
		{
		case 2:
			j=10;
			break;
		case 3:
			j=100;
			break;
		case 4:
			j=1000;
			break;
		default:
			j=1;
			break;
		}
		*size+=((*(pBuf-i))&0x0F)*j;
		i++;
	}
	i++;
	*channel=((*(pBuf-i))&0x0F);
}

static int GetDMACountByte(void)
{
	return ESP_RECV_BUFF_SIZE-__HAL_DMA_GET_COUNTER(&ESP_UART_DMA_RX);
}

static int vSendDataPacket(char *pData, int packetLen, int channel)
{
	char tempBuff[30];
	int itx, commandLen;

	Dbg(DBG, ".");
	commandLen=mini_snprintf(tempBuff, sizeof(tempBuff), "AT+CIPSEND=%d,%d\r\n", channel, packetLen);
	if (HAL_OK!=SendToEsp_DMA(tempBuff, commandLen))
		return 1;

	itx=0;
	while (RecvFromEsp("\r\n>")==0)
	{
		if (RecvFromEsp(",CLOSED"))
			return 5;
		if (RecvFromEsp("ERROR"))
			return 6;

		itx++;
		if (itx>GetAnswerDelay())
			return 2;
		vTaskDelay(1);
	}

	if (HAL_OK!=SendToEsp_DMA(pData, packetLen))
		return 3;

	itx=0;
	while (RecvFromEsp("\r\nSEND OK")==0)
	{
		if (RecvFromEsp(",CLOSED"))
			return 7;
		if (RecvFromEsp("ERROR"))
			return 8;

		itx++;
		if (itx>GetAnswerDelay())
			return 4;
		vTaskDelay(1);
	}
	return 0;
}

static int vCloseConnection(int channel)
{
	int itx=0, commandLen;

	commandLen=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CIPCLOSE=%d\r\n", channel);
	if (HAL_OK!=SendToEsp_DMA(sendBuff, commandLen))
		return 1;
	while (RecvFromEsp("CLOSED")==0)
	{
		itx++;
		if (itx>GetAnswerDelay())
			return 2;
		vTaskDelay(1);
	}
	return 0;
}

static int vSendData(char *pData, int len, int channel)
{
	int i, partial=0, result;

	while (partial<=len-PACKET_SEND_LEN)
	{
		for (i=0; i<PACKET_SEND_LEN; i++)
			sendBuff[i]=*(pData+i+partial);

		result=vSendDataPacket(sendBuff, PACKET_SEND_LEN, channel);
		if (result>0)
			return result;
		partial+=PACKET_SEND_LEN;

	}
	if (len-partial>0)
	{
		for (i=0; i<len-partial; i++)
			sendBuff[i]=*(pData+i+partial);

		result=vSendDataPacket(sendBuff, len-partial, channel);
		if (result>0)
			return result;
	}
	return 0;
}

static int vSendDataHTTP(char *getHttpRequest, int channel)
{
	int result;
	DATA_TO_SEND *temp=GetPageWWW(getHttpRequest);
	result=vSendData(temp->pData, temp->len, channel);
	vPortFree(temp);
	return result;
}

static int vSendDataSMTP(int id)
{
	int result;
	DATA_TO_SEND *temp=GetDataEmail(id);
	if ((result=vSendData(temp->pData, temp->len, ESP_EMAIL_CHANNEL)))
	{
		SetEmailState(SMTP_FAIL);
		SetEmailCode(result);
	}
	vPortFree(temp);
	return result;
}

static int vSendCommandSMTP(char *pCommand, int commandLen)
{
	int result;
	if ((result=vSendDataPacket(pCommand, commandLen, ESP_EMAIL_CHANNEL)))
	{
		SetEmailState(SMTP_FAIL);
		SetEmailCode(100+result);
	}
	return result;
}

static int GetRecvCodeEmail(char *pBuf)
{
	int i=1, j, code=0;
	char *ptr=pBuf;

	ptr++;
	while ((*ptr>0x2F)&&(*ptr<0x3A))
		ptr++;
	while ((*(ptr-i)>0x2F)&&(*(ptr-i)<0x3A))
	{
		switch (i)
		{
		case 2:
			j=10;
			break;
		case 3:
			j=100;
			break;
		case 4:
			j=1000;
			break;
		default:
			j=1;
			break;
		}
		code+=((*(ptr-i))&0x0F)*j;
		i++;
	}
	return code;
}

static char *vWaitForRecv(int timeoutWait)
{
	int itx=0;
	char *ptr,*ptrP;

	while ((ptr=RecvFromEsp("+IPD,"))==0)
	{
		itx++;
		if (itx>timeoutWait)
			return ptr;
		vTaskDelay(1);
	}
	while ((ptrP=strstr(ptr,":"))==0)
	{
		itx++;
		if (itx>timeoutWait)
			return ptrP;
		vTaskDelay(1);
	}
	ptr=ptrP;
	return ptr;
}

static int vGetEmailRecvCode(int *channel)
{
	char *pRecvBuff;
	int size, codeE, itx=0;

	do
	{
		if ((pRecvBuff=vWaitForRecv(GetAnswerDelay()))==0)
		{
			Dbg(DBG, "\r\nNOTHING RECV ");
			return 0;
		}
		GetSizeAndChannel(pRecvBuff, channel, &size);
		if (itx>0)
		{
			Dbg(DBG, " Repeat email parser ");
			if (itx>5)
				return 20;
		}
		itx++;

	} while (*channel!=ESP_EMAIL_CHANNEL);

	while (GetDMACountByte()<size)
		vTaskDelay(1);
	vTaskDelay(1);
	codeE=GetRecvCodeEmail(pRecvBuff);
	Dbg(DBG, "\r\n");
	Dbg(DBG, pRecvBuff);
	return codeE;
}

static void SendDummyData(int delayMs)
{
	vTaskDelay(delayMs);
	SendToEsp_DMA(sendBuff, PACKET_SEND_LEN);
	vTaskDelay(delayMs);
	SendToEsp_DMA(sendBuff, PACKET_SEND_LEN);
	vTaskDelay(delayMs);
	SendToEsp_DMA(sendBuff, PACKET_SEND_LEN);
	Dbg(DBG, "\r\nSEND DUMMY DATA ");
}

static int GetHttpPort(void)
{
	switch(GetTabVal(Const_wifiGeneral_mode,NO_TAB))
	{
	case WIFI_MODE_AP:
		return GetTabVal(Const_wifiAP_port,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB));
	default:
		return GetTabVal(Const_wifiSTA_port,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB));
	}
}

static int vStartServer(void)
{
	int len,itx=0;

	len=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CIPSERVER=1,%d\r\n", GetHttpPort());
	SendToEsp_DMA(sendBuff,len);
	while (RecvFromEsp("\r\nOK")==0)
	{
		itx++;
		if (itx>GetAnswerDelay())
			return 1;
		vTaskDelay(1);
	}
	return 0;
}

static int vStopServer(void)
{
	int len,itx=0;

	len=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CIPSERVER=0\r\n");
	SendToEsp_DMA(sendBuff,len);
	while (RecvFromEsp("\r\nOK")==0)
	{
		itx++;
		if (itx>GetAnswerDelay())
			return 1;
		vTaskDelay(1);
	}
	return 0;
}

void vRestartWifiServer(void)
{
	vStopServer();
	vTaskDelay(30);
	vStartServer();
}

static bool vCheckEmailAnswer(int emailCode)
{
	int channel, code;

	code=vGetEmailRecvCode(&channel);
	SetEmailCode(code);

	if (code==emailCode)
	{
		SetEmailState(SMTP_INPROGRESS);
		return true;
	}
	else
	{
		SetEmailState(SMTP_FAIL);
		vTaskDelay(200);
		return false;
	}
}

static void EmailSendStart(void)
{
	int len;
	char buftemp[5];

	switch(GetTabVal(Const_wifiGeneral_mode,NO_TAB))
	{
		case WIFI_MODE_STA:
		case WIFI_MODE_AP_STA:

			if(GetTabVal(Const_emailSend_IP, EmailSendParam.whichSender))
			{
				connectionType=SMTP_CONNECTION;
				SetEmailState(SMTP_START);
				EmailSendParam.start=3;
				vStopServer();

				if (GetTabVal(Const_emailSend_useSSL, EmailSendParam.whichSender))
					strcpy(buftemp, "SSL");
				else
					strcpy(buftemp, "TCP");

				len=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CIPSTART=%d,\"%s\",\"%s\",%d\r\n",
						ESP_EMAIL_CHANNEL,
						buftemp,
						IP2Str(GetTabVal(Const_emailSend_IP, EmailSendParam.whichSender)),
						GetTabVal(Const_emailSend_port, EmailSendParam.whichSender));
				SendToEsp_DMA(sendBuff, len);
				Dbg(DBG,"\r\n"); Dbg(DBG,sendBuff);
			}

			break;
	}
}

static void ErrorServiceSMTP(void)
{
	if(GetEmailState()==SMTP_FAIL)
	{
		switch(GetEmailCode())
		{
		case ESPANSWER_TIMEOUT_TO_GET_SEND_OK:
			Dbg(DBG,"\r\nESPANSWER_TIMEOUT_TO_GET_SEND_OK");
			break;
		case ESPANSWER_TIMEOUT_FOR_SMTPCOMM_TO_GET_SEND_OK:
			Dbg(DBG,"\r\nESPANSWER_TIMEOUT_FOR_SMTPCOMM_TO_GET_SEND_OK");
			break;
		case ESPANSWER_CLOSED_AFTER_SEND_DATA:
			Dbg(DBG,"\r\nESPANSWER_CLOSED_AFTER_SEND_DATA");
			break;
		case ESPANSWER_ERROR_AFTER_SEND_DATA:
			Dbg(DBG,"\r\nESPANSWER_ERROR_AFTER_SEND_DATA");
			break;
		case 20:
			break;
		}
	}
}

static int vGetConnectionResultToSTA(void)
{
	int itx=0;
	char *ptr;
	while(1)
	{
		if(RecvFromEsp("\r\nOK"))
		{
			return ESP_CONNECTION_OK;
		}
		else if(RecvFromEsp("ERROR"))
		{
			if ((ptr=RecvFromEsp("+CWJAP:")))
			{
				switch(atoi(ptr+7))
				{
				case 1:
					return ESP_CONNECTION_TIMEOUT;
				case 2:
					return ESP_WRONG_PASSWORD;
				case 3:
					return ESP_CANNOT_FIND_THE_TARGET_AP;
				case 4:
					return ESP_CONNECTION_FAILED;
				default:
					return ESP_UNKNOW_ERROR_OCCURRED;
				}
			}
			else
				return ESP_UNKNOW_ERROR_OCCURRED;
		}
		else
		{
			itx++;
			if(itx>CONNECTION_TIMEOUT_MS)
				return ESP_UNKNOW_ERROR_OCCURRED;
			vTaskDelay(1);
		}
	}
}

static void GetAddressesForConnection(void)
{
	char *ptr;
	int nrAP=GetTabVal(Const_wifiGeneral_nrAP,NO_TAB);
	int nrSTA=GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB);

	if ((ptr=RecvFromEsp("+CIFSR:APIP,\"")))
		SetTabVal(Const_wifiAP_ip, nrAP, IPStr2Int(ptr+13));

	if ((ptr=RecvFromEsp("+CIFSR:APMAC,\"")))
		SetVal64(Const_wifiAP_mac, nrAP, MACStr2Int64(ptr+14));

	if ((ptr=RecvFromEsp("+CIFSR:STAIP,\"")))
		SetTabVal(Const_wifiSTA_ip, nrSTA, IPStr2Int(ptr+14));

	if ((ptr=RecvFromEsp("+CIFSR:STAMAC,\"")))
		SetVal64(Const_wifiSTA_mac, nrSTA, MACStr2Int64(ptr+15));
}

static void vQueryAndReplaceEmailAddrName2AddrIP(void)
{
	int i,len,itx;
	char *ptr;

	for(i=0;i<MAX_EMAIL_SENDERS;++i)
	{
		if(*GetStr(Const_emailSend_server,i))
		{
			len=mini_snprintf(sendBuff,sizeof(sendBuff),"AT+CIPDOMAIN=\"%s\"\r\n",GetStr(Const_emailSend_server,i));
			SendToEsp_DMA(sendBuff,len);  DbgStr(DBG,"\r\n",sendBuff," ");
			itx=0;
			while (1)
			{
				if (RecvFromEsp("\r\nOK"))
				{
					if ((ptr=RecvFromEsp("+CIPDOMAIN:")))
					{
						SetTabVal(Const_emailSend_IP,i,IPStr2Int(ptr+11));
						DbgStr(DBG,"\r\n",ptr,"  ");
						vTaskDelay(50);
						break;
					}
					else
						break;
				}
				else if (RecvFromEsp("ERROR"))
					return;
				else
				{
					itx++;
					if (itx>DNS_SERVER_TIMEOUT_MS)
						return;
					vTaskDelay(1);
				}
			}
		}
	}
}

static int vQueryAndLoadTimeFromSNTP(void)
{
	int itx=0;
	char *ptr;
	time_t getTime;

	while (1)
	{
		SendToEsp("AT+SYSTIMESTAMP?\r\n");
		while (1)
		{
			if (RecvFromEsp("\r\nOK"))
			{
				if ((ptr=RecvFromEsp("+SYSTIMESTAMP:")))
				{
					getTime=(time_t)atoi(ptr+14);
					if(getTime>1565853509)
					{
						SetTabVal(Const_sntp_time,NO_TAB,getTime);
						sntpTime=gmtime(&getTime);
						DbgVar(1,50,"\r\nES TIME LOADED %d; %02d-%02d-%02d  %02d:%02d:%02d",
								GetTabVal(Const_sntp_time,NO_TAB),
								sntpTime->tm_year-100,
								sntpTime->tm_mon+1,
								sntpTime->tm_mday,
								sntpTime->tm_hour,
								sntpTime->tm_min,
								sntpTime->tm_sec);
						return 1;
					}
					Dbg(DBG,"*");
					vTaskDelay(500);
					itx+=500;
					if (itx>SNTP_SERVER_TIMEOUT_MS)
						return 0;
					break;
				}
				else
					return 0;
			}
			else if (RecvFromEsp("ERROR"))
				return 0;

			itx++;
			if (itx>SNTP_SERVER_TIMEOUT_MS)
				return 0;
			vTaskDelay(1);
		}
	}
}

static void vLoadTime(time_t timeSet)
{
	int len=mini_snprintf(sendBuff,sizeof(sendBuff),"AT+SYSTIMESTAMP=%d\r\n",timeSet);
	SendToEsp_DMA(sendBuff,len);
	while (RecvFromEsp("\r\nOK")==0)
		vTaskDelay(10);
}

void vtaskWifi(void *argument)
{
	char *pHttpGet;
	int channel=0, size=0, len, result, result2;
	int j;

	StartDMA();
	ESP_ON;

	connectionType=INIT_CONNECTION;
	EmailSendParam.start=0;
	DefaultSettingsWIFI();
	DefaultSettingsEmail();
	DefaultSettingsDNS();
	DefaultSettingsSNTP();
	ResetTestTab(); //Do USUNIECIA !!!

	Dbg(DBG,"\r\nStart vtaskWifi\r\n");   //StartUp aktivity dla tego watki jezeli nie ma odp na AT to innty watek restartuje ten watek

	while (1)
	{
		if (true==isAnythingRecv())  //Cykliczne odpytatywanie czy nie zawieszony np AT
		{
			switch (connectionType)
			{
			case HTTP_CONNECTION:
				if ((pHttpGet=RecvFromEsp(":GET /")))
				{
					GetSizeAndChannel(pHttpGet, &channel, &size);

					while (GetDMACountByte()<size)
						vTaskDelay(1);

					if (RecvFromEsp(":GET /TME.txt")==0)
						DisplayRequestGET(pHttpGet, 2000);

					result=vSendDataHTTP(pHttpGet, channel);
					DbgVar(DBG, 20, "\r\nSend Code: %d ", result);

					result2=vCloseConnection(channel);
					DbgVar(DBG, 20, "\r\nClose Code: %d ", result2);

					if ((result==2)&&(result2==2))
						SendDummyData(100);
					RestartDMA();
				}
				break;

			case SMTP_CONNECTION:
				if (false==vCheckEmailAnswer(220))
					goto GOTO_Email_Quit;
				EmailSendParam.start=4;

				len=mini_snprintf(sendBuff, sizeof(sendBuff), "EHLO %s\r\n", GetStr(Const_emailSend_name,EmailSendParam.whichSender));
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;
				if (false==vCheckEmailAnswer(250))
					goto GOTO_Email_Quit;

				len=mini_snprintf(sendBuff, sizeof(sendBuff), "AUTH LOGIN\r\n");
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;
				if (false==vCheckEmailAnswer(334))
					goto GOTO_Email_Quit;

				len=mini_snprintf(sendBuff, sizeof(sendBuff), "%s\r\n", base64_enc2(GetStr(Const_emailSend_login,EmailSendParam.whichSender)));
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;
				if (false==vCheckEmailAnswer(334))
					goto GOTO_Email_Quit;

				len=mini_snprintf(sendBuff, sizeof(sendBuff), "%s\r\n", base64_enc2(GetStr(Const_emailSend_password,EmailSendParam.whichSender)));
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;
				if (false==vCheckEmailAnswer(235))
					goto GOTO_Email_Quit;

				len=mini_snprintf(sendBuff, sizeof(sendBuff), "MAIL FROM:<%s>\r\n", GetStr(Const_emailSend_login,EmailSendParam.whichSender));
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;
				if (false==vCheckEmailAnswer(250))
					goto GOTO_Email_Quit;

				j=0;
				for (int i=0; i<MAX_EMAIL_RECIPIENTS; ++i)
				{
					if ((EmailSendParam.recepientsMask>>i)&0x01)
					{
						len=mini_snprintf(sendBuff, sizeof(sendBuff), "RCPT TO:<%s>\r\n", GetStr(Const_emailRecv_email,j));
						if (vSendCommandSMTP(sendBuff, len))
							goto GOTO_Email_Quit;
						if (false==vCheckEmailAnswer(250))
							goto GOTO_Email_Quit;
					}
					j++;
				}

				len=mini_snprintf(sendBuff, sizeof(sendBuff), "DATA\r\n");
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;
				if (false==vCheckEmailAnswer(354))
					goto GOTO_Email_Quit;

				vTaskDelay(1);
				len=mini_snprintf(sendBuff, sizeof(sendBuff), "FROM: %s\r\nSubject: %s\r\n", GetStr(Const_emailSend_login,EmailSendParam.whichSender), GetTitleEmail());
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;

				vTaskDelay(1);
				len=mini_snprintf(sendBuff, sizeof(sendBuff), "TO: ");
				j=0;
				for (int i=0; i<MAX_EMAIL_RECIPIENTS; ++i)
				{
					if ((EmailSendParam.recepientsMask>>i)&0x01)
						len+=mini_snprintf(sendBuff+len, sizeof(sendBuff), "%s,", GetStr(Const_emailRecv_email,j));
					j++;
				}
				len+=mini_snprintf(sendBuff+len, sizeof(sendBuff), "\r\n");
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;
				DbgStr(DBG,"\r\nRecv mail: ",sendBuff,"\r\n");

				vTaskDelay(1);
				len=mini_snprintf(sendBuff, PACKET_SEND_LEN, "Content-Transfer-Encoding: 8bit\r\n");
				len+=mini_snprintf(sendBuff+len, PACKET_SEND_LEN, "Content-Type: text/html; charset=\"UTF-8\"");
				len+=mini_snprintf(sendBuff+len, PACKET_SEND_LEN, "\r\n\r\n");
				if (vSendCommandSMTP(sendBuff, len))
					goto GOTO_Email_Quit;

				vTaskDelay(1);
				if ((result=vSendDataSMTP(EmailSendParam.id)))
				{
					DbgVar(DBG,50, "\r\nResult Error Send Data: %d ", result);
					goto GOTO_Email_Quit;
				}
				if (false==vCheckEmailAnswer(250))
					goto GOTO_Email_Quit;
				else
					SetEmailState(SMTP_SUCCESS);

				GOTO_Email_Quit:
				if ((RecvFromEsp("closing connection")==0)||(RecvFromEsp(",CLOSED")==0))
				{
					if (vSendDataPacket("quit\r\n", 6, ESP_EMAIL_CHANNEL));
					switch (vGetEmailRecvCode(&channel))
					{
					case 221:
						break;
					default:
						vTaskDelay(200);
						break;
					}
				}

				DbgVar(DBG, 40, "\r\nEMAIL STATUS: %d %s ", GetEmailCode(), GetStrEmailState());
				ErrorServiceSMTP();

				EmailSendParam.start=0;
				connectionType=HTTP_CONNECTION;
				vStartServer();
				RestartDMA();
				break;

			case INIT_CONNECTION:
				if (RecvFromEsp("ready"))
				{
					SendToEsp("ATE0\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);

					mini_snprintf(sendBuff, sizeof(sendBuff), "AT+UART_CUR=%d,8,1,0,0\r\n", ESP_UART_BUADRATE);
					SendToEsp(sendBuff);
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);
					HAL_Delay(10);

					ChangeUartBuadRate(ESP_UART_BUADRATE);
					SendToEsp("AT+GMR\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);
					Dbg(DBG, RecvBuffer);

					len=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CWMODE=%d\r\n",GetTabVal(Const_wifiGeneral_mode,NO_TAB));
					SendToEsp_DMA(sendBuff,len);
					DbgStr(DBG,"\r\n",sendBuff,"\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);

					if(WIFI_MODE_DISABLED==GetTabVal(Const_wifiGeneral_mode,NO_TAB))
					{
						Dbg(DBG, "\r\nWifi DISABLED ");
						break;
					}

					SendToEsp("AT+CWLAPOPT=1,23\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);

					SendToEsp("AT+CIPMUX=1\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);

					SendToEsp("AT+CWDHCP=0,3\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);

					if(0==GetTabVal(Const_wifiAP_dhcp,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB))&&
						1==GetTabVal(Const_wifiSTA_dhcp,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB)) )
					{
						SendToEsp("AT+CWDHCP=1,1\r\n");
						while (RecvFromEsp("\r\nOK")==0)
							vTaskDelay(10);
					}
					else if(1==GetTabVal(Const_wifiAP_dhcp,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB))&&
							  0==GetTabVal(Const_wifiSTA_dhcp,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB)) )
					{
						SendToEsp("AT+CWDHCP=1,2\r\n");
						while (RecvFromEsp("\r\nOK")==0)
							vTaskDelay(10);
					}
					else if(1==GetTabVal(Const_wifiAP_dhcp,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB))&&
							  1==GetTabVal(Const_wifiSTA_dhcp,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB)) )
					{
						SendToEsp("AT+CWDHCP=1,3\r\n");
						while (RecvFromEsp("\r\nOK")==0)
							vTaskDelay(10);
					}

					SendToEsp("AT+CWHOSTNAME=\"Elektronika_STM\"\r\n");
					while (RecvFromEsp("\r\nOK")==0)  //WProwadzic variable dla hostName
					{
						if (RecvFromEsp("ERROR"))
							break;
						vTaskDelay(10);
					}

					switch(GetTabVal(Const_wifiGeneral_mode,NO_TAB))
					{
					case WIFI_MODE_STA:
					case WIFI_MODE_AP_STA:
						if(0==GetTabVal(Const_wifiSTA_dhcp,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB)))
						{
							len=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CIPSTA=\"%s\",\"%s\",\"%s\"\r\n",
									IP2Str(GetTabVal(Const_wifiSTA_ip,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB))),
									IP2Str(GetTabVal(Const_wifiSTA_gate,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB))),
									IP2Str(GetTabVal(Const_wifiSTA_mask,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB))));
							SendToEsp_DMA(sendBuff,len);
							DbgStr(DBG,"\r\n",sendBuff,"\r\n");
							while (RecvFromEsp("\r\nOK")==0)
								vTaskDelay(10);
						}
						break;
					}
					switch(GetTabVal(Const_wifiGeneral_mode,NO_TAB))
					{
					case WIFI_MODE_AP:
					case WIFI_MODE_AP_STA:
						if(0==GetTabVal(Const_wifiAP_dhcp,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB)))
						{
							len=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CIPAP=\"%s\",\"%s\",\"%s\"\r\n",
									IP2Str(GetTabVal(Const_wifiAP_ip,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB))),
									IP2Str(GetTabVal(Const_wifiAP_gate,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB))),
									IP2Str(GetTabVal(Const_wifiAP_mask,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB))));
							SendToEsp_DMA(sendBuff,len);
							DbgStr(DBG,"\r\n",sendBuff,"\r\n");
							while (RecvFromEsp("\r\nOK")==0)
								vTaskDelay(10);
						}
						break;
					}

					switch(GetTabVal(Const_wifiGeneral_mode,NO_TAB))
					{
					case WIFI_MODE_AP:
					case WIFI_MODE_AP_STA:
						len=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CWSAP=\"%s\",\"%s\",5,3\r\n",
								GetStr(Const_wifiAP_name,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB)),
								GetStr(Const_wifiAP_pass,GetTabVal(Const_wifiGeneral_nrAP,NO_TAB)));
						SendToEsp_DMA(sendBuff,len);
						DbgStr(DBG,"\r\n",sendBuff,"\r\n");
						while (RecvFromEsp("\r\nOK")==0)
							vTaskDelay(10);
						break;
					}
					switch(GetTabVal(Const_wifiGeneral_mode,NO_TAB))
					{
					case WIFI_MODE_STA:
					case WIFI_MODE_AP_STA:
						len=mini_snprintf(sendBuff, sizeof(sendBuff), "AT+CWJAP=\"%s\",\"%s\"\r\n",
								GetStr(Const_wifiSTA_name,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB)),
								GetStr(Const_wifiSTA_pass,GetTabVal(Const_wifiGeneral_nrSTA,NO_TAB)));  // Timer do logowania !!!!!  i poprawic GetPort !! zamiast port na indeks
						SendToEsp_DMA(sendBuff,len);
						DbgStr(DBG,"\r\n",sendBuff,"\r\n");
						result=vGetConnectionResultToSTA();
						if(ESP_CONNECTION_OK!=result)
							DbgVar(DBG,30,"\r\nERROR_ESP_CONNECTION: %d\r\n",result);
						break;
					}

					SendToEsp("AT+CIFSR\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);
					GetAddressesForConnection();
					Dbg(DBG, RecvBuffer);

					SendToEsp("AT+CIPSERVERMAXCONN=1\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);

					len=mini_snprintf(sendBuff,sizeof(sendBuff),"AT+CIPDNS=1,\"%s\",\"%s\",\"%s\"\r\n",
							IP2Str(GetTabVal(Const_dns_IP1,NO_TAB)),
							IP2Str(GetTabVal(Const_dns_IP2,NO_TAB)),
							IP2Str(GetTabVal(Const_dns_IP3,NO_TAB)));
					SendToEsp_DMA(sendBuff,len);
					DbgStr(DBG,"\r\n",sendBuff,"\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(1);
					Dbg(DBG, RecvBuffer);

					len=mini_snprintf(sendBuff,sizeof(sendBuff),"AT+CIPSNTPCFG=1,%d,\"%s\",\"%s\"\r\n",
							GetTabVal(Const_sntp_timezone,NO_TAB),
							GetStr(Const_sntp_nameServer1,NO_TAB),
							GetStr(Const_sntp_nameServer2,NO_TAB));
					SendToEsp_DMA(sendBuff,len);
					DbgStr(DBG,"\r\n",sendBuff,"\r\n");
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(1);
					Dbg(DBG, RecvBuffer);

					vStartServer();

					len=mini_snprintf(sendBuff,sizeof(sendBuff),"AT+CIPSTO=%d\r\n",TCP_SERVER_TIMEOUT_S);
					SendToEsp_DMA(sendBuff,len);
					while (RecvFromEsp("\r\nOK")==0)
						vTaskDelay(10);

					switch(GetTabVal(Const_wifiGeneral_mode,NO_TAB))
					{
						case WIFI_MODE_STA:
						case WIFI_MODE_AP_STA:
							vQueryAndReplaceEmailAddrName2AddrIP();
							if(0==vQueryAndLoadTimeFromSNTP())
								vLoadTime(GetTabVal(Const_sntp_time,NO_TAB));
							break;
						default:
							vLoadTime(GetTabVal(Const_sntp_time,NO_TAB));
							break;
					}

					Dbg(DBG, "\r\nESP INIT OK ");
					RestartDMA();
					connectionType=HTTP_CONNECTION;
				}
				break;
			}
		}

		if (resetDMA==1)
		{
			resetDMA=0;
			Dbg(DBG, "\r\nERROR USART6 ");
			if (EmailSendParam.start==0)
				SendDummyData(200);
			RestartDMA();
			EmailSendParam.start=0;
			vRestartWifiServer();
		}

		if (EmailSendParam.start==1)
			EmailSendParam.start++;
		else if (EmailSendParam.start==2)
			EmailSendStart();

		vTaskDelay(20);
	}
}

void CreateWifiTask(void)
{
	xTaskCreate(vtaskWifi, "vtaskWifi", 1000, NULL, (unsigned portBASE_TYPE ) 4, &vtaskWifiHandle);
}

void CloseWifiTask(void)
{
	vTaskDelete(vtaskWifiHandle);
	ESP_OFF;
	ChangeUartBuadRate(115200);
}

void RestartWifiTask(void)
{
	CloseWifiTask();
	vTaskDelay(200);
	CreateWifiTask();
}

void WIFI_UartErrorService(void)
{
	resetDMA=1;
}

void WIFI_RxCallbackService(void)
{
	RestartDMA();
	Dbg(DBG, "\r\n -----  USART6  HAL_UART_RxCpltCallback -------  ");
}

//------------- ATTENTIONS ------------------------------
/*
AT version:2.1.0.0(883f7f2 - Jul 24 2020 11:50:07)
SDK version:v4.0.1-193-ge7ac221
compile time(0ad6331):Jul 28 2020 02:47:21
Bin version:2.1.0(WROOM-32)

OK
+CIFSR:APIP,"192.168.4.1"
+CIFSR:APMAC,"4c:11:ae:f9:45:55"
+CIFSR:STAIP,"192.168.1.71"
+CIFSR:STAMAC,"4c:11:ae:f9:45:54"

OK

AT+CIPSTAMAC="1a:fe:35:99:d5:4A"
AT+CIFSR  //...jesli IP 0.0.0.0 to cyklicznie �aczyc z skoja�on� siecia CWJAP (CWSAP) az do polaczenia
AT+CWJAP?  wyswietla parametry sieci do ktorej jest podlaczony
AT+CWSAP?  wyswietla parametry swojej sieci AP
AT+CWLIF: Obtain IP Address of the Station That Connects to an ESP SoftAP

*/
