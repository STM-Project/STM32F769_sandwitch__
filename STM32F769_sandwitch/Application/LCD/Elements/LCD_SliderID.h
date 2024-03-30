/*
 * LCD_sliderID.h
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_ELEMENTS_LCD_SLIDERID_H_
#define LCD_ELEMENTS_LCD_SLIDERID_H_

#include "stm32f7xx_hal.h"

typedef enum
{
	SLIDER_ID1,
	SLIDER_ID2,
	SLIDER_ID3,
	SLIDER_ID4,
	SLIDER_ID5,
	SLIDER_ID6,
	SLIDER_ID7,
	SLIDER_ID8,
	SLIDER_ID9,
	SLIDER_ID10
}SLIDER_ID;

void LCD_DrawSliderWithButtons_H(int ID_Slider,int xPos,int yPos, int width,int height, int value, uint16_t touchSliderID, uint16_t touchLeftID,uint16_t touchRightID);
void LCD_DrawSliderWithButtons_V(int ID_Slider,int xPos,int yPos, int width,int height, int value, uint16_t touchSliderID, uint16_t touchUpID,uint16_t touchDownID);

int LCD_SliderButton(int nrSlider,int percentVal);
int LCD_Slider(int nrSlider,int x, int y);

void vLCD_UpSliderTouchEffect(int nrSlider);
void vLCD_DownSliderTouchEffect(int nrSlider);
void vLCD_LeftSliderTouchEffect(int nrSlider);
void vLCD_RightSliderTouchEffect(int nrSlider);

void ClearAllSlider(void);
void LCD_DrawSliderWithButtons(int ID_Slider,int orient,int xPos,int yPos, int width,int height, int varNameID, uint16_t touchSliderID, uint16_t touchLeftID,uint16_t touchRightID);
int TOUCH_SliderService(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y);
void LCD_TxtIDSlider(int ID_Slider, int fontID, int Xpos, int Ypos,int nrTxt);
int IsTouchSlider(int ID_Slider,int ID_touchDetect);

#endif /* LCD_ELEMENTS_LCD_SLIDERID_H_ */
