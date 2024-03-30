/*
 * LCD_ButtonID.c
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */
#include "LCD_ButtonID.h"
#include "LCD_BasicGaphics.h"
#include "LCD_Common.h"
#include "stm32f7xx_hal.h"
#include "touch.h"
#include "sdram.h"
#include "mini_printf.h"
#include "fonts_images.h"

#define MAX_BUTTON	 40

SRAM LCD_BUTTONStruct Button[MAX_BUTTON];

void ClearAllButton(void)
{
	for(int i=0;i<MAX_BUTTON;++i)
		Button[i].id=0;
}

int SearchFirstFreeButton(void)
{
	for(int i=0;i<MAX_BUTTON;++i){
		if(Button[i].id==0) return i;
	}
	return -1;
}

int GetButtonNumber(int touchID)
{
	for(int i=0;i<MAX_BUTTON;++i){
		if(Button[i].id==touchID) return i;
	}
	return -1;
}

void LCD_Button(int buttonID, int fontID,int fontPress, int x, int y, int width, char *txt, int frameColor, int fillColor, int bkColor, uint16_t touch, int txtVar)
{
	int width_p;
	int heightFont		  = ReturnHeightFontID(fontID);
	int marginFromLeft  = ReturnWidthFontID(fontID,"2");
	int marginFromUp 	  = 1+heightFont/2;
	int space=0;

	if(width==param_auto)
		width_p=ReturnWidthFontID(fontID,txt)+2*marginFromLeft+space*(mini_strlen(txt)-1);
	else
		width_p=width;

	switch(buttonID)
	{
	case 0:
	default:
		LCD_DrawRectangle(x,y, width_p, heightFont+2*marginFromUp, frameColor, fillColor,0);
		break;
	case 1:
		LCD_DrawRoundRectangle1(x,y, width_p, heightFont+2*marginFromUp, frameColor, fillColor,bkColor);
		break;
	case 2:
		LCD_DrawRoundRectangle3(x,y, width_p, heightFont+2*marginFromUp, frameColor, fillColor,bkColor);
		break;
	}
	if(touch>0)
	{
		TxtID_Var_color(txtVar,fontID, x+marginFromLeft, y+marginFromUp, txt, fullHight,fillColor,0,space);
		int k= SearchFirstFreeButton();
		if(k!=-1)
		{
			Button[k].id=touch;
			Button[k].x=x;
			Button[k].y=y;
			Button[k].width=width_p;
			Button[k].height=heightFont+2*marginFromUp;
			Button[k].bkColor=bkColor;
			Button[k].frameColor=frameColor;
			Button[k].fillColor=fillColor;
			StrBuffCopy(Button[k].txtTab,txt);
			Button[k].txtVar=txtVar;
			Button[k].font=fontID;
			Button[k].fontPress=fontPress;
			Button[k].buttonID=buttonID;

			touchTemp.x_Start= x;
			touchTemp.y_Start= y;
			touchTemp.x_End= touchTemp.x_Start+width_p;
			touchTemp.y_End= touchTemp.y_Start+heightFont+2*marginFromUp;
			SetTouch(touch);
		}
	}
	else
		TxtID_color(fontID, x+marginFromLeft, y+marginFromUp, txt, fullHight,fillColor,0,space);
}

int TOUCH_ButtonID(int buttonID, int touchID)
{
	int k=GetButtonNumber(touchID);
	if (k!=-1)
	{
		uint32_t frameColor, fillColor;
		frameColor=ColorIntensiv(Button[k].fillColor, +100);
		fillColor=ColorIntensiv(Button[k].fillColor, -100);

		switch (buttonID)
		{
		case 0:
		default:
			switch (Button[k].buttonID)
			{
			case 0:
			default:
				LCD_DrawRectangleBuff(0, Button[k].width+4, Button[k].height+4, Button[k].bkColor, Button[k].bkColor, Button[k].bkColor); //tlo
				LCD_DrawBoldRectangleBuff(350000, Button[k].width+4, Button[k].height+4, frameColor, fillColor, Button[k].bkColor);
				LCD_DrawBoldRectangleBuff(700000, Button[k].width, Button[k].height, frameColor, fillColor, frameColor);
				LCD_ImageComposition(0, 350000, 0, 0);
				LCD_ImageComposition(0, 700000, 2, 2);
				LCD_DisplayBuff(4, Button[k].x-2, Button[k].y-2, Button[k].width+4, Button[k].height+4);
				LCD_Txt_color(Button[k].txtVar, Button[k].txtTab, Button[k].fontPress, fullHight, fillColor, 0, 0);
				break;
			case 1:
				LCD_DrawRectangleBuff(0, Button[k].width+4, Button[k].height+4, Button[k].bkColor, Button[k].bkColor, Button[k].bkColor); //tlo
				LCD_DrawBoldRoundRectangle1Buff(350000, Button[k].width+4, Button[k].height+4, frameColor, fillColor, Button[k].bkColor);
				LCD_DrawBoldRoundRectangle1Buff(700000, Button[k].width, Button[k].height, frameColor, fillColor, frameColor);
				LCD_ImageComposition(0, 350000, 0, 0);
				LCD_ImageComposition(0, 700000, 2, 2);
				LCD_DisplayBuff(4, Button[k].x-2, Button[k].y-2, Button[k].width+4, Button[k].height+4);
				LCD_Txt_color(Button[k].txtVar, Button[k].txtTab, Button[k].fontPress, fullHight, fillColor, 0, 0);
				break;
			case 2:
				LCD_DrawRectangleBuff(0, Button[k].width+4, Button[k].height+4, Button[k].bkColor, Button[k].bkColor, Button[k].bkColor); //tlo
				LCD_DrawBoldRoundRectangle3Buff(350000, Button[k].width+4, Button[k].height+4, frameColor, fillColor, Button[k].bkColor);
				LCD_DrawBoldRoundRectangle3Buff(700000, Button[k].width, Button[k].height, frameColor, fillColor, frameColor);
				LCD_ImageComposition(0, 350000, 0, 0);
				LCD_ImageComposition(0, 700000, 2, 2);
				LCD_DisplayBuff(4, Button[k].x-2, Button[k].y-2, Button[k].width+4, Button[k].height+4);
				LCD_Txt_color(Button[k].txtVar, Button[k].txtTab, Button[k].fontPress, fullHight, fillColor, 0, 0);
				break;
			}
			break;
		}
	}
	return k;
}

int TOUCH_ButtonService(int ID_touchDetect, int pressRelease)
{
	if (ID_touchDetect)
	{
		switch (pressRelease)
		{
		case PRESSED:
			if(-1!=TOUCH_ButtonID(0,ID_touchDetect))
				return ID_touchDetect;

		case RELEASED:
			break;
		}
	}
	return -1;
}
