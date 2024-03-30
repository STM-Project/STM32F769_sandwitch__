/*
 * variables.h
 *
 *  Created on: 09.09.2020
 *      Author: Elektronika RM
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include "stm32f7xx_hal.h"


#define   SIZE_TXT         160

#define   NO_TAB	0

enum{
	Var_slider0,
	Var_slider1,
	Var_slider2,
	Var_gsmStrenght,
	Const_STA_ssid,
	Const_STA_pass,
	Const_AP_ssid,
	Const_AP_pass,
	Const_Wifi_mac,
	Const_Wifi_ip,
	Const_Wifi_mask,
	Const_Wifi_gate,
	Const_Wifi_port,
	Const_Wifi_dhcp,
	Const_Wifi_sta,
	Const_Wifi_ap,
	Const_emailSend_server,
	Const_emailSend_IP,
	Const_emailSend_port,
	Const_emailSend_name,
	Const_emailSend_login,
	Const_emailSend_password,
	Const_emailSend_useSSL,
	Const_emailRecv_email,
	Const_dns_enable,
	Const_dns_IP1,
	Const_dns_IP2,
	Const_dns_IP3,
	Const_sntp_nameServer1,
	Const_sntp_nameServer2,
	Const_sntp_timezone,
	Const_sntp_time,
	Const_wifiAP_mac,
	Const_wifiAP_ip,
	Const_wifiAP_mask,
	Const_wifiAP_gate,
	Const_wifiAP_port,
	Const_wifiAP_dhcp,
	Const_wifiAP_name,
	Const_wifiAP_pass,
	Const_wifiSTA_mac,
	Const_wifiSTA_ip,
	Const_wifiSTA_mask,
	Const_wifiSTA_gate,
	Const_wifiSTA_port,
	Const_wifiSTA_dhcp,
	Const_wifiSTA_name,
	Const_wifiSTA_pass,
	Const_wifiGeneral_nrAP,
	Const_wifiGeneral_nrSTA,
	Const_wifiGeneral_mode,
	Const_Checkbox_test_1,
	Const_Checkbox_test_2,
	Const_Checkbox_test_3,
	Const_Checkbox_test_4,
	Const_Checkbox_test_5
};



void SetVal(int nameVar, int val);
void IncrVal(int nameVar, int incrSize);
void DecrVal(int nameVar, int decrSize);
int GetVal(int nameVar);
void SetTabVal(int nameVar, int nrTab, int val);
int GetTabVal(int nameVar,int nrTab);
void SetStr(int nameVar, int nrTab, char *str);
char* GetStr(int nameVar, int nrTab);
void SetVal64(int nameVar, int nrTab, uint64_t val);
uint64_t GetVal64(int nameVar, int nrTab);

#endif /* VARIABLES_H_ */
