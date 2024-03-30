/*
 * float.c
 *
 *  Created on: 17.07.2020
 *      Author: Elektronika RM
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "float.h"
#include "mini_printf.h"

#define SIZE_STRBUFF	50
static char strBuff[SIZE_STRBUFF], iBuff=0;

uint32_t IPStr2Int(char *str)
{
	char *ptr;
	StrBuffCopylimit(strBuff,str,16);
	return IPDOT(strtoll(strBuff,&ptr,10),strtoll(ptr+1,&ptr,10),strtoll(ptr+1,&ptr,10),strtoll(ptr+1,NULL,10));
}

uint64_t MACStr2Int64(char *str)
{
	char *ptr;
	uint64_t mac[6];
	StrBuffCopylimit(strBuff,str,40);
	mac[0]=strtoll(strBuff,&ptr,16);
	mac[1]=strtoll(ptr+1,&ptr,16);
	mac[2]=strtoll(ptr+1,&ptr,16);
	mac[3]=strtoll(ptr+1,&ptr,16);
	mac[4]=strtoll(ptr+1,&ptr,16);
	mac[5]=strtoll(ptr+1,NULL,16);
	return mac[0]<<40 | mac[1]<<32 | mac[2]<<24 | mac[3]<<16 | mac[4]<<8 | mac[5];
}

char* IP2Str(uint32_t value)
{
	int len, MaxSize=16;

	if(iBuff+MaxSize >= SIZE_STRBUFF)
		iBuff=0;
	len=1+mini_snprintf(strBuff+iBuff,MaxSize,"%d.%d.%d.%d",(uint8_t)(value>>24),(uint8_t)(value>>16),(uint8_t)(value>>8),(uint8_t)value);
	iBuff+=len;
	return strBuff+iBuff-len;
}

char* MAC2Str(uint64_t value)
{
	mini_snprintf(strBuff,18,"%02x:%02x:%02x:%02x:%02x:%02x",(uint8_t)(value>>40),(uint8_t)(value>>32),(uint8_t)(value>>24),(uint8_t)(value>>16),(uint8_t)(value>>8),(uint8_t)value);
	return strBuff;
}

char* Int64ToHexStr(uint64_t value)
{
 	itoa(value>>32, strBuff, 16);
	itoa(value, &strBuff[10], 16);
	strncat(strBuff,&strBuff[10],strlen(&strBuff[10]));
	return strBuff;
}

char* Int2DecStr(uint32_t value)
{
 	itoa(value, strBuff, 10);
	return strBuff;
}

char* int2str(char* buf, const int value)
{
	itoa(value, buf, 10);
	if(value<10)
	{
		buf[1]=buf[0];
		buf[0]='0';
		buf[2]=0;
	}

	return buf;
}

char* int2perctStr(uint32_t value, char freeSign)
{
	char buf[5];
	itoa(value, buf, 10);
	if(value<10)
	{
		strBuff[0]=freeSign;
		strBuff[1]=freeSign;
		strBuff[2]=buf[0];
	}
	else if(value<100)
	{
		strBuff[0]=freeSign;
		strBuff[1]=buf[0];
		strBuff[2]=buf[1];
	}
	else
	{
		strBuff[0]=buf[0];
		strBuff[1]=buf[1];
		strBuff[2]=buf[2];
	}
	strBuff[3]=0;
	return strBuff;
}

const double round_nums[MAX_ROUND_SIZE] =
{ 0.5, 0.05, 0.005, 0.0005, 0.00005,0.000005,0.0000005};

void dbl2stri(char *buffer, double value, unsigned int decDigits)
{
	int idx;
	int64_t dbl_int, dbl_frac;
	int64_t mult = 1;
	char *output = buffer;
	char tbfr[40];

	if (isfinite(value))
	{
		if ((value <= -99999999999999) || (value >= 99999999999999))
		{
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
		}
		else
		{
			if (value < 0.0)
			{
				*output++ = '-';
				value *= -1.0;
			}

			if (decDigits < MAX_ROUND_SIZE)
			{

				value += round_nums[decDigits];
				for (idx = 0; idx < decDigits; idx++)
					mult *= 10;
			}
			else
			{
				decDigits = 5;
				value += round_nums[6];
				mult = 100000;
			}

			dbl_int = (int64_t) value;
			dbl_frac = (int64_t) ((value - (double) dbl_int) * (double) mult);

			idx = 0;
			while (dbl_int != 0)
			{
				tbfr[idx++] = '0' + (dbl_int % 10);
				dbl_int /= 10;
			}

			if (idx == 0)
				*output++ = '0';
			else
			{
				while (idx > 0)
				{
					*output++ = tbfr[idx - 1];
					idx--;
				}
			}

			if (decDigits > 0)
			{
				*output++ = '.';

				idx = 0;
				while (dbl_frac != 0)
				{
					tbfr[idx++] = '0' + (dbl_frac % 10);
					dbl_frac /= 10;
				}
				while (idx < decDigits)
					tbfr[idx++] = '0';

				if (idx == 0)
					*output++ = '0';
				else
				{
					while (idx > 0)
					{
						*output++ = tbfr[idx - 1];
						idx--;
					}
				}
			}
		}
	}
	else
	{
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
	}
	*output = 0;
}

void float2stri(char *buffer, float value, unsigned int decDigits)
{
	int idx;
	int64_t dbl_int, dbl_frac;
	int64_t mult = 1;
	char *output = buffer;
	char tbfr[40];

	if (isfinite(value))
	{
		if ((value <= -99999999999999) || (value >= 99999999999999))
		{
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
		}
		else
		{
			if (value < 0.0)
			{
				*output++ = '-';
				value *= -1.0;
			}

			if (decDigits < MAX_ROUND_SIZE)
			{

				value += round_nums[decDigits];
				for (idx = 0; idx < decDigits; idx++)
					mult *= 10;
			}
			else
			{
				decDigits = 5;
				value += round_nums[6];
				mult = 100000;
			}

			dbl_int = (int64_t) value;
			dbl_frac = (int64_t) ((value - (double) dbl_int) * (double) mult);

			idx = 0;
			while (dbl_int != 0)
			{
				tbfr[idx++] = '0' + (dbl_int % 10);
				dbl_int /= 10;
			}

			if (idx == 0)
				*output++ = '0';
			else
			{
				while (idx > 0)
				{
					*output++ = tbfr[idx - 1];
					idx--;
				}
			}

			if (decDigits > 0)
			{
				*output++ = '.';

				idx = 0;
				while (dbl_frac != 0)
				{
					tbfr[idx++] = '0' + (dbl_frac % 10);
					dbl_frac /= 10;
				}
				while (idx < decDigits)
					tbfr[idx++] = '0';

				if (idx == 0)
					*output++ = '0';
				else
				{
					while (idx > 0)
					{
						*output++ = tbfr[idx - 1];
						idx--;
					}
				}
			}
		}
	}
	else
	{
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
	}
	*output = 0;
}
