
#include "StartUp.h"
#include "dbg.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"

EventGroupHandle_t taskSequence;

void WaitForAccess(int bits)
{
	while((xEventGroupWaitBits(taskSequence, bits,pdFALSE, pdFALSE, 100) & bits) != bits); //sprawdz dzialanie pdFALSE !!!
	xEventGroupClearBits(taskSequence, bits);
}

void ReleaseAccess(int bits)
{
	xEventGroupSetBits(taskSequence, bits);
}

void STARTUP_Init(void)
{
	taskSequence = xEventGroupCreate();
	xEventGroupSetBits(taskSequence, 0x0000);
}

uint32_t STARTUP_WaitForBits(uint32_t bits)
{
	if ((xEventGroupWaitBits(taskSequence, bits,pdFALSE, pdFALSE, 100) & bits) == bits)
		return 1;
	else
		return 0;
}

void STARTUP_SetBits(uint32_t bits)
{
	xEventGroupSetBits(taskSequence, bits);
}

void STARTUP_ClaerBits(uint32_t bits)
{
	xEventGroupClearBits(taskSequence, bits);
}

static int setBits=0;

void CLEAR_allBits(void)
{
	setBits=0;
}
void SET_Bit(uint32_t bits)
{
	setBits |= bits;
}
void CLEAR_Bit(uint32_t bits)
{
	setBits &= ~bits;
}
int GET_Bit(uint32_t bits)
{
	if(setBits&bits)
		return 1;
	else
		return 0;
}

#define MAX_COUNT_TIME	40
static portTickType timeVar[MAX_COUNT_TIME];

void AllTimeReset(void)
{
	for(int i=0; i<MAX_COUNT_TIME; ++i)
		timeVar[i] = xTaskGetTickCount();
}
void TimeReset(int nrTime)
{
	timeVar[nrTime] = xTaskGetTickCount();
}
int IsTimeout(int nrTime, int timeout)
{
	if((xTaskGetTickCount()-timeVar[nrTime])>timeout)
		return 1;
	else
		return 0;
}

#define MAX_MEASUR_TIME		10
static portTickType measurTime[MAX_MEASUR_TIME];
void StartMeasurTime(int nr)
{
	measurTime[nr] = xTaskGetTickCount();
}
void StopMeasurTime(int nr, char *nameTime)
{
	int measurTimeStop = xTaskGetTickCount();
	DbgSprintf(20,"%s:%d ",nameTime,measurTimeStop-measurTime[nr]);
}
