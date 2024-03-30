/*
 * LCD_CheckboxID.c
 *
 *  Created on: 05.02.2021
 *      Author: RafalMar
 */
#include "LCD_CheckboxID.h"
#include "sdram.h"
#include "LCD_BasicGaphics.h"
#include "touch.h"
#include "fonts_images.h"
#include "variables.h"
#include "LCD_Common.h"

#define MAX_CHECKBOX	 200

SRAM static LCD_CHECKBOXStruct Checkbox[MAX_CHECKBOX];

void ClearAllCheckbox(void)
{
	for(int i=0;i<MAX_CHECKBOX;++i)
		Checkbox[i].id=0;
}

int SearchFirstFreeCheckbox(void)
{
	for(int i=0;i<MAX_CHECKBOX;++i){
		if(Checkbox[i].id==0) return i;
	}
	return -1;
}

int GetCheckboxNumber(int touchID)
{
	for(int i=0;i<MAX_CHECKBOX;++i){
		if(Checkbox[i].id==touchID) return i;
	}
	return -1;
}

void LCD_Checkbox(int checkboxID, int valName, int x, int y, int size, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch)
{
	struct struct_Checkparam
	{
		int x;
		int y;
		int size;
	}Checkparam;

	Checkparam.x= 0;
	Checkparam.y= 0;
	Checkparam.size= 0;

	switch(checkboxID)
	{
	case Rect:
	case Rect_b:
	case Rect_r:
	default:
		LCD_DrawRectangle(x,y, size, size, frameColor, fillColor,0);
		break;
	case Bold_Rect:
	case Bold_Rect_b:
	case Bold_Rect_r:
		LCD_DrawBoldRectangle(x,y, size, size, frameColor, fillColor,0);
		break;
	case Round_Rect_1:
	case Round_Rect_1b:
	case Round_Rect_1r:
		LCD_DrawRoundRectangle1(x,y, size, size, frameColor, fillColor,bkColor);
		break;
	case Bold_Round_Rect_1:
	case Bold_Round_Rect_1b:
	case Bold_Round_Rect_1r:
		LCD_DrawBoldRoundRectangle1(x,y, size, size, frameColor, fillColor,bkColor);
		break;
	case Round_Rect_2:
	case Round_Rect_2b:
	case Round_Rect_2r:
		LCD_DrawRoundRectangle2(x,y, size, size, frameColor, fillColor,bkColor);
		break;
	case Bold_Round_Rect_2:
	case Bold_Round_Rect_2b:
	case Bold_Round_Rect_2r:
		LCD_DrawBoldRoundRectangle2(x,y, size, size, frameColor, fillColor,bkColor);
		break;
	case Round_Rect_3:
	case Round_Rect_3b:
	case Round_Rect_3r:
		LCD_DrawRoundRectangle3(x,y, size, size, frameColor, fillColor,bkColor);
		break;
	case Bold_Round_Rect_3:
	case Bold_Round_Rect_3b:
	case Bold_Round_Rect_3r:
		LCD_DrawBoldRoundRectangle3(x,y, size, size, frameColor, fillColor,bkColor);
		break;
	}

	switch (checkboxID)
	{
	case Rect:
	case Bold_Rect:
	case Round_Rect_1:
	case Bold_Round_Rect_1:
	case Round_Rect_2:
	case Bold_Round_Rect_2:
	case Round_Rect_3:
	case Bold_Round_Rect_3:
		Checkparam.x=x+2;
		Checkparam.y=y+size/5;
		Checkparam.size=size-4;
		if (GetVal(valName))
			LCD_DrawChecked(Checkparam.x, Checkparam.y, Checkparam.size, checkColor);
		break;

	case Rect_b:
	case Bold_Rect_b:
	case Round_Rect_1b:
	case Bold_Round_Rect_1b:
	case Round_Rect_2b:
	case Bold_Round_Rect_2b:
	case Round_Rect_3b:
	case Bold_Round_Rect_3b:
		Checkparam.x=x+2;
		Checkparam.y=y+size/5;
		Checkparam.size=size-4;
		if (GetVal(valName))
			LCD_DrawBoldChecked(Checkparam.x, Checkparam.y, Checkparam.size, checkColor);
		break;

	case Rect_r:
	case Bold_Rect_r:
		Checkparam.x=x+size/4;
		Checkparam.y=y+size/4;
		Checkparam.size=size/2;
		if (GetVal(valName))
			LCD_DrawRectangle(Checkparam.x, Checkparam.y, Checkparam.size, Checkparam.size, checkColor, checkColor, 0);
		break;

	case Round_Rect_1r:
	case Bold_Round_Rect_1r:
		Checkparam.x=x+size/4;
		Checkparam.y=y+size/4;
		Checkparam.size=size/2;
		if (GetVal(valName))
			LCD_DrawRoundRectangle1(Checkparam.x, Checkparam.y, Checkparam.size, Checkparam.size, checkColor, checkColor, fillColor);
		break;

	case Round_Rect_2r:
	case Bold_Round_Rect_2r:
	case Round_Rect_3r:
	case Bold_Round_Rect_3r:
		Checkparam.x=x+size/4;
		Checkparam.y=y+size/4;
		Checkparam.size=size/2;
		if (GetVal(valName))
			LCD_DrawRoundRectangle3(Checkparam.x, Checkparam.y, Checkparam.size, Checkparam.size, checkColor, checkColor, fillColor);
		break;
	}

	if(touch>0)
	{
		int k= SearchFirstFreeCheckbox();
		if(k!=-1)
		{
			Checkbox[k].id=touch;
			Checkbox[k].type=checkboxID;
			Checkbox[k].xBox=x;
			Checkbox[k].yBox=y;
			Checkbox[k].xCheck=Checkparam.x;
			Checkbox[k].yCheck=Checkparam.y;
			Checkbox[k].size=Checkparam.size;
			Checkbox[k].bkColor=bkColor;
			Checkbox[k].frameColor=frameColor;
			Checkbox[k].fillColor=fillColor;
			Checkbox[k].checkColor=checkColor;
			Checkbox[k].valName=valName;

			touchTemp.x_Start= x;
			touchTemp.y_Start= y;
			touchTemp.x_End= x+size;
			touchTemp.y_End= y+size;
			SetTouch(touch);
		}
	}
}

void LCD_DescrCheckboxLeft(int checkboxID, int valName, int x,int xBox, int yBox, int size,int descrFontID,char *descrTxt, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch)
{
	TxtID(descrFontID,x, yBox+(size-ReturnHeightHalfFontID(descrFontID))/2 ,descrTxt,fullHight,0);
	if(xBox==param_auto)
		LCD_Checkbox(checkboxID,valName, x+ReturnWidthFontID(descrFontID,descrTxt)+ReturnWidthFontID(descrFontID,"2"), yBox, size, frameColor,fillColor,bkColor,checkColor,touch);
	else
		LCD_Checkbox(checkboxID,valName, xBox, yBox, size, frameColor,fillColor,bkColor,checkColor,touch);
}

void LCD_DescrCheckboxRight(int checkboxID, int valName, int xBox,int x, int yBox, int size,int descrFontID,char *descrTxt, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch)
{
	LCD_Checkbox(checkboxID,valName, xBox, yBox, size, frameColor,fillColor,bkColor,checkColor,touch);
	TxtID(descrFontID,x, yBox+(size-ReturnHeightHalfFontID(descrFontID))/2 ,descrTxt,fullHight,0);
}

void LCD_DescrCheckboxUp(int checkboxID, int valName, int xBox,int yBox,int ySpace, int size,int descrFontID,char *descrTxt, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch)
{
	int xDescrOffset= (size-ReturnWidthFontID(descrFontID,descrTxt))/2;
	TxtID(descrFontID,xBox+xDescrOffset, yBox-ySpace-ReturnHeightHalfFontID(descrFontID) ,descrTxt,fullHight,0);
	LCD_Checkbox(checkboxID,valName, xBox, yBox, size, frameColor,fillColor,bkColor,checkColor,touch);
}

void LCD_DescrCheckboxDown(int checkboxID, int valName, int xBox,int yBox,int ySpace, int size,int descrFontID,char *descrTxt, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch)
{
	int xDescrOffset= (size-ReturnWidthFontID(descrFontID,descrTxt))/2;
	LCD_Checkbox(checkboxID,valName, xBox, yBox, size, frameColor,fillColor,bkColor,checkColor,touch);
	TxtID(descrFontID,xBox+xDescrOffset, yBox+size+ySpace,descrTxt,fullHight,0);
}

int TOUCH_CheckboxID(int CheckboxID, int touchID, int k)
{
	switch(CheckboxID)
	{
	case 0:
	default:
		switch(Checkbox[k].type)
		{
		case Rect:
		case Bold_Rect:
		case Round_Rect_1:
		case Bold_Round_Rect_1:
		case Round_Rect_2:
		case Bold_Round_Rect_2:
		case Round_Rect_3:
		case Bold_Round_Rect_3:
			switch(GetVal(Checkbox[k].valName))
			{
			case No_Checked:
				LCD_DrawCheckedBuff(0, Checkbox[k].size, Checkbox[k].checkColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,Checked);
				break;
			case Checked:
				LCD_DrawCheckedBuff(0, Checkbox[k].size, Checkbox[k].fillColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,No_Checked);
				break;
			}
			LCD_DisplayBuff(4, Checkbox[k].xCheck, Checkbox[k].yCheck, Checkbox[k].size, Checkbox[k].size/2+1);
			break;

		case Rect_b:
		case Bold_Rect_b:
		case Round_Rect_1b:
		case Bold_Round_Rect_1b:
		case Round_Rect_2b:
		case Bold_Round_Rect_2b:
		case Round_Rect_3b:
		case Bold_Round_Rect_3b:
			switch(GetVal(Checkbox[k].valName))
			{
			case No_Checked:
				LCD_DrawBoldCheckedBuff(0, Checkbox[k].size, Checkbox[k].checkColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,Checked);
				break;
			case Checked:
				LCD_DrawBoldCheckedBuff(0, Checkbox[k].size, Checkbox[k].fillColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,No_Checked);
				break;
			}
			LCD_DisplayBuff(4, Checkbox[k].xCheck, Checkbox[k].yCheck, Checkbox[k].size, Checkbox[k].size/2+1);
			break;

		case Rect_r:
		case Bold_Rect_r:
			switch(GetVal(Checkbox[k].valName))
			{
			case No_Checked:
				LCD_DrawRectangleBuff(0, Checkbox[k].size,Checkbox[k].size, Checkbox[k].checkColor,Checkbox[k].checkColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,Checked);
				break;
			case Checked:
				LCD_DrawRectangleBuff(0, Checkbox[k].size,Checkbox[k].size, Checkbox[k].fillColor,Checkbox[k].fillColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,No_Checked);
				break;
			}
			LCD_DisplayBuff(4, Checkbox[k].xCheck, Checkbox[k].yCheck, Checkbox[k].size, Checkbox[k].size);
			break;

		case Round_Rect_1r:
		case Bold_Round_Rect_1r:
			switch(GetVal(Checkbox[k].valName))
			{
			case No_Checked:
				LCD_DrawRoundRectangle1Buff(0, Checkbox[k].size,Checkbox[k].size, Checkbox[k].checkColor,Checkbox[k].checkColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,Checked);
				break;
			case Checked:
				LCD_DrawRoundRectangle1Buff(0, Checkbox[k].size,Checkbox[k].size, Checkbox[k].fillColor,Checkbox[k].fillColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,No_Checked);
				break;
			}
			LCD_DisplayBuff(4,Checkbox[k].xCheck,Checkbox[k].yCheck, Checkbox[k].size, Checkbox[k].size);
			break;

		case Round_Rect_2r:
		case Bold_Round_Rect_2r:
		case Round_Rect_3r:
		case Bold_Round_Rect_3r:
			switch(GetVal(Checkbox[k].valName))
			{
			case No_Checked:
				LCD_DrawRoundRectangle3Buff(0, Checkbox[k].size,Checkbox[k].size, Checkbox[k].checkColor,Checkbox[k].checkColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,Checked);
				break;
			case Checked:
				LCD_DrawRoundRectangle3Buff(0, Checkbox[k].size,Checkbox[k].size, Checkbox[k].fillColor,Checkbox[k].fillColor,Checkbox[k].fillColor);
				SetVal(Checkbox[k].valName,No_Checked);
				break;
			}
			LCD_DisplayBuff(4,Checkbox[k].xCheck,Checkbox[k].yCheck, Checkbox[k].size, Checkbox[k].size);
			break;

		default:
			return -1;
		}
		break;
	}
	return GetVal(Checkbox[k].valName);
}

void TOUCH_CheckboxService(int ID_touchDetect, int pressRelease)
{
	static int onePress=0;	int k;

	if (ID_touchDetect)
	{
		k=GetCheckboxNumber(ID_touchDetect);
		switch (pressRelease)
		{
		case PRESSED:
			if((k>-1)&&(onePress==0))
			{
				onePress=1;
				TOUCH_CheckboxID(0,ID_touchDetect,k);
			}
			break;

		case RELEASED:
			if(-1!=k)
				onePress=0;
			break;
		}
	}
}
