/*
 * dbg.c
 *
 *  Created on: 17.07.2020
 *      Author: Elektronika RM
 */

#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "dbg.h"
#include "mini_printf.h"

#define RECV_BUFF_SIZE	100

UART_HandleTypeDef huart7;
static char dbgRecvBuffer[RECV_BUFF_SIZE];
static int errorUART;

void dbg(char *txt)
{
	HAL_UART_Transmit(&huart7, (uint8_t*)txt, strlen(txt),30);
}

void Dbg(int on, char *txt)
{
	if(on)
		HAL_UART_Transmit(&huart7, (uint8_t*)txt, strlen(txt),300);
}

void DbgStr(int on, char *startTxt, char *txt, char *endTxt)
{
	if(on)
	{
		dbg(startTxt);
		dbg(txt);
		dbg(endTxt);
	}
}

void DbgVar(int on, unsigned int buffer_len, const char *fmt, ...)
{
	if(on)
	{
		char buffer[buffer_len+1];
		va_list va;
		va_start(va, fmt);
		mini_vsnprintf(buffer, buffer_len, fmt, va);
		va_end(va);
		dbg_roz(buffer);
	}
}

void dbg_DMA(char *txt)
{
	HAL_UART_Transmit_DMA(&huart7, (uint8_t*)txt, strlen(txt));
}

void dbg_roz(char *txt)
{
	HAL_UART_Transmit(&huart7, (uint8_t*)txt, strlen(txt),230);
}

void DbgSprintf(unsigned int buffer_len, const char *fmt, ...)
{
	char buffer[buffer_len+1];
	va_list va;
	va_start(va, fmt);
	mini_vsnprintf(buffer, buffer_len, fmt, va);
	va_end(va);
	dbg_roz(buffer);
}

void Init_DBG(void)
{
	memset(dbgRecvBuffer,0,RECV_BUFF_SIZE);
	HAL_UART_Receive_DMA(&huart7, (uint8_t*)dbgRecvBuffer, RECV_BUFF_SIZE);
}

bool isDBG_Recv(char *txt)
{
	if(strstr(dbgRecvBuffer,txt))
	{
		//errorUART = HAL_UART_GetError(&huart7);
		HAL_UART_DMAStop(&huart7);
		memset(dbgRecvBuffer,0,RECV_BUFF_SIZE);
		HAL_UART_Receive_DMA(&huart7, (uint8_t*)dbgRecvBuffer, RECV_BUFF_SIZE);
		return true;
	}
	return false;
}

bool isAnything_Recv(void)
{
	if(dbgRecvBuffer[0]>0)
		return true;
	else
		return false;
}


void DBG_RxCallbackService(void)
{
	errorUART = HAL_UART_GetError(&huart7);
	dbg(dbgRecvBuffer);
	memset(dbgRecvBuffer,0,RECV_BUFF_SIZE);
	HAL_UART_Receive_DMA(&huart7, (uint8_t*)dbgRecvBuffer, RECV_BUFF_SIZE);
}







