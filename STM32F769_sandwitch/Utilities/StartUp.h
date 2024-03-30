/*
 * StartUp.h
 *
 *  Created on: 22.02.2017
 *      Author: Tomaszs
 */

#ifndef STARTUP_H_
#define STARTUP_H_

#include <stdint.h>

enum SET_BITS
{
	Bit_1 = 0x00000001,
	Bit_2 = 0x00000002,
	Bit_3 = 0x00000004,
	Bit_4 = 0x00000008,
	Bit_5 = 0x00000010,
	Bit_6 = 0x00000020,
	Bit_7 = 0x00000040,
	Bit_8 = 0x00000080,

};

void WaitForAccess(int bits);
void ReleaseAccess(int bits);

void STARTUP_Init(void);

uint32_t STARTUP_WaitForBits(uint32_t bits);
void STARTUP_SetBits(uint32_t bits);
void STARTUP_ClaerBits(uint32_t bits);

void CLEAR_allBits(void);
void SET_Bit(uint32_t bits);
void CLEAR_Bit(uint32_t bits);
int GET_Bit(uint32_t bits);

void AllTimeReset(void);
void TimeReset(int nrTime);
int IsTimeout(int nrTime, int timeout);
void StartMeasurTime(int nr);
void StopMeasurTime(int nr, char *nameTime);

#endif /* STARTUP_H_ */
