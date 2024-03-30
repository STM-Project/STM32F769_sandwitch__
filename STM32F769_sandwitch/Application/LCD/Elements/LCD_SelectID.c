/*
 * select.c
 *
 *  Created on: 01.02.2021
 *      Author: RafalMar
 */

#include "LCD_SelectID.h"
#include "LCD_Hardware.h"
#include "LCD_BasicGaphics.h"
#include <string.h>
#include "fonts_images.h"
#include "sdram.h"
#include "lang.h"
#include "mini_printf.h"
#include "touch.h"
#include "dbg.h"

#define MAX_BUFTEMP	500

SRAM LCD_SelectStruct Sel;   //STATIC daj!!!!!1

static char bufTemp[MAX_BUFTEMP];

void ClearAllSelect(void)
{
	for (int i=0; i<MAX_LCD_SELECT; ++i)
		Sel.Param[i].id=0;
	Sel.wsk=0;
}

static int SearchFirstFreeSelect(void)
{
	for (int i=0; i<MAX_LCD_SELECT; ++i)
	{
		if (Sel.Param[i].id==0)
			return i;
	}
	return -1;
}

static int GetSelectNumber(int touchID)
{
	for (int i=0; i<MAX_LCD_SELECT; ++i)
	{
		if (Sel.Param[i].id==touchID)
			return i;
	}
	return -1;
}

char* GetSelTxt(int nrBuff, const char* txt, int line)
{
	int i, j;
	const char *ptr=txt;

	for (i=0; i<line; ++i)
	{
		for (j=0; j<Lang.howMuch; ++j)
			ptr=strstr(ptr, ",")+1;
	}
	for (i=0; i<Lang.what; ++i)
		ptr=strstr(ptr, ",")+1;

	i=0;
	while (ptr[i]!=',')
	{
		if (nrBuff+i>MAX_BUFTEMP-1)
			break;
		bufTemp[nrBuff+i]=ptr[i];
		i++;
	}
	bufTemp[nrBuff+i]=0;
	return &bufTemp[nrBuff];
}

static int GetNumberAllLinesSelTxt(const char* txt)
{
	int i, k=0, len=mini_strlen(txt);

	for (i=0; i<len; ++i)
	{
		if (txt[i]==',')
			k++;
	}
	return k/Lang.howMuch;
}

void LCD_Select(int selectID, int x, int y, int fontTitle, int fontTitlePress, int fontScroll, const char *txt, int line, int touch, int txtVar)
{
	int bkColor=0;
	int fillColorMain=0, 	frameColorMain;
	int fillColorUnroll=0, 	frameColorUnroll;
	int fillColorPress=0, 	frameColorPress;
	int marginFromUp, marginFromLeft, widthDescr, widthAllSelect=0, heightSelect=0;
	int widthTriangle=ReturnHeightFontID(fontTitle);
	int xPos=0, xTriangl=0, nrLine=0, Xoffset=60;

	switch (selectID)
	{
	case SELECT_TYPE_MainMenu:
		fillColorMain=BROWN;				frameColorMain=MYLIGHTGRAY;
		fillColorUnroll=BROWN;			frameColorUnroll=MYLIGHTGRAY;
		fillColorPress=DARKBLUE;		frameColorPress=MYLIGHTGRAY;
		bkColor=BROWN;

		marginFromUp=5, marginFromLeft=10;
		nrLine=0;
		widthTriangle=ReturnHeightFontID(fontTitle);
		widthDescr=marginFromLeft+ReturnWidthFontID(fontTitle, GetSelTxt(0, txt, nrLine))+marginFromLeft;
		heightSelect=ReturnHeightFontID(fontTitle)+2*marginFromUp;
		widthAllSelect=widthDescr+widthTriangle+marginFromLeft;
		if (x+widthAllSelect>LCD_GetXSize())
			xPos=x-(x+widthAllSelect-LCD_GetXSize());
		else
			xPos=x;
		xTriangl=widthDescr;
		LCD_DrawBoldRectangle(xPos, y, widthAllSelect, heightSelect, frameColorMain, fillColorMain, 0);
		LCD_DrawDownTriangle(xPos+xTriangl, y+(heightSelect-widthTriangle/2)/2, widthTriangle, WHITE, WHITE, bkColor);
		break;

	case SELECT_TYPE_Other:
		fillColorMain=DARKGRAY;				frameColorMain=MYLIGHTGRAY;
		fillColorUnroll=DARKMAGENTA;		frameColorUnroll=MYLIGHTGRAY;
		fillColorPress=0x0000C0;			frameColorPress=MYLIGHTGRAY;
		bkColor=DARKGRAY;

		marginFromUp=5, marginFromLeft=10;
		nrLine=GetVal(line);
		widthTriangle=ReturnHeightFontID(fontTitle);
		widthDescr=marginFromLeft+ReturnWidthFontID(fontTitle, GetSelTxt(0, txt, nrLine))+marginFromLeft;
		heightSelect=ReturnHeightFontID(fontTitle)+2*marginFromUp;
		//widthAllSelect= widthDescr+widthTriangle+marginFromLeft;
		xPos=x;
		int width=0, width_p;
		int coutFrame=GetNumberAllLinesSelTxt(txt);
		for (int i=0; i<coutFrame; ++i)
		{
			width_p=ReturnWidthFontID(fontScroll, GetSelTxt(0, txt, i));
			if (width<width_p)
				width=width_p;
		}
		width+=Xoffset;
		widthAllSelect=width;
		xTriangl=widthAllSelect-widthTriangle-marginFromLeft;
		LCD_DrawRectangle(xPos, y, widthAllSelect, heightSelect, frameColorMain, fillColorMain, 0);
		LCD_DrawDownTriangle(xPos+xTriangl, y+(heightSelect-widthTriangle/2)/2, widthTriangle, WHITE, WHITE, bkColor);
		break;
	}

	if (touch>0)
	{
		int k=SearchFirstFreeSelect();
		if (k!=-1)
		{
			TxtID_Var_color(txtVar, fontTitle, xPos+marginFromLeft, y+marginFromUp, GetSelTxt(0, txt, nrLine), fullHight, bkColor, 0, 0);
			Sel.Param[k].id=touch;
			Sel.Param[k].xTriangl=xPos+xTriangl;
			Sel.Param[k].yTriangl=y+(heightSelect-widthTriangle/2)/2;
			Sel.Param[k].xScroll=xPos;
			Sel.Param[k].yScroll=y+heightSelect;
			Sel.Param[k].yScrollUp=y;
			Sel.Param[k].width=widthTriangle;
			Sel.Param[k].frameColorMain=frameColorMain;
			Sel.Param[k].fillColorMain=fillColorMain;
			Sel.Param[k].frameColorUnroll=frameColorUnroll;
			Sel.Param[k].fillColorUnroll=fillColorUnroll;
			Sel.Param[k].frameColorPress=frameColorPress;
			Sel.Param[k].fillColorPress=fillColorPress;
			Sel.Param[k].bkColor=bkColor;
			Sel.Param[k].pTxt=txt;
			Sel.Param[k].nmbTitleVar=txtVar;
			Sel.Param[k].fontTitle=fontTitle;
			Sel.Param[k].fontTitlePress=fontTitlePress;
			Sel.Param[k].fontScroll=fontScroll;
			Sel.Param[k].selType=selectID;
			Sel.Param[k].valName=line;
			Sel.Param[k].xOffset=Xoffset;

			touchTemp.x_Start=xPos;
			touchTemp.y_Start=y;
			touchTemp.x_End=touchTemp.x_Start+widthAllSelect;
			touchTemp.y_End=touchTemp.y_Start+heightSelect;
			SetTouch(touch);
		}
	}
	else
		TxtID_color(fontTitle, xPos+marginFromLeft, y+marginFromUp, GetSelTxt(0, txt, nrLine), fullHight, bkColor, 0, 0);
}

static void LCD_SelectSevice(int fontID, int nmbSel, int ID_touchDetect)
{
	uint32_t width;
	uint32_t width_p;
	int i,j;
	int offset=0;
	int imageHeight, imageWidth, xImg=0, xTxt=0;
	char bufImg[50];
	uint32_t ColorObram, ColorWypel;
	int coutFrame;
	int xPosSel;

	int marginFromLeft=10;
	int marginFromUp=5;

	uint32_t height=marginFromUp+ReturnHeightFontID(fontID)+marginFromUp;
	coutFrame=GetNumberAllLinesSelTxt(Sel.Param[nmbSel].pTxt);
	width=0;
	switch (Sel.Param[Sel.nrLcdSelect].selType)
	{
	case SELECT_TYPE_MainMenu:
		offset=1;
		coutFrame--;
		mini_snprintf(bufImg, 50, "LCD_images/term_vlow.bmp");
		break;
	case SELECT_TYPE_Other:
		offset=0;
		mini_snprintf(bufImg, 50, "LCD_images/wilg_vlow.bmp");
		break;
	}
	for (i=0; i<coutFrame; ++i)
	{
		width_p=ReturnWidthFontID(fontID, GetSelTxt(0, Sel.Param[nmbSel].pTxt, offset+i));
		if (width<width_p)
			width=width_p;
	}
	width+=Sel.Param[Sel.nrLcdSelect].xOffset;

	if (Sel.Param[nmbSel].xScroll+width>LCD_GetXSize())
		xPosSel=Sel.Param[nmbSel].xScroll-(Sel.Param[nmbSel].xScroll+width-LCD_GetXSize());
	else
		xPosSel=Sel.Param[nmbSel].xScroll;

	LCD_DrawRectangleBuff(0, width, height*coutFrame, 0, 0, 0); //t³o
	LCD_DrawBmpBuff(1000000, bufImg);
	imageHeight=0x0000FFFF&LCD_GetWidthHeightBmp(bufImg);
	imageWidth=0x0000FFFF&(LCD_GetWidthHeightBmp(bufImg)>>16);

	switch (Sel.Param[Sel.nrLcdSelect].selType)
	{
	case SELECT_TYPE_MainMenu:
		xImg=(height-imageHeight)/2;
		xTxt=(height-imageHeight)+imageWidth;
		break;
	case SELECT_TYPE_Other:
		xImg=width-imageWidth-marginFromLeft;
		xTxt=marginFromLeft;
		break;
	}

	for (i=0; i<coutFrame; ++i)    //UWAGA ZWIEKSZ ROZMIAR &pLcd[0] !!!!!    pLcd[450000] - > np pLcd[800000]
	{
		if ((ID_touchDetect==0)&&(Sel.Param[Sel.nrLcdSelect].selType==SELECT_TYPE_MainMenu))
		{
			ColorObram=Sel.Param[Sel.nrLcdSelect].frameColorUnroll;
			ColorWypel=Sel.Param[Sel.nrLcdSelect].fillColorUnroll;
		}
		else if((ID_touchDetect==0)&&(Sel.Param[Sel.nrLcdSelect].selType==SELECT_TYPE_Other))
		{
			if (GetVal(Sel.Param[Sel.nrLcdSelect].valName)==i){
				ColorObram=Sel.Param[Sel.nrLcdSelect].frameColorPress;
				ColorWypel=Sel.Param[Sel.nrLcdSelect].fillColorPress;
			}
			else{
				ColorObram=Sel.Param[Sel.nrLcdSelect].frameColorUnroll;
				ColorWypel=Sel.Param[Sel.nrLcdSelect].fillColorUnroll;
			}
		}
		else
		{
			if (ID_touchDetect-SELECT_TOUCH==i){
				ColorObram=Sel.Param[Sel.nrLcdSelect].frameColorPress;
				ColorWypel=Sel.Param[Sel.nrLcdSelect].fillColorPress;
			}
			else{
				ColorObram=Sel.Param[Sel.nrLcdSelect].frameColorUnroll;
				ColorWypel=Sel.Param[Sel.nrLcdSelect].fillColorUnroll;
			}
		}
		LCD_DrawRectangleBuff(850000, width, height, ColorObram, ColorWypel, 0);
		TxtID_color_BufferWindow(850000, fontID, xTxt, marginFromUp, GetSelTxt(0, Sel.Param[nmbSel].pTxt, offset+i), fullHight, width, ColorWypel, 0);
		LCD_ImageComposition(0, 850000, 0, height*i-1*i);
		LCD_ImageComposition(0, 1000000, xImg, (height-imageHeight)/2+height*i-1*i);
	}

	int totalHeight=coutFrame*(height-1)+1;
	if (Sel.Param[nmbSel].yScroll-1+totalHeight>LCD_GetYSize())
	{
		if (Sel.Param[nmbSel].yScroll-1<=LCD_GetYSize()/2)  // w I polowce u gory
		{
			//szeregowo w dol
			int c=0;
			int countFrameAdd=(Sel.Param[nmbSel].yScroll-1+totalHeight-LCD_GetYSize())/(height-1)+1;
			int maxFrameInColumn=coutFrame-countFrameAdd;
			int pixelColumn=maxFrameInColumn*(height-1)+1;
			int maxFullColumns=coutFrame/maxFrameInColumn;
			int frameInLastColumn=coutFrame-maxFullColumns*maxFrameInColumn;
			int pixelLastColumn=frameInLastColumn*(height-1)+1;
			int pixelAllWidth=frameInLastColumn==0 ? maxFullColumns*width : (maxFullColumns+1)*width;
			int signOffset;

			if (xPosSel>(LCD_GetXSize()/2)&&(xPosSel+pixelAllWidth)>LCD_GetXSize())
				signOffset=-1;
			else
				signOffset=1;

			for (i=0; i<maxFullColumns; ++i)
			{
				LCD_DisplayBuff(4+i*(3*width*(pixelColumn-1)), xPosSel+i*(width-1)*signOffset, Sel.Param[nmbSel].yScroll-1, width, pixelColumn);
				for (j=0; j<maxFrameInColumn; ++j)
				{
					touchTemp.x_Start=xPosSel+i*(width-1)*signOffset;
					touchTemp.x_End=touchTemp.x_Start+width;
					touchTemp.y_Start=Sel.Param[nmbSel].yScroll-1+j*(height-1);
					touchTemp.y_End=touchTemp.y_Start+height;
					SetTouch(SELECT_TOUCH+c);
					c++;
				}
			}
			if (frameInLastColumn)
			{
				LCD_DisplayBuff(4+i*(3*width*(pixelColumn-1)), xPosSel+i*(width-1)*signOffset, Sel.Param[nmbSel].yScroll-1, width, pixelLastColumn);
				for (j=0; j<frameInLastColumn; ++j)
				{
					touchTemp.x_Start=xPosSel+i*(width-1)*signOffset;
					touchTemp.x_End=touchTemp.x_Start+width;
					touchTemp.y_Start=Sel.Param[nmbSel].yScroll-1+j*(height-1);
					touchTemp.y_End=touchTemp.y_Start+height;
					SetTouch(SELECT_TOUCH+c);
					c++;
				}
			}
		}
		else  // w II polowce na dol
		{
			if (Sel.Param[nmbSel].yScrollUp+1>=totalHeight)
			{
				// tylko w gore
				LCD_DisplayBuff(4, xPosSel, Sel.Param[nmbSel].yScrollUp+1-totalHeight, width, totalHeight);
				for (i=0; i<coutFrame; ++i)
				{
					touchTemp.x_Start=xPosSel;
					touchTemp.x_End=touchTemp.x_Start+width;
					touchTemp.y_Start=Sel.Param[nmbSel].yScrollUp+1-totalHeight+i*(height-1);
					touchTemp.y_End=touchTemp.y_Start+height;
					SetTouch(SELECT_TOUCH+i);
				}
			}
			else
			{
				//szeregowo w gore
				int c=0;
				int countFrameAdd=(totalHeight-(Sel.Param[nmbSel].yScrollUp+1))/(height-1)+1;
				int maxFrameInColumn=coutFrame-countFrameAdd;
				int pixelColumn=maxFrameInColumn*(height-1)+1;
				int maxFullColumns=coutFrame/maxFrameInColumn;
				int frameInLastColumn=coutFrame-maxFullColumns*maxFrameInColumn;
				int pixelLastColumn=frameInLastColumn*(height-1)+1;
				int pixelAllWidth=frameInLastColumn==0 ? maxFullColumns*width : (maxFullColumns+1)*width;
				int signOffset;

				if (xPosSel>(LCD_GetXSize()/2)&&(xPosSel+pixelAllWidth)>LCD_GetXSize())
					signOffset=-1;
				else
					signOffset=1;

				for (i=0; i<maxFullColumns; ++i)
				{
					LCD_DisplayBuff(4+i*(3*width*(pixelColumn-1)), xPosSel+i*(width-1)*signOffset, Sel.Param[nmbSel].yScrollUp+1-pixelColumn, width, pixelColumn);
					for (j=0; j<maxFrameInColumn; ++j)
					{
						touchTemp.x_Start=xPosSel+i*(width-1)*signOffset;
						touchTemp.x_End=touchTemp.x_Start+width;
						touchTemp.y_Start=Sel.Param[nmbSel].yScrollUp+1-pixelColumn+j*(height-1);
						touchTemp.y_End=touchTemp.y_Start+height;
						SetTouch(SELECT_TOUCH+c);
						c++;
					}
				}
				if (frameInLastColumn)
				{
					LCD_DisplayBuff(4+i*(3*width*(pixelColumn-1)), xPosSel+i*(width-1)*signOffset, Sel.Param[nmbSel].yScrollUp+1-pixelColumn, width, pixelLastColumn);
					for (j=0; j<frameInLastColumn; ++j)
					{
						touchTemp.x_Start=xPosSel+i*(width-1)*signOffset;
						touchTemp.x_End=touchTemp.x_Start+width;
						touchTemp.y_Start=Sel.Param[nmbSel].yScrollUp+1-pixelColumn+j*(height-1);
						touchTemp.y_End=touchTemp.y_Start+height;
						SetTouch(SELECT_TOUCH+c);
						c++;
					}
				}
			}
		}
	}
	else
	{  //tylko w dol
		LCD_DisplayBuff(4, xPosSel, Sel.Param[nmbSel].yScroll-1, width, totalHeight);
		for (i=0; i<coutFrame; ++i)
		{
			touchTemp.x_Start=xPosSel;
			touchTemp.x_End=touchTemp.x_Start+width;
			touchTemp.y_Start=Sel.Param[nmbSel].yScroll-1+i*(height-1);
			touchTemp.y_End=touchTemp.y_Start+height;
			SetTouch(SELECT_TOUCH+i);
		}
	}
}

static void LCD_SelectTriangleIndirect(int selectTriangleID, int k)
{
	switch (selectTriangleID)
	{
	case 0:
		LCD_DrawRectangleBuff(0, Sel.Param[k].width, Sel.Param[k].width/2, 0, 0, 0); //t³o
		LCD_DrawDownTriangleBuff(400000, Sel.Param[k].width, MYBLUE, MYBLUE, Sel.Param[k].bkColor);
		LCD_ImageComposition(0, 400000, 0, 0);
		LCD_DisplayBuff(4, Sel.Param[k].xTriangl, Sel.Param[k].yTriangl, Sel.Param[k].width, Sel.Param[k].width/2+1);
		break;
	}
}

void LCD_SelectUnroll(int touchID)
{
	int nrLcdSelect_p=GetSelectNumber(touchID);

	if (nrLcdSelect_p!=-1)
	{
		if (0==Sel.wsk)	//press select and scrol list
		{
			DeleteAllTouchWithout(touchID);
			Sel.nrLcdSelect=nrLcdSelect_p;
			Sel.wsk=1;
			int selTxtTop=0;
			switch (Sel.Param[Sel.nrLcdSelect].selType)
			{
			case SELECT_TYPE_MainMenu:
				selTxtTop=0;
				break;
			case SELECT_TYPE_Other:
				selTxtTop=GetVal(Sel.Param[Sel.nrLcdSelect].valName);
				break;
			}
			LCD_Txt_color(Sel.Param[Sel.nrLcdSelect].nmbTitleVar, (char*) GetSelTxt(0, Sel.Param[Sel.nrLcdSelect].pTxt, selTxtTop), Sel.Param[Sel.nrLcdSelect].fontTitlePress, fullHight, Sel.Param[Sel.nrLcdSelect].bkColor, 0, 0);
			LCD_SelectTriangleIndirect(0, Sel.nrLcdSelect);
			LCD_SelectSevice(Sel.Param[Sel.nrLcdSelect].fontScroll, Sel.nrLcdSelect, 0);
		}
		else if (2==Sel.wsk)	//press select and back to latest screen
			Sel.wsk=3;
	}
}

int LCD_SelectRoll(int touchID)
{
	if (Sel.wsk>0)
	{
		Sel.wsk++;
		if (4==Sel.wsk)
		{
			Sel.wsk=0;
			for (int i=SELECT_TOUCH; i<MAX_SELECT_TOUCH; ++i)
				DeleteSelectTouch(i);

			switch (Sel.Param[Sel.nrLcdSelect].selType)
			{
			case SELECT_TYPE_MainMenu:
			case SELECT_TYPE_Other:
				return 1;
			}
		}
	}
	return 0;
}

int LCD_SelectPress(int ID_touchDetect)
{
	if((ID_touchDetect>=SELECT_TOUCH)&&(ID_touchDetect<MAX_SELECT_TOUCH)&&(Sel.wsk>0))
	{
		if(Sel.Param[Sel.nrLcdSelect].id)
			LCD_SelectSevice(Sel.Param[Sel.nrLcdSelect].fontScroll,Sel.nrLcdSelect,ID_touchDetect);
		return Sel.Param[Sel.nrLcdSelect].id;
	}
	else
		return 0;
}

int GetSelectLineNr(int ID_touchDetect)
{
	return ID_touchDetect-SELECT_TOUCH;
}

int IsSelectRoll(void)
{
	if(Sel.wsk)
		return 0;
	else
		return 1;
}



#include "LCD_Common.h"
#include "SCREEN_Common.h"
#include "variables.h"
#include "cmsis_os.h"

void vTOUCH_SelectService(int ID_touchDetect, int pressRelease)
{
	if (ID_touchDetect)
	{
		switch (pressRelease)
		{
		case PRESSED:
			LCD_SelectUnroll(ID_touchDetect);
			switch (LCD_SelectPress(ID_touchDetect))
			{
			case 0:
				break;

			case TOUCH_mainMenu:
				FUNCTION_MainMenuSelect(GetSelectLineNr(ID_touchDetect));  // w screen_common.h
				break;

			default:
				osDelay(500);
				SetVal(Sel.Param[Sel.nrLcdSelect].valName, GetSelectLineNr(ID_touchDetect));
				SCREEN_Load(Screen.current, Screen.param, 1);  // To przenies st¹d !!!
				break;
			}
			break;

		case RELEASED:
			if(ID_touchDetect<SELECT_TOUCH)
			{
				if (LCD_SelectRoll(ID_touchDetect))
					SCREEN_Load(Screen.current, Screen.param, 1);
			}
			break;
		}
	}
}
