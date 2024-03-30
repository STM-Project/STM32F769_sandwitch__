/*
 * sntp_dns.c
 *
 *  Created on: 17.01.2021
 *      Author: Elektronika RM
 */

#include "stm32f7xx_hal.h"
#include <string.h>
#include "sntp_dns.h"
#include "variables.h"
#include "def.h"

struct tm *sntpTime;

void DefaultSettingsDNS(void)
{
	SetTabVal(Const_dns_enable, NO_TAB, 1);
	SetTabVal(Const_dns_IP1, NO_TAB, LWIP_MAKEU32(8,8,8,8));
	SetTabVal(Const_dns_IP2, NO_TAB, LWIP_MAKEU32(8,8,8,8));
	SetTabVal(Const_dns_IP3, NO_TAB, LWIP_MAKEU32(8,8,8,8));
}

void DefaultSettingsSNTP(void)
{
	SetStr(Const_sntp_nameServer1, NO_TAB, "vega.cbk.poznan.pl");
	SetStr(Const_sntp_nameServer2, NO_TAB, "ntp1.tp.pl");
	SetTabVal(Const_sntp_timezone, NO_TAB, 1);

	sntpTime->tm_year=20+100;
	sntpTime->tm_mon=6-1;
	sntpTime->tm_mday=15;
	sntpTime->tm_hour=23;
	sntpTime->tm_min=59;
	sntpTime->tm_sec=34;
	SetTabVal(Const_sntp_time, NO_TAB, mktime(sntpTime));
}
