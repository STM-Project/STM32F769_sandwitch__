/*
 * LCD_comon.c
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */

#include <LCD_Common.h>
#include "stm32f7xx_hal.h"

uint32_t ColorIntensiv(uint32_t colour, int16_t factor)
{
	int16_t a,b,c;	uint32_t color=0x00FFFFFF&colour;
	a=0x000000FF&(color>>16);	b=0x000000FF&(color>>8);	c=0x000000FF&color;
	a+=factor;                  b+=factor;					c+=factor;

	if(a>255) a=255;
	if(a<0)   a=0;

	if(b>255) b=255;
	if(b<0)   b=0;

	if(c>255) c=255;
	if(c<0)   c=0;

	return ( (0x00FF0000&(((uint32_t)a)<<16)) | (0x0000FF00&(((uint32_t)b)<<8)) | (0x000000FF&((uint32_t)c)) );
}
