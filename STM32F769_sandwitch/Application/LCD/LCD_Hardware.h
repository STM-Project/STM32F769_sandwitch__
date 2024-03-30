/*
 * lcd.h
 *
 *  Created on: 02.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#include "stm32f7xx_hal.h"

void BSP_LCD_DrawXXX(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint8_t *pbmp);
void BSP_LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp);
void DMA2D_CopyBuffer(uint32_t *pSrc, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize, uint32_t width_offset);
void BSP_LCD_Display(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint32_t *pbmp);

uint32_t LCD_GetXSize(void);
uint32_t LCD_GetYSize(void);
void LCD_Init(void);
void BSP_LCD_Clear(uint32_t Color);
void BSP_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code);

#endif /* LCD_LCD_H_ */
