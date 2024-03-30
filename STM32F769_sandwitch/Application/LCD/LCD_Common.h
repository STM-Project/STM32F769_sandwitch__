/*
 * LCD_common.h
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_LCD_COMMON_H_
#define LCD_LCD_COMMON_H_

#include "stm32f7xx_hal.h"

typedef enum
{
  PRESSED,
  RELEASED
}touchEvent;

enum generalEnum
{
  param_auto
};

typedef enum
{
	HORIZONTAL,
	VERTICAL
}LCD_ORIENTATION;

typedef void(*frameShape)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);

uint32_t ColorIntensiv(uint32_t colour, int16_t factor);

#endif /* LCD_LCD_COMMON_H_ */
