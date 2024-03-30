/*
 * float.h
 *
 *  Created on: 17.07.2020
 *      Author: Elektronika RM
 */

#ifndef FLOAT_H_
#define FLOAT_H_

#include "stm32f7xx_hal.h"

#define IPDOT(a,b,c,d)	((a<<24)|(b<<16)|(c<<8)|(d))

#define MAX_ROUND_SIZE	7

uint32_t IPStr2Int(char *str);
char* IP2Str(uint32_t value);
char* int2str(char* buf, const int value);
char* Int64ToHexStr(uint64_t value);
char* Int2DecStr(uint32_t value);
uint64_t MACStr2Int64(char *str);
char* MAC2Str(uint64_t value);
char* int2perctStr(uint32_t value, char freeSign);

void dbl2stri(char *buffer, double value, unsigned int decDigits);
void float2stri(char *buffer, float value, unsigned int decDigits);

#endif /* FLOAT_H_ */
