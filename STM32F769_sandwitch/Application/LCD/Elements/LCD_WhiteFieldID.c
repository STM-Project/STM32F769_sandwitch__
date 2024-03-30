/*
 * LCD_whiteFieldID.c
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */
#include "LCD_WhiteFieldID.h"
#include "LCD_BasicGaphics.h"
#include "LCD_Common.h"
#include "fonts_images.h"
#include "stm32f7xx_hal.h"
#include "sdram.h"
#include "touch.h"
#include "SCREEN_Keyboard.h"
#include "mini_printf.h"

#define MAX_WHITE_FIELD	 40

SRAM LCD_WhiteFieldStruct Field[MAX_WHITE_FIELD];

void ClearAllWhiteField(void)
{
	for(int i=0;i<MAX_WHITE_FIELD;++i)
		Field[i].id=0;
}

int SearchFirstFreeWhiteField(void)
{
	for(int i=0;i<MAX_WHITE_FIELD;++i){
		if(Field[i].id==0) return i;
	}
	return -1;
}

int GetWhiteFieldNumber(int touchID)
{
	for(int i=0;i<MAX_WHITE_FIELD;++i){
		if(Field[i].id==touchID) return i;
	}
	return -1;
}

void DeleteWhiteField(int touchID)
{
	for(int i=0;i<MAX_WHITE_FIELD;++i)
	{
		if(Field[i].id==touchID)
		{
			Field[i].id=0;
			break;
		}
	}
}

int LCD_WhiteField(int fieldID, int fontID, int x, int y, int width, char *txt, uint16_t touch, int space, int txtVar)
{
	int fillColor, frameColor, bkColor, width_p;
	int heightFont			= ReturnHeightFontID(fontID);
	int marginFromLeft	= ReturnWidthFontID(fontID,"2");
	int marginFromUp 		= 1+heightFont/6;

	if(width==param_auto)
		width_p=ReturnWidthFontID(fontID,txt)+2*marginFromLeft+space*(mini_strlen(txt)-1);
	else
		width_p=width;

	switch(fieldID)
	{
	case WHITEFIELD_Type1:
		fillColor=WHITE;
		frameColor=MYLIGHTGRAY;
		bkColor=DARKGRAY;
		LCD_DrawRoundRectangle3(x,y, width_p, heightFont+2*marginFromUp, frameColor, fillColor,bkColor);
		break;
	case WHITEFIELD_Type2:
		fillColor=WHITE;
		frameColor=MYLIGHTGRAY;
		bkColor=DARKGRAY;
		LCD_DrawBoldRoundRectangle3(x,y, width_p, heightFont+2*marginFromUp, frameColor, fillColor,bkColor);
		break;
	}
	if(touch>0)
	{
		TxtID_Var(txtVar,fontID, x+marginFromLeft, y+marginFromUp, txt, fullHight,space);
		int k= SearchFirstFreeWhiteField();
		if(k!=-1)
		{
			Field[k].id=touch;
			Field[k].x=x;
			Field[k].y=y;
			Field[k].width=width_p;
			Field[k].height=heightFont+2*marginFromUp;
			Field[k].bkColor=bkColor;
			Field[k].frameColor=frameColor;
			Field[k].fillColor=fillColor;
			Field[k].txtVar=txtVar;
			StrBuffCopy(Field[k].txtTab,txt);
			Field[k].font=fontID;
			Field[k].space=space;

			touchTemp.x_Start= x;
			touchTemp.y_Start= y;
			touchTemp.x_End= touchTemp.x_Start+width_p;
			touchTemp.y_End= touchTemp.y_Start+heightFont+2*marginFromUp;
			SetTouch(touch);
		}
	}
	else
		TxtID(fontID, x+marginFromLeft, y+marginFromUp, txt, fullHight,space);
	return y+marginFromUp+ReturnHeightHalfFontID(fontID);
}

void LCD_DescrWhiteField(int fieldID,int x,int xField,int yField,int ySpace,int widthField, int descrFontID,char *descrTxt, int fieldFontID,char *fieldTxt, int touch, int space, int txtVar)
{
   if(ySpace==0)
   	TxtID( descrFontID, x,
   			 LCD_WhiteField(fieldID,fieldFontID, xField==param_auto?x+(mini_strlen(descrTxt)+1)*ReturnWidthFontID(descrFontID,"2"):xField, yField,widthField,fieldTxt,touch,space,txtVar) - ReturnHeightHalfFontID(descrFontID),
				 descrTxt, fullHight, 0 );
   else
   {
	   TxtID(descrFontID,x,	yField-ySpace-ReturnHeightHalfFontID(descrFontID) ,descrTxt,fullHight,0);
	   LCD_WhiteField(fieldID,fieldFontID, xField,yField, widthField,fieldTxt,touch,space,txtVar);
   }
}

void TOUCH_WhiteFieldID(int whietFieldID, int touchID)
{
  int k=GetWhiteFieldNumber(touchID);
  if(k!=-1)
  {
	switch(whietFieldID)
	{
	case 0:
		LCD_DrawRectangleBuff(0,Field[k].width+4,Field[k].height+4,Field[k].bkColor,Field[k].bkColor,Field[k].bkColor); //t這
		LCD_DrawBoldRoundRectangle3Buff(350000,Field[k].width+4,Field[k].height+4, LIGHTBLUE,WHITE,Field[k].bkColor);
		LCD_DrawBoldRoundRectangle3Buff(700000,Field[k].width,Field[k].height, BLUE,WHITE,LIGHTBLUE);
		LCD_ImageComposition(0,350000,0,0);
		LCD_ImageComposition(0,700000,2,2);
		LCD_DisplayBuff(4,Field[k].x-2,Field[k].y-2, Field[k].width+4, Field[k].height+4);
		LCD_Txt_color(Field[k].txtVar, Field[k].txtTab, Field[k].font, halfHight,WHITE,0,1);
		break;

	case 1:
		LCD_DrawRectangleBuff(0,Field[k].width+4,Field[k].height+4,Field[k].bkColor,Field[k].bkColor,Field[k].bkColor); //t這
		LCD_DrawRoundRectangle3Buff(700000,Field[k].width,Field[k].height, Field[k].frameColor,Field[k].fillColor,Field[k].bkColor);
		LCD_ImageComposition(0,700000,2,2);
		LCD_DisplayBuff(4,Field[k].x-2,Field[k].y-2, Field[k].width+4, Field[k].height+4);
		LCD_Txt(Field[k].txtVar, Field[k].txtTab, Field[k].font, halfHight,1);
		break;

	case 2: //for keyboard next WhiteField line
		LCD_DrawRectangleBuff(0,Field[k].width+4,Field[k].height+4,Field[k].bkColor,Field[k].bkColor,Field[k].bkColor); //t這
		LCD_DrawBoldRoundRectangle3Buff(700000,Field[k].width,Field[k].height, Field[k].frameColor,Field[k].fillColor,Field[k].bkColor);
		LCD_ImageComposition(0,700000,2,2);
		LCD_DisplayBuff(4,Field[k].x-2,Field[k].y-2+KeyBoard.nextLine*KeyBoard.pixelBetweenLines, Field[k].width+4, Field[k].height+4);
		break;

	case 3: //for keyboard erase next WhiteField line
		LCD_DrawRectangleBuff(0,Field[k].width+4,Field[k].height+4,Field[k].bkColor,Field[k].bkColor,Field[k].bkColor); //t這
		LCD_DisplayBuff(4,Field[k].x-2,Field[k].y-2+KeyBoard.nextLine*KeyBoard.pixelBetweenLines, Field[k].width+4, Field[k].height+4);
		break;
	}
  }
}

int vTOUCH_WhiteFieldService(int ID_touchDetect, int pressRelease)
{
	if (ID_touchDetect)
	{
		switch (pressRelease)
		{
		case PRESSED:
			for(int i=0;i<MAX_WHITE_FIELD;++i)
			{
				if(Field[i].id==ID_touchDetect)
				{
					TOUCH_WhiteFieldID(0,Field[i].id);
					//osDelay(1000);
					//TOUCH_WhiteFieldID(1,ID_touchDetect);
					return Field[i].id;
				}
			}
			break;
		}
	}
	return -1;
}
