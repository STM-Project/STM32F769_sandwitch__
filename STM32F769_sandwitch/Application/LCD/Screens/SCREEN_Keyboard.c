/*
 * SCREEN_Keyboard.c
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */
#include <LCD_BasicGaphics.h>
#include <LCD_Hardware.h>
#include <LCD_SelectID.h>
#include <LCD_WhiteFieldID.h>
#include "SCREEN_Keyboard.h"
#include "sdram.h"
#include "string.h"
#include "touch.h"
#include "fonts_images.h"
#include "mini_printf.h"
#include "LCD_ButtonID.h"
#include "SCREEN_WifiSet.h"
#include "dbg.h"
#include "SCREEN_Common.h"
#include "task.h"

#define MAX_WHITE_FIELD		10
#define MAX_SiNGS_IN_KEYBOARD		60

#define FONT_MAIN_MENU			fontID_16
#define MAX_BUFTEMP	500

#define MAX_KEYBOARD	 100
#define MAX_BUFTEMP	500

enum touchKeyboard{
	TOUCH_capitalLetter=1,
	TOUCH_whiteFieldKey,
	TOUCH_back=99,
	TOUCH_firstLetter=100,
	TOUCH_lastLetter=TOUCH_firstLetter+MAX_SiNGS_IN_KEYBOARD,
	TOUCH_backspace,
	TOUCH_anotherSigns,
	TOUCH_nextAnotherSigns
};

enum fontWifiSet{   // DO USUNIECIA bo POPWTARZA SIE w WIFISET !!!!!!
	FontSelMainMenuTitle				=fontID_1,
	FontSelMainMenuTitlePress		=fontID_2,
	FontSelMainMenuSubtitle	    	=fontID_3,
	FontDescription 					=fontID_4,
	FontWhiteField						=fontID_5,
	FontSelOtherTitle					=fontID_6,
	FontSelOtherTitlePress			=fontID_7,
	FontSelOtherSubtitle	   		=fontID_8,
	FontButton			   			=fontID_9,
	FontButtonPress	   			=fontID_10
};

static const char KeySignsS[]="qwertyuiopasdfghjklzxcvbnm ,.+123456-789.0";
static const char KeySignsC[]="QWERTYUIOPASDFGHJKLZXCVBNM ,.+123456-789.0";
static const char KeySignsA1[]="•π∆Ê Í£≥—Ò”ÛåúèüØø:;[]{}<> ,.+123456-789.0";
static const char KeySignsA2[]="'~*()&#^=_$%∞@|?!/******** ,.+123456-789.0";

static char bufTemp[MAX_BUFTEMP];

LCD_keyboard KeyBoard;
SRAM LCD_KEYBOARDStruct Key[MAX_KEYBOARD];

static int cursorToggleColor=WHITE;

void KEYBOARD_CursorToggle(void)
{
	LCD_CursorIndirect(KeyBoard.cursorPixelPosX, KeyBoard.cursorPixelPosY, KeyBoard.cursorPixelWidth, cursorToggleColor=(cursorToggleColor==WHITE) ? BLACK:WHITE );
}

static char GetKeySign(int nr)
{
	switch (KeyBoard.pressAnotherSigns)
	{
	case 0:
		switch (KeyBoard.pressCapitalLetter)
		{
		case 0:
			if (nr<sizeof(KeySignsS))
				return KeySignsS[nr];
			break;
		case 1:
		default:
			if (nr<sizeof(KeySignsC))
				return KeySignsC[nr];
			break;
		}
		break;

	case 1:
		if (nr<sizeof(KeySignsA1))
			return KeySignsA1[nr];
		break;

	case 2:
	default:
		if (nr<sizeof(KeySignsA2))
			return KeySignsA2[nr];
		break;
	}
	return 0;
}

 void ClearKeyBoardStruct(void)
{
	KeyBoard.pressCapitalLetter=0;
	KeyBoard.pressAnotherSigns=0;
	memset(KeyBoard.txt,0,SIZE_TXT);
    KeyBoard.lenTxt=0;
	KeyBoard.nextLine=0;
	KeyBoard.cursorPixelPosX=0;
	KeyBoard.cursorPixelPosY=0;
	KeyBoard.cursorPixelWidth=0;
	KeyBoard.cursorTxtPos=0;
}

void ClearAllKey(void)
{
	for(int i=0;i<MAX_KEYBOARD;++i)
		Key[i].id=0;
}

int SearchFirstFreeKey(void)
{
	for(int i=0;i<MAX_KEYBOARD;++i){
		if(Key[i].id==0) return i;
	}
	return -1;
}

int GetKeyNumber(int touchID)
{
	for(int i=0;i<MAX_KEYBOARD;++i){
		if(Key[i].id==touchID) return i;
	}
	return -1;
}

void LCD_CursorIndirect(int x, int y, int width, int color)
{
	LCD_DrawLineBuff(0,width, 0,0, width,0,color);
	LCD_DrawLineBuff(0,width, 0,1, width,1,color);
	LCD_DrawLineBuff(0,width, 0,2, width,2,color);
	LCD_DisplayBuff(4,x,y, width,3);
}

void LCD_DrawUpArrow(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor)
{
	uint16_t wRect=width/3;
	uint16_t hRect=height;
	LCD_DrawBoldUpTriangle(x,y, width, frameColor, fillColor, bkColor);
	LCD_DrawBoldRectangle(x+(width-wRect)/2, y+width/2, wRect,hRect, frameColor, fillColor, bkColor);
}

void LCD_SignX(int x, int y, int width, int color)
{
	SetPolygonPoint(x,       y, 0);
	SetPolygonPoint(x+width, y+width, 1);
	LCD_DrawPolygon(2,color);

	SetPolygonPoint(x+1,       y, 0);
	SetPolygonPoint(x+1+width, y+width, 1);
	LCD_DrawPolygon(2,color);

	SetPolygonPoint(x+width, y, 0);
	SetPolygonPoint(x,       y+width, 1);
	LCD_DrawPolygon(2,color);

	SetPolygonPoint(x+1+width, y, 0);
	SetPolygonPoint(x+1,       y+width, 1);
	LCD_DrawPolygon(2,color);
}
void LCD_SignBackspace(int x, int y, int width, int height, int color)
{
	int halfHeight=height/2;

	SetPolygonPoint(x, 				    y, 0);
	SetPolygonPoint(x+halfHeight,       y-halfHeight, 1);
	SetPolygonPoint(x+halfHeight+width, y-halfHeight, 2);
	SetPolygonPoint(x+halfHeight+width, y+halfHeight, 3);
	SetPolygonPoint(x+halfHeight,       y+halfHeight, 4);
	LCD_DrawPolygon(5,color);

	SetPolygonPoint(x+1, 				  y, 0);
	SetPolygonPoint(x+halfHeight,         y-halfHeight+1, 1);
	SetPolygonPoint(x+halfHeight+width-1, y-halfHeight+1, 2);
	SetPolygonPoint(x+halfHeight+width-1, y+halfHeight-1, 3);
	SetPolygonPoint(x+halfHeight,         y+halfHeight-1, 4);
	LCD_DrawPolygon(5,color);

	SetPolygonPoint(x+2,          y, 0);
	SetPolygonPoint(x+halfHeight, y-halfHeight+2, 1);
	LCD_DrawPolygon(2,color);

	SetPolygonPoint(x+2,          y, 0);
	SetPolygonPoint(x+halfHeight, y+halfHeight-2, 1);
	LCD_DrawPolygon(2,color);

	SetPolygonPoint(x+2,          y, 0);
	SetPolygonPoint(x+halfHeight, y+halfHeight-2, 1);
	LCD_DrawPolygon(2,color);

	int sizeX=height/3;
	LCD_SignX(x+halfHeight+(width-sizeX)/2-2, y-sizeX/2, sizeX,color);
}
static void LCD_NextAnotherSign(int x, int y, int width, int height, int color)
{
	int halfHeight=height/2;

	void _NextSign(int xPos)
	{
		LCD_DrawLine(x+xPos,            y-halfHeight,	x+halfHeight+xPos, y,	          color);
		LCD_DrawLine(x+halfHeight+xPos, y,	            x+xPos,            y+halfHeight,	 color);
	}

	_NextSign(0);
	_NextSign(1);
	_NextSign(2);
	_NextSign(width-halfHeight);
	_NextSign(width-halfHeight+1);
	_NextSign(width-halfHeight+2);
}
static void LCD_NextAnotherSign_Buff(int posBuff,int areaWidth, int x, int y, int width, int height, int color)
{
	int halfHeight=height/2;

	void _NextSign(int xPos)
	{
		LCD_DrawLineBuff(posBuff,areaWidth,  x+xPos,            y-halfHeight,	x+halfHeight+xPos, y,	          color);
		LCD_DrawLineBuff(posBuff,areaWidth,  x+halfHeight+xPos, y,	            x+xPos,            y+halfHeight,	 color);
	}

	_NextSign(0);
	_NextSign(1);
	_NextSign(2);
	_NextSign(width-halfHeight);
	_NextSign(width-halfHeight+1);
	_NextSign(width-halfHeight+2);
}

void LCD_SignX_Buff(int posBuff,int areaWidth, int x, int y, int width, int color)
{
	SetPolygonPoint(x,       y, 0);
	SetPolygonPoint(x+width, y+width, 1);
	LCD_DrawPolygonBuff(posBuff,areaWidth,2,color);

	SetPolygonPoint(x+1,       y, 0);
	SetPolygonPoint(x+1+width, y+width, 1);
	LCD_DrawPolygonBuff(posBuff,areaWidth,2,color);

	SetPolygonPoint(x+width, y, 0);
	SetPolygonPoint(x,       y+width, 1);
	LCD_DrawPolygonBuff(posBuff,areaWidth,2,color);

	SetPolygonPoint(x+1+width, y, 0);
	SetPolygonPoint(x+1,       y+width, 1);
	LCD_DrawPolygonBuff(posBuff,areaWidth,2,color);
}

static void LCD_SignSpace_Buff(int posBuff,int areaWidth, int x, int y, int width, int height, int color)
{
	LCD_DrawLineBuff(posBuff,areaWidth, x,y,        		x,      y+height, color);
	LCD_DrawLineBuff(posBuff,areaWidth, x,y+height, 		x+width,y+height, color);
	LCD_DrawLineBuff(posBuff,areaWidth, x+width,y+height, x+width,y,			color);

	LCD_DrawLineBuff(posBuff,areaWidth, x+1,      y,        	 x+1,      y+height-1, color);
	LCD_DrawLineBuff(posBuff,areaWidth, x+1,		 y+height-1, x+width-1,y+height-1, color);
	LCD_DrawLineBuff(posBuff,areaWidth, x+width-1,y+height-1, x+width-1,y,			  color);
}
static void LCD_SignSpace(int x, int y, int width, int height, int color)
{
	LCD_DrawLine(x,		y,			 x,      y+height, color);
	LCD_DrawLine(x,		y+height, x+width,y+height, color);
	LCD_DrawLine(x+width,y+height, x+width,y,			 color);

	LCD_DrawLine(x+1,		  y,			  x+1,      y+height-1, color);
	LCD_DrawLine(x+1,		  y+height-1, x+width-1,y+height-1, color);
	LCD_DrawLine(x+width-1,y+height-1, x+width-1,y,			   color);
}

void LCD_SignBackspace_Buff(int posBuff,int areaWidth, int x, int y, int width, int height, int color)
{
	int halfHeight=height/2;

	SetPolygonPoint(x, 				    y, 0);
	SetPolygonPoint(x+halfHeight,       y-halfHeight, 1);
	SetPolygonPoint(x+halfHeight+width, y-halfHeight, 2);
	SetPolygonPoint(x+halfHeight+width, y+halfHeight, 3);
	SetPolygonPoint(x+halfHeight,       y+halfHeight, 4);
	LCD_DrawPolygonBuff(posBuff,areaWidth,5,color);

	SetPolygonPoint(x+1, 				  y, 0);
	SetPolygonPoint(x+halfHeight,         y-halfHeight+1, 1);
	SetPolygonPoint(x+halfHeight+width-1, y-halfHeight+1, 2);
	SetPolygonPoint(x+halfHeight+width-1, y+halfHeight-1, 3);
	SetPolygonPoint(x+halfHeight,         y+halfHeight-1, 4);
	LCD_DrawPolygonBuff(posBuff,areaWidth,5,color);

	SetPolygonPoint(x+2,          y, 0);
	SetPolygonPoint(x+halfHeight, y-halfHeight+2, 1);
	LCD_DrawPolygonBuff(posBuff,areaWidth,2,color);

	SetPolygonPoint(x+2,          y, 0);
	SetPolygonPoint(x+halfHeight, y+halfHeight-2, 1);
	LCD_DrawPolygonBuff(posBuff,areaWidth,2,color);

	SetPolygonPoint(x+2,          y, 0);
	SetPolygonPoint(x+halfHeight, y+halfHeight-2, 1);
	LCD_DrawPolygonBuff(posBuff,areaWidth,2,color);

	int sizeX=height/3;
	LCD_SignX_Buff(0,areaWidth,x+halfHeight+(width-sizeX)/2-2, y-sizeX/2, sizeX,color);
}

void KeyBlock(uint16_t x,uint16_t y,uint16_t width,uint16_t height, int fontID,int pressFontID, uint32_t bkColor, frameShape pframeShape,char *txt,int touch)
{
	int16_t xFont=x+(width-ReturnWidthFontID(fontID,txt))/2;
	int16_t yFont=y+(height-ReturnHeightFontID(fontID))/2;
	uint32_t fillColor,frameColor,alternatFillColor,alternatFrameColor,pressFillColor,pressFrameColor;  int wArrow,hArrow;
	int widthPaint=0,heightPaint=0;

	        fillColor=0x707070;		        frameColor=0xD0D0D0;    //ColorIntensiv(BLUE,+100);
	alternatFillColor=0x505050;		alternatFrameColor=0xD0D0D0;
	   pressFillColor=0xFFFFFF;		   pressFrameColor=0x000000;

	switch(touch)
	{
	case TOUCH_capitalLetter:
		switch(KeyBoard.pressCapitalLetter)
		{
		default:
		case 0:
			pframeShape(x,y,width,height, alternatFrameColor,alternatFillColor,bkColor);
			wArrow=width/2-width/10;  // optymalizuj !!!!! nie powtarzaj to samo w case: !!!!
			hArrow=wArrow/3;
			LCD_DrawUpArrow(x+(width-wArrow)/2, y+(height-(wArrow/2+hArrow))/2, wArrow,hArrow, WHITE, WHITE, alternatFillColor);
			break;
		case 1:
			pframeShape(x,y,width,height, pressFrameColor,pressFillColor,bkColor);
			wArrow=width/2-width/10;
			hArrow=wArrow/3;
			LCD_DrawUpArrow(x+(width-wArrow)/2, y+(height-(wArrow/2+hArrow))/2, wArrow,hArrow, BLACK, BLACK, pressFillColor);
			break;
		}
		break;

	case TOUCH_anotherSigns:
		switch (KeyBoard.pressAnotherSigns)
		{
		case 0:
			pframeShape(x,y,width,height, alternatFrameColor,alternatFillColor,bkColor);
			TxtID_color(fontID, xFont, yFont, txt, fullHight,alternatFillColor,0,0);
			break;
		default:
		case 1:
			pframeShape(x,y,width,height, pressFrameColor,pressFillColor,bkColor);
			TxtID_color(pressFontID, xFont, yFont, txt, fullHight,pressFillColor,0,0);
			break;
		}
		break;

	case TOUCH_backspace:
		pframeShape(x,y,width,height, alternatFrameColor,alternatFillColor,bkColor);
		widthPaint=height/2, heightPaint=height/2-4;
		LCD_SignBackspace(x+(width-(widthPaint+heightPaint/2))/2-2, y+height/2, widthPaint,heightPaint, WHITE);
		break;

	case TOUCH_nextAnotherSigns:
		pframeShape(x,y,width,height, alternatFrameColor,alternatFillColor,bkColor);
		widthPaint=height/2-3, heightPaint=height/2-6;
		LCD_NextAnotherSign(x+(width-widthPaint)/2, y+height/2, widthPaint,heightPaint, WHITE);
		break;

	default:
		if(GetKeySign(touch-TOUCH_firstLetter)==' ')
		{
			pframeShape(x,y,width,height, frameColor,fillColor,bkColor);
			widthPaint=height, heightPaint=height/6;
			LCD_SignSpace(x+(width-widthPaint)/2, y+(height-heightPaint)/2, widthPaint,heightPaint, WHITE);
			break;
		}
		else
		{
			pframeShape(x,y,width,height, frameColor,fillColor,bkColor);
			TxtID_color(fontID, xFont, yFont, txt, fullHight,fillColor,0,0);
			break;
		}
	}

	if(touch>0)
	{
		int k= SearchFirstFreeKey();
		if(k!=-1)
		{
			Key[k].id=touch;
			Key[k].xBox=x;
			Key[k].yBox=y;
			Key[k].wBox=width;
			Key[k].hBox=height;
			Key[k].bkColor=bkColor;
			Key[k].frameColor=frameColor;
			Key[k].fillColor=fillColor;
			Key[k].pShape=pframeShape;
			Key[k].fontID=fontID;
			Key[k].pressFontID=pressFontID;
			Key[k].pressFrameColor=pressFrameColor;
			Key[k].pressFillColor=pressFillColor;
			Key[k].alternatFrameColor=alternatFrameColor;
			Key[k].alternatFillColor=alternatFillColor;

			switch(touch)
			{
			case TOUCH_backspace:
			case TOUCH_nextAnotherSigns:
				Key[k].xFont=widthPaint;
				Key[k].yFont=heightPaint;
				break;

			default:
				if(GetKeySign(touch-TOUCH_firstLetter)==' '){
					Key[k].xFont=widthPaint;
					Key[k].yFont=heightPaint;
				}
				else{
					Key[k].xFont=xFont-x;
					Key[k].yFont=yFont-y;
				}
				break;
			}

			StrBuffCopy(Key[k].txtBuff,txt);

			touchTemp.x_Start= x;
			touchTemp.y_Start= y;
			touchTemp.x_End= x+width;
			touchTemp.y_End= y+height;
			SetTouch(touch);
		}
	}
}

void LCD_Keyboard(int keyBoardID, frameShape pframeShape)
{
	uint16_t keyWidth=44, keyHeight=47, Vmargin=5, Hmargin=5, offset=10, offsetLettersDigits=30;
	int keyFont=fontID_14, pressKeyFont=fontID_13;  uint32_t bkColor=0xA0A0A0, frameColor=LIGHTGRAY;

	uint16_t xP=0,xP2=0,wP=0;  char bTxt[2]={0};  int i,itx=0;

	uint16_t heightKeyboard=offset+3*(keyHeight+Vmargin)+(keyHeight+offset);		uint16_t xPos=offset, yPos=LCD_GetYSize()-heightKeyboard-offset;

	pframeShape(xPos,yPos, LCD_GetXSize()-2*offset,heightKeyboard,frameColor,bkColor,DARKGRAY );

	xPos=2*offset;   yPos+=offset;
	for(i=0; i<10; ++i){
		bTxt[0]=GetKeySign(itx);
		KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		xPos+=(keyWidth+Hmargin);
	}

	xPos=2*offset+(keyWidth+Hmargin)/2;   yPos+=(keyHeight+Vmargin);
	for(i=0; i<9; ++i){
		bTxt[0]=GetKeySign(itx);
		KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		xPos+=(keyWidth+Hmargin);   if(i==0) xP=xPos;
	}

	xPos=2*offset;   yPos+=(keyHeight+Vmargin);    wP=xP-Vmargin-xPos;

	switch(KeyBoard.pressAnotherSigns)
	{
	case 0:
		KeyBlock(xPos,yPos, xP-Vmargin-xPos, keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"*",TOUCH_capitalLetter);
		break;
	default:
		KeyBlock(xPos,yPos, xP-Vmargin-xPos, keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,">>",TOUCH_nextAnotherSigns);
		break;
	}
	xPos=xP;


	for(i=0; i<7; ++i){
		bTxt[0]=GetKeySign(itx);
		KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		xPos+=(keyWidth+Hmargin);
	}
	xPos+=(Vmargin-Hmargin);
	KeyBlock(xPos,yPos,wP,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"*",TOUCH_backspace);  xP2=xPos+wP;

	xPos=2*offset;   yPos+=(keyHeight+Vmargin);
	KeyBlock(xPos,yPos,wP,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"@#%",TOUCH_anotherSigns);	       xPos+=(wP+Hmargin);
	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"*",0);  xPos+=(keyWidth+Hmargin);
	KeyBlock(xPos,yPos, xP2-xPos-wP-Hmargin-keyWidth-Hmargin, keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"space",TOUCH_firstLetter+itx++);  xPos+=(xP2-xPos-wP-Hmargin-keyWidth);
	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,",",TOUCH_firstLetter+itx++);  xPos+=(keyWidth+Hmargin);
	KeyBlock(xPos,yPos,wP,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,".",TOUCH_firstLetter+itx++);	       xPos+=(wP+Hmargin);

	xP=xPos+offsetLettersDigits;
	xPos=xP;   yPos=LCD_GetYSize()-heightKeyboard;

//	KeyBlock(xPos,yPos,keyWidth,2*keyHeight+Vmargin, keyFont,pressKeyFont,bkColor,pframeShape,"+",TOUCH_firstLetter+itx++);  //to wszystko nizej dac w for() !!!!
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"1",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"2",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"3",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	xPos=xP;   yPos+=(keyHeight+Vmargin);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"4",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"5",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"6",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	xPos=xP;   yPos+=(keyHeight+Vmargin);
//	KeyBlock(xPos,yPos,keyWidth,2*keyHeight+Vmargin, keyFont,pressKeyFont,bkColor,pframeShape,"-",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"7",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"8",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"9",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	xPos=xP;   yPos+=(keyHeight+Vmargin);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,".",TOUCH_firstLetter+itx++);
//	xPos+=(keyWidth+Hmargin);
//	KeyBlock(xPos,yPos,2*keyWidth+Hmargin,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,"0",TOUCH_firstLetter+itx++);


	for(i=0; i<4; ++i){
		bTxt[0]=GetKeySign(itx);
		if(i==0) KeyBlock(xPos,yPos,keyWidth,2*keyHeight+Vmargin, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		else     KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		xPos+=(keyWidth+Hmargin);
	}

	xPos=xP;   yPos+=(keyHeight+Vmargin);
	for(i=0; i<4; ++i){
		bTxt[0]=GetKeySign(itx);
		if(i>0) KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		xPos+=(keyWidth+Hmargin);
	}

	xPos=xP;   yPos+=(keyHeight+Vmargin);
	for(i=0; i<4; ++i){
		bTxt[0]=GetKeySign(itx);
		if(i==0) KeyBlock(xPos,yPos,keyWidth,2*keyHeight+Vmargin, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		else     KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		xPos+=(keyWidth+Hmargin);
	}

	xPos=xP;   yPos+=(keyHeight+Vmargin);
	for(i=0; i<4; ++i){
		bTxt[0]=GetKeySign(itx);
		if(i==1) KeyBlock(xPos,yPos,keyWidth,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		else if(i==2) KeyBlock(xPos,yPos,2*keyWidth+Hmargin,keyHeight, keyFont,pressKeyFont,bkColor,pframeShape,bTxt,TOUCH_firstLetter+itx++);
		xPos+=(keyWidth+Hmargin);
	}

	if(itx>=MAX_SiNGS_IN_KEYBOARD)
		return;

}

int KEYBOARD_GetEndPosX(int k)
{
	return Field[k].x + Field[k].width - ReturnWidthFontID(Field[k].font,"2");
}
int KEYBOARD_GetStartPosX(int k)
{
	return Field[k].x + ReturnWidthFontID(Field[k].font,"2");
}
int KEYBOARD_GetStartPosY(int k)
{
	return Field[k].y + 1+ReturnHeightFontID(Field[k].font)/6;
}
int KEYBOARD_GetPixelTxtPosX(int k, int *bufLineStart,int *TxtLenInLine)
{
	int i,j,stop=0,aaa;   char ctxt[2]={0,0};
	int endPosX = KEYBOARD_GetEndPosX(k);
	int startPosX;

	j=0; aaa=0;
	do
	{
		i=0;
		startPosX = KEYBOARD_GetStartPosX(k);
		do
		{
			ctxt[0]=KeyBoard.txt[aaa+i++];
			if(ctxt[0]==0){ stop=1; break; }
			startPosX += ReturnWidthFontID(Field[k].font,ctxt)+Field[k].space;

		}while(startPosX <= endPosX);
		if(stop==0)
		{
			bufLineStart[j]=aaa;
			TxtLenInLine[j++]=(i-1);
			aaa+=i-1; dbg("*");
		}

	}while(stop==0);

	if(stop==1)
	{
		bufLineStart[j]=aaa;
		TxtLenInLine[j++]=(i-1);
		aaa+=i-1;dbg("&");

//		startPosX += ReturnWidthFontID(Field[k].font,"2")+Field[k].space;
//		if(startPosX >= endPosX)
//		{
//			bufLineStart[j]=aaa;
//			TxtLenInLine[j++]=0; dbg("#");
//		}
	}
    return j;
}

void KEYBOARD_DisplayTxt(void)
{
	int i,j,s,bufA[10]={0},bufB[10]={0}, lines;	char bufTxt[200];
	int k=GetWhiteFieldNumber(TOUCH_whiteFieldKey);
	int endPosX = KEYBOARD_GetEndPosX(k);
	int startPosX = KEYBOARD_GetStartPosX(k);

	int y_temp = LCD_GetTxtYPos(Field[k].txtVar);
	LCD_SetTxtYPos(Field[k].txtVar, KEYBOARD_GetStartPosY(k) );

	lines=KEYBOARD_GetPixelTxtPosX(k,bufA,bufB);
	DbgSprintf(50,"\r\nXSSSSSSSS 1) %d %d    2) %d %d     %d ",bufA[0],bufB[0], bufA[1],bufB[1], lines);

	for(i=0;i<lines;i++)
	{
		for(j=0;j<bufB[i];j++) bufTxt[j]=KeyBoard.txt[ bufA[i]+j ];
		bufTxt[j]=0;

		s=0;
		do
		{   if(s>5) break;
			bufTxt[j+s]=' '; s++;
			bufTxt[j+s]=0;  dbg(".");
		}while(startPosX + ReturnWidthFontID(Field[k].font,bufTxt)+Field[k].space*(mini_strlen(bufTxt)-1) <= endPosX);

		LCD_Txt(Field[k].txtVar, bufTxt, fontNoChange, fullHight,Field[k].space);
		LCD_Txt_AddY(Field[k].txtVar,KeyBoard.pixelBetweenLines);

		if(i==lines-1)
		{
			if(KeyBoard.nextLine > lines-1)
			{
				bufTxt[0]=' ';
				bufTxt[1]=' ';
				bufTxt[2]=' ';
				bufTxt[3]=' ';
				bufTxt[4]=' '; bufTxt[5]=0;
				LCD_Txt(Field[k].txtVar, bufTxt, fontNoChange, fullHight,Field[k].space);
			}
		}
	}

	LCD_SetTxtYPos(Field[k].txtVar, y_temp );


}

static void KEYBOARD_CursorPosChange(int touchID, int x, int y, int lineY, int prevCursorColor);

void SCREEN_KeyboardSet(int screenID, int fontSize,int resumeScreen)  //key
{
	LCD_Clear(0x000000);   int y_temp;
	LCD_DrawThemeFrame(1,0, MYLIGHTGRAY,DARKGRAY);
	DeleteAllTouch();
	KeyBoard.pixelBetweenLines=50;

	if(resumeScreen==0)
	{
		ClearKeyBoardStruct();
		LCD_DeleteAllFontAndImages();
		LCD_LoadFont(FONT_12_bold,Arial,DARKGRAY,WHITE, FONT_MAIN_MENU,0);
		LCD_LoadFont(FONT_18,Arial,WHITE,BLACK, FontWhiteField,0);
		LCD_LoadFont(FONT_12_bold, Arial, DARKGRAY, GREEN, FontDescription,0);
		LCD_LoadFont(FONT_16_bold,Arial,DARKGRAY,WHITE, fontID_14,0);
		LCD_LoadFont(FONT_16_bold,Arial,WHITE,BLACK, fontID_13,0);

		StrBuffCopylimit(KeyBoard.txt, "Krasnoludki lubia budki XXXXA 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789",mini_strlen("Krasnoludki lubia budki XXXXA 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789"));
		KeyBoard.lenTxt=mini_strlen(KeyBoard.txt);
	}
	else
	{
		y_temp = LCD_GetTxtYPos(Field[GetWhiteFieldNumber(TOUCH_whiteFieldKey)].txtVar);

	}

DbgVar(1,50,"\r\nXXXXXXXXXXXXXXXXX111111111111  : %d    ",KeyBoard.cursorPixelPosY);



//	ClearAllWhiteField();
//	if(KeyBoard.nextLine>0) StrBuffCopylimit(&bufTemp[300],KeyBoard.txt,KeyBoard.itxNextLine);
//	else 						 StrBuffCopy(&bufTemp[300],KeyBoard.txt);
//
//	LCD_DescrWhiteField(1,5,15,10,60, LCD_GetXSize()-20,FontDescription,GetSelTxt(0,Descr_ActWifi,0) ,FontWhiteField,&bufTemp[300], TOUCH_whiteFieldKey,0,1);
//
//	if(KeyBoard.nextLine>0){
//		LCD_Txt_AddY(Field[GetWhiteFieldNumber(TOUCH_whiteFieldKey)].txtVar,KeyBoard.pixelBetweenLines);
//		LCD_WhiteField(1,FontWhiteField, 10,60+KeyBoard.pixelBetweenLines, LCD_GetXSize()-20,&KeyBoard.txt[KeyBoard.itxNextLine],TOUCH_whiteFieldKey+KeyBoard.nextLine,0,0);
//	}


	int bufA[10]={0},bufB[10]={0}, lines,i;

	ClearAllWhiteField();
	LCD_DescrWhiteField(1,15,10,60,5, LCD_GetXSize()-20,FontDescription,GetSelTxt(0,WhiteFieldsTXT,0) ,FontWhiteField,"Tuuuuuuu !!!!! Twoj Text", TOUCH_whiteFieldKey,0,1);

	lines=KEYBOARD_GetPixelTxtPosX(GetWhiteFieldNumber(TOUCH_whiteFieldKey),bufA,bufB);
	if(lines>1)
	{
		for(i=0;i<lines;i++)
		{
			StrBuffCopylimit(bufTemp, &KeyBoard.txt[bufA[i]], bufB[i]);
			if(i>0) LCD_Txt_AddY(Field[GetWhiteFieldNumber(TOUCH_whiteFieldKey)].txtVar,KeyBoard.pixelBetweenLines);
			if(i==0) LCD_WhiteField(1,FontWhiteField, 10,60+i*KeyBoard.pixelBetweenLines, LCD_GetXSize()-20,bufTemp,0,0,0);   //3-ci od konca noTouch!!!
			else     LCD_WhiteField(1,FontWhiteField, 10,60+i*KeyBoard.pixelBetweenLines, LCD_GetXSize()-20,bufTemp,TOUCH_whiteFieldKey+i,0,0);
		}
	}
	else
	{
		StrBuffCopylimit(bufTemp, &KeyBoard.txt[bufA[0]], bufB[0]);
		LCD_WhiteField(1,FontWhiteField, 10,60, LCD_GetXSize()-20,bufTemp,TOUCH_whiteFieldKey,0,0);
	}


	ClearAllButton();
	LCD_Button(0,FONT_MAIN_MENU,FontDescription,700,0,100,"BACK",MYLIGHTGRAY,DARKGRAY,DARKGRAY,TOUCH_back,0);

	ClearAllKey();
	LCD_Keyboard(0,LCD_DrawRoundRectangle3);


	LCD_DisplayBuff(0,(uint32_t)0,(uint32_t)0,(uint32_t)LCD_GetXSize(),(uint32_t)LCD_GetYSize());



	if(resumeScreen==0)
	{
		int k=GetWhiteFieldNumber(TOUCH_whiteFieldKey);
		if(k!=-1)
		{
//			KeyBoard.cursorPixelPosX=Field[k].x + ReturnWidthFontID(Field[k].font,"2");
//			KeyBoard.cursorPixelPosY=LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);
//			KeyBoard.cursorPixelWidth=11;

			DbgVar(1,50,"\r\nXXXXXXXXXXXXXXXXX2222222222222222222  : %d    ",KeyBoard.cursorPixelPosY);

			KEYBOARD_CursorPosChange(TOUCH_whiteFieldKey, KEYBOARD_GetEndPosX(k), Field[k].y,  KEYBOARD_GetTxtLines(k)-1,WHITE);
			KeyBoard.nextLine=KEYBOARD_GetTxtLines(k)-1;
			//LCD_SetTxtYPos(Field[k].txtVar, KEYBOARD_GetStartPosY(k)+KeyBoard.nextLine*KeyBoard.pixelBetweenLines );

			//KEYBOARD_DisplayTxt();
		}


	}
	else
	{
		int k=GetWhiteFieldNumber(TOUCH_whiteFieldKey);
				if(k!=-1)
				{
					DbgVar(1,50,"\r\nXXXXXXXXXXXXXXXXX333333333333333333  : %d    ",KeyBoard.cursorPixelPosY);


					LCD_SetTxtYPos(Field[GetWhiteFieldNumber(TOUCH_whiteFieldKey)].txtVar, y_temp );
					KEYBOARD_DisplayTxt();
				}
	}




	//KEYBOARD_DisplayTxt();
//
//	BSP_LCD_FillCircle(100,300,30);    BSP_LCD_DrawLine(100,400,180,410);
//	BSP_LCD_DrawEllipse(400,400,30,60);
//	BSP_LCD_FillEllipse(410,410,20,40);
//	BSP_LCD_FillPolygon(5);
//	BSP_LCD_DrawPolygon(5);




}

void TOUCH_KeyID(int KeyID, int touchID)
{
  int k=GetKeyNumber(touchID);
  if(k!=-1)
  {
	uint32_t frameColor, fillColor;
	uint16_t font;

	switch(KeyID)
	{
	case PRESSED:
	default:
		frameColor=Key[k].pressFrameColor;   fillColor=Key[k].pressFillColor;	font=Key[k].pressFontID;
		break;

	case RELEASED:
		switch(touchID)
		{
		case TOUCH_backspace:
		case TOUCH_nextAnotherSigns:
			frameColor=	Key[k].alternatFrameColor;
			fillColor=	Key[k].alternatFillColor;
			font=			Key[k].fontID;
			break;

		default:
			frameColor=	Key[k].frameColor;
			fillColor=	Key[k].fillColor;
			font=			Key[k].fontID;
			break;
		}
		break;
	}
	LCD_DrawRectangleBuff(0,Key[k].wBox,Key[k].hBox,Key[k].bkColor,Key[k].bkColor,Key[k].bkColor); //t≥o

	     if(Key[k].pShape==LCD_DrawRectangle)			          LCD_DrawRectangleBuff(0,Key[k].wBox,Key[k].hBox,frameColor,fillColor,Key[k].bkColor);
	else if(Key[k].pShape==LCD_DrawRoundRectangle1)		    LCD_DrawRoundRectangle1Buff(0,Key[k].wBox,Key[k].hBox,frameColor,fillColor,Key[k].bkColor);
	else if(Key[k].pShape==LCD_DrawRoundRectangle2)		    LCD_DrawRoundRectangle3Buff(0,Key[k].wBox,Key[k].hBox,frameColor,fillColor,Key[k].bkColor);
	else if(Key[k].pShape==LCD_DrawRoundRectangle3)		    LCD_DrawRoundRectangle3Buff(0,Key[k].wBox,Key[k].hBox,frameColor,fillColor,Key[k].bkColor);
	else if(Key[k].pShape==LCD_DrawBoldRoundRectangle1)	LCD_DrawBoldRoundRectangle1Buff(0,Key[k].wBox,Key[k].hBox,frameColor,fillColor,Key[k].bkColor);
	else if(Key[k].pShape==LCD_DrawBoldRoundRectangle2)	LCD_DrawBoldRoundRectangle3Buff(0,Key[k].wBox,Key[k].hBox,frameColor,fillColor,Key[k].bkColor);
	else if(Key[k].pShape==LCD_DrawBoldRoundRectangle3)	LCD_DrawBoldRoundRectangle3Buff(0,Key[k].wBox,Key[k].hBox,frameColor,fillColor,Key[k].bkColor);

	switch(touchID)
	{
	case TOUCH_backspace:
		LCD_SignBackspace_Buff(0,Key[k].wBox,    (Key[k].wBox-(Key[k].xFont+Key[k].yFont/2))/2-2,  Key[k].hBox/2,    Key[k].xFont,Key[k].yFont, KeyID==PRESSED ? BLACK:WHITE);
		break;

	case TOUCH_nextAnotherSigns:
		LCD_NextAnotherSign_Buff(0,Key[k].wBox,    (Key[k].wBox-Key[k].xFont)/2,  Key[k].hBox/2,    Key[k].xFont,Key[k].yFont, KeyID==PRESSED ? BLACK:WHITE);
		break;

	default:
		if(GetKeySign(touchID-TOUCH_firstLetter)==' ')
			LCD_SignSpace_Buff(0,Key[k].wBox,    (Key[k].wBox-Key[k].xFont)/2,  (Key[k].hBox-Key[k].yFont)/2,    Key[k].xFont,Key[k].yFont, KeyID==PRESSED ? BLACK:WHITE);
		else
			TxtID_color_BufferWindow(0,font, Key[k].xFont,Key[k].yFont, Key[k].txtBuff, fullHight, Key[k].wBox,fillColor,0);
		break;
	}

	LCD_DisplayBuff(4,Key[k].xBox,Key[k].yBox, Key[k].wBox, Key[k].hBox);
  }
}

int KEYBOARD_GetTxtLines(int k)
{
	int i,j,stop=0,aaa;  char ctxt[2]={0,0};
	int endPosX = KEYBOARD_GetEndPosX(k);
	int startPosX;

	j=0; aaa=0;
	do
	{
		i=0;
		startPosX = KEYBOARD_GetStartPosX(k);
		do
		{
			ctxt[0]=KeyBoard.txt[aaa+i++];
			if(ctxt[0]==0){ stop=1; break; }
			startPosX += ReturnWidthFontID(Field[k].font,ctxt)+Field[k].space;

		}while(startPosX <= endPosX);
		if(stop==0)
		{
			j++;
			aaa+=i-1;
		}

	}while(stop==0);

	if(stop==1)
	{
		j++;
		aaa+=i-1;

//		startPosX += ReturnWidthFontID(Field[k].font,"2")+Field[k].space;
//		if(startPosX >= endPosX)
//		{
//			j++;
//		}
	}
    return j;
}

static void KEYBOARD_CursorPosChange(int touchID, int x, int y, int lineY, int prevCursorColor)
{
	int bufA[10]={0},bufB[10]={0}, lines;  char bufTxt[200];  int j;
	int k=GetWhiteFieldNumber(touchID);
	if(k!=-1)
	{
		lines=KEYBOARD_GetPixelTxtPosX(k,bufA,bufB);
		DbgSprintf(50,"\r\nXSSSSSSSSAAAA 1) %d %d    2) %d %d     %d ",bufA[0],bufB[0], bufA[1],bufB[1], lines);

		for(j=0;j<bufB[lineY];j++)
			bufTxt[j]=KeyBoard.txt[ bufA[lineY]+j ];
		bufTxt[j]=0;

		int txtPixelLen= ReturnWidthFontID(Field[k].font,"2")+ReturnWidthFontID(Field[k].font,bufTxt)+Field[k].space*(mini_strlen(bufTxt)-1);

		DbgVar(1,50,"\r\nTXT: %s -> %d -> %d ",bufTxt,txtPixelLen,Field[k].width);

		if(txtPixelLen < Field[k].width)
		{
		   LCD_CursorIndirect(KeyBoard.cursorPixelPosX, KeyBoard.cursorPixelPosY, KeyBoard.cursorPixelWidth, prevCursorColor);
			if(x < Field[k].x+txtPixelLen)
		   {
			   dbg("\r\n555555 ");
			   int posX = Field[k].x + ReturnWidthFontID(Field[k].font,"2");
			   int i=0;  char ctxt[2]; ctxt[1]=0;
			   do
			   {
				   ctxt[0]=bufTxt[i]; i++;
				   if(ctxt[0]==0) break;
				   posX += ReturnWidthFontID(Field[k].font,ctxt)+Field[k].space;

			   }while(posX < x);
			   posX -= ReturnWidthFontID(Field[k].font,ctxt);

				LCD_SetTxtYPos(Field[k].txtVar, KEYBOARD_GetStartPosY(k)+lineY*KeyBoard.pixelBetweenLines );  //tego nie trzeba "KeyBoard.pixelBetweenLines" jezeli bedziemy miec::  KEYBOARD_CursorPosChange(x,y,ID_touchDetect-TOUCH_whiteFieldKey); ->  ( GetWhiteFieldNumber(ID_touchDetect-TOUCH_whiteFieldKey)  )
			   KeyBoard.cursorPixelPosX	= posX;
				KeyBoard.cursorPixelPosY	= LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);
			   KeyBoard.cursorTxtPos		= bufA[lineY] + i-1;
			   KeyBoard.cursorPixelWidth	= ReturnWidthFontID(Field[k].font,ctxt);
		   }
		   else
		   {
			   if(lineY==lines-1)  //jezeli dotyk x biedzie dalszy niø koniec tekstu dla ostatniej linii to ustawa na kocowy znak tekstu
			   {
			    	dbg("\r\n6666666 ");
					LCD_SetTxtYPos(Field[k].txtVar, KEYBOARD_GetStartPosY(k)+lineY*KeyBoard.pixelBetweenLines );    //tego nie trzeba "KeyBoard.pixelBetweenLines" jezeli bedziemy miec::  KEYBOARD_CursorPosChange(x,y,ID_touchDetect-TOUCH_whiteFieldKey); ->  ( GetWhiteFieldNumber(ID_touchDetect-TOUCH_whiteFieldKey)  )
			    	KeyBoard.cursorPixelPosX	= LCD_GetTxtXPos(Field[k].txtVar)+ReturnWidthFontID(Field[k].font,bufTxt)+Field[k].space*(mini_strlen(bufTxt)-1);
					KeyBoard.cursorPixelPosY	= LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);
			    	KeyBoard.cursorTxtPos		= KeyBoard.lenTxt;
			    	KeyBoard.cursorPixelWidth	= 11;
			   }
		   }
		}
	}
}

//void KEYBOARD_CursorPosChange(int x, int y, int lineY)
//{
//	int bufA[10]={0},bufB[10]={0}, lines;  char bufTxt[200];  int j;
//	int k=GetWhiteFieldNumber(TOUCH_whiteFieldKey+lineY);
//	if(k>-1)
//	{
//		lines=KEYBOARD_GetPixelTxtPosX(k,bufA,bufB);
//		DbgSprintf(50,"\r\nXSSSSSSSSAAAA 1) %d %d    2) %d %d     %d ",bufA[0],bufB[0], bufA[1],bufB[1], lines);
//
//		for(j=0;j<bufB[lineY];j++) bufTxt[j]=KeyBoard.txt[ bufA[lineY]+j ];
//		bufTxt[j]=0;
//
//		int txtPixelLen= ReturnWidthFontID(Field[k].font,"2")+ReturnWidthFontID(Field[k].font,bufTxt)+Field[k].space*(mini_strlen(bufTxt)-1);
//		if(txtPixelLen < Field[k].width)
//		{
//		   if(x < Field[k].x+txtPixelLen)
//		   {
//			   dbg("\r\n555555 ");
//			   int posX = Field[k].x + ReturnWidthFontID(Field[k].font,"2");
//			   int i=0;  char ctxt[2]; ctxt[1]=0;
//			   do
//			   {
//				   ctxt[0]=bufTxt[i]; i++;
//				   if(ctxt[0]==0) break;
//				   posX += ReturnWidthFontID(Field[k].font,ctxt)+Field[k].space;
//
//			   }while(posX < x);
//			   posX -= ReturnWidthFontID(Field[k].font,ctxt);
//
//				LCD_SetTxtYPos(Field[k].txtVar, KEYBOARD_GetStartPosY(k) );
//			   KeyBoard.cursorPixelPosX	= posX;
//				KeyBoard.cursorPixelPosY	= LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);
//			   KeyBoard.cursorTxtPos		= bufA[lineY] + i-1;
//			   KeyBoard.cursorPixelWidth	= ReturnWidthFontID(Field[k].font,ctxt);
//			   LCD_CursorIndirect(KeyBoard.cursorPixelPosX, KeyBoard.cursorPixelPosY, KeyBoard.cursorPixelWidth, WHITE);
//		   }
//		   else
//		   {
//			   if(lineY==lines-1)
//			   {
//			    	dbg("\r\n6666666 ");
//					LCD_SetTxtYPos(Field[k].txtVar, KEYBOARD_GetStartPosY(k) );
//			    	KeyBoard.cursorPixelPosX	= LCD_GetTxtXPos(Field[k].txtVar)+ReturnWidthFontID(Field[k].font,bufTxt)+Field[k].space*(mini_strlen(bufTxt)-1);
//					KeyBoard.cursorPixelPosY	= LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);
//			    	KeyBoard.cursorTxtPos		= KeyBoard.lenTxt;
//			    	KeyBoard.cursorPixelWidth	= 11;
//			    	LCD_CursorIndirect(KeyBoard.cursorPixelPosX, KeyBoard.cursorPixelPosY, KeyBoard.cursorPixelWidth, WHITE);
//			   }
//		   }
//		}
//	}
//}

extern int ID_touchDetect; // <- //ZLIKWIDUJ TO !!!!
int KEYBOARD_SetChar(int touchID)
{
  int i;   int bufA[10]={0},bufB[10]={0};  ///ROZMIAR Z DEFINE !!!!
  char buf[2]={0,0};   char buf_p[SIZE_TXT];     static uint16_t cursorPixelPosX_p=0;  static char letter[2]={0,0};
  char letterToCancel[2]={0,0};
  int k=GetWhiteFieldNumber(TOUCH_whiteFieldKey);
  if(k<0) return 0;

  int startPosX = KEYBOARD_GetStartPosX(k);   // to samo co LCD_GetTxtXPos(Field[k].txtVar !!!!!
  int endPosX = KEYBOARD_GetEndPosX(k);

  LCD_CursorIndirect(KeyBoard.cursorPixelPosX, KeyBoard.cursorPixelPosY, KeyBoard.cursorPixelWidth, WHITE);
  dbg("\r\nQQQQQQQQQQQQQQ 1  ");   KeyBoard.cursorPixelPosY=LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);

  if(TOUCH_backspace==ID_touchDetect)
  {
	 if(KeyBoard.lenTxt > 0)
	 {
		if(KeyBoard.cursorTxtPos < KeyBoard.lenTxt)
		{
			if(KeyBoard.cursorTxtPos==0) return 0;
			dbg("\r\n111111 ");
			KeyBoard.cursorTxtPos--;
			letterToCancel[0]=KeyBoard.txt[KeyBoard.cursorTxtPos];
			for(i=KeyBoard.cursorTxtPos; i<KeyBoard.lenTxt; ++i) KeyBoard.txt[i] =KeyBoard.txt[i+1];
			KeyBoard.lenTxt--;
			KeyBoard.txt[KeyBoard.lenTxt]=0;
			//KeyBoard.cursorPixelPosX -= ReturnWidthFontID(Field[k].font,letterToCancel)+Field[k].space;


			if(cursorPixelPosX_p>0)
			{
						if(KeyBoard.cursorPixelPosX==startPosX)
						{
							cursorPixelPosX_p -= ReturnWidthFontID(Field[k].font,letterToCancel)+Field[k].space;
							if(cursorPixelPosX_p  <= endPosX-(ReturnWidthFontID(Field[k].font,letter)+Field[k].space))
							{
								cursorPixelPosX_p=0;    DbgSprintf(50,"\r\n111 %s    ",letter);

//								for(i=0; i<KeyBoard.cursorTxtPos; ++i) buf_p[i]=KeyBoard.txt[i];
//								buf_p[i]=0;
								KEYBOARD_GetPixelTxtPosX(GetWhiteFieldNumber(TOUCH_whiteFieldKey),bufA,bufB);
								i=KeyBoard.nextLine-1;
								StrBuffCopylimit(buf_p, &KeyBoard.txt[bufA[i]], KeyBoard.cursorTxtPos-bufA[i]);  //bufTemp zamienic na lokalne w funkcji buf !!!!!

								KeyBoard.cursorPixelPosX= startPosX + ReturnWidthFontID(Field[k].font,buf_p) + Field[k].space*(mini_strlen(buf_p)-1);
								if(buf_p[0]>0) LCD_Txt_SubY(Field[k].txtVar,KeyBoard.pixelBetweenLines);
								dbg("\r\nQQQQQQQQQQQQQQ 2  "); KeyBoard.cursorPixelPosY=LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);

							}
							else{  dbg("\r\nAAAAAAAAAAAAA");  }
						}
						else{  cursorPixelPosX_p=0;    dbg("\r\n222");
						       KeyBoard.cursorPixelPosX -= ReturnWidthFontID(Field[k].font,letterToCancel)+Field[k].space;   }
			}
			else{    dbg("\r\n3333");
				KeyBoard.cursorPixelPosX -= ReturnWidthFontID(Field[k].font,letterToCancel)+Field[k].space;   }

			if(KEYBOARD_GetTxtLines(k)-1 < KeyBoard.nextLine)
			{
				dbg("gggggg ");


				DeleteSelectTouch(TOUCH_whiteFieldKey+KeyBoard.nextLine);
				TOUCH_WhiteFieldID(3,TOUCH_whiteFieldKey);
				KeyBoard.nextLine--;


				if(KeyBoard.cursorPixelPosX < startPosX)
				{
					if(cursorPixelPosX_p==0)
					{
						KEYBOARD_CursorPosChange(TOUCH_whiteFieldKey, endPosX-Field[k].space-ReturnWidthFontID(Field[k].font,"2"), Field[k].y, KeyBoard.nextLine,DARKGRAY);

					}
					else
					{

					}
				}
				else
				{

				}





			}
			if(KeyBoard.cursorPixelPosX < startPosX)
			{
				dbg("\r\nhhhhh");
//				for(i=0; i<KeyBoard.cursorTxtPos; ++i) buf_p[i]=KeyBoard.txt[i];
//				buf_p[i]=0;
				KEYBOARD_GetPixelTxtPosX(GetWhiteFieldNumber(TOUCH_whiteFieldKey),bufA,bufB);
				i=KeyBoard.nextLine-1;
				StrBuffCopylimit(buf_p, &KeyBoard.txt[bufA[i]], KeyBoard.cursorTxtPos-bufA[i]);  //bufTemp zamienic na lokalne w funkcji buf !!!!!

				if(cursorPixelPosX_p==0)
				{
					dbg("\r\nGGGGGGGGGG");
					cursorPixelPosX_p= startPosX + ReturnWidthFontID(Field[k].font,buf_p) + Field[k].space*(mini_strlen(buf_p)-1);

					buf[0]=KeyBoard.txt[KeyBoard.cursorTxtPos];
					letter[0]=buf[0];  DbgSprintf(50," %s    ",buf);
					KeyBoard.cursorPixelPosX=startPosX;
				}
				else
				{
					KeyBoard.cursorPixelPosX= startPosX + ReturnWidthFontID(Field[k].font,buf_p) + Field[k].space*(mini_strlen(buf_p)-1);
					if(buf_p[0]>0) LCD_Txt_SubY(Field[k].txtVar,KeyBoard.pixelBetweenLines);
					dbg("\r\nQQQQQQQQQQQQQQ 3  "); KeyBoard.cursorPixelPosY=LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);

					buf[0]=KeyBoard.txt[KeyBoard.cursorTxtPos-1];
					KeyBoard.cursorPixelWidth=ReturnWidthFontID(Field[k].font,buf);
				}
			}
			else
			{
				buf[0]=KeyBoard.txt[KeyBoard.cursorTxtPos];
				KeyBoard.cursorPixelWidth=ReturnWidthFontID(Field[k].font,buf);
			}

		}
		else
		{
			dbg("\r\n222222 ");
			KeyBoard.lenTxt--;
			KeyBoard.cursorTxtPos=KeyBoard.lenTxt;
			letterToCancel[0]=KeyBoard.txt[KeyBoard.lenTxt];
			KeyBoard.txt[KeyBoard.lenTxt]=0;
			KeyBoard.cursorPixelPosX -= ReturnWidthFontID(Field[k].font,letterToCancel)+Field[k].space;

			if(KeyBoard.cursorPixelPosX < startPosX)
			{
				KEYBOARD_GetPixelTxtPosX(GetWhiteFieldNumber(TOUCH_whiteFieldKey),bufA,bufB);
				i=KeyBoard.nextLine-1;
				StrBuffCopylimit(bufTemp, &KeyBoard.txt[bufA[i]], bufB[i]);  //bufTemp zamienic na lokalne w funkcji buf !!!!!

				KeyBoard.cursorPixelPosX= startPosX + ReturnWidthFontID(Field[k].font,bufTemp) + Field[k].space*(mini_strlen(bufTemp)-1);
				if(KeyBoard.txt[0]>0) LCD_Txt_SubY(Field[k].txtVar,KeyBoard.pixelBetweenLines);
				dbg("\r\nQQQQQQQQQQQQQQ 4  "); KeyBoard.cursorPixelPosY=LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);
				if(KeyBoard.nextLine>0)
				{
					DeleteSelectTouch(TOUCH_whiteFieldKey+KeyBoard.nextLine);
					TOUCH_WhiteFieldID(3,TOUCH_whiteFieldKey);
					KeyBoard.nextLine--;
				}
			}
			KeyBoard.cursorPixelWidth=11;
		}
		return 1;
	 }
	 else return 0;
  }
  else
  {
	if(KeyBoard.lenTxt < SIZE_TXT-1)
	{
		//DbgSprintf(50,"\r\nJ: %d %d  %d ",KeyBoard.lenTxt,SIZE_TXT-1,sizeof(KeyBoard.txt));
		//buf[0]='a'+ (touchID-TOUCH_firstLetter);
		buf[0]=GetKeySign(touchID-TOUCH_firstLetter);
		if(KeyBoard.cursorTxtPos < KeyBoard.lenTxt)
		{
			dbg("33333 ");
			for(i=KeyBoard.lenTxt-1; i>=KeyBoard.cursorTxtPos; --i) KeyBoard.txt[i+1]=KeyBoard.txt[i];
			KeyBoard.txt[KeyBoard.lenTxt+1]=0;
			KeyBoard.lenTxt++;
			KeyBoard.txt[KeyBoard.cursorTxtPos]=buf[0];





if(cursorPixelPosX_p>0)
{
			if(KeyBoard.cursorPixelPosX==startPosX)
			{
				cursorPixelPosX_p += ReturnWidthFontID(Field[k].font,buf)+Field[k].space;
				if(cursorPixelPosX_p  <= endPosX)
				{


				}
				else{  cursorPixelPosX_p=0;
					KeyBoard.cursorPixelPosX += ReturnWidthFontID(Field[k].font,buf)+Field[k].space;  }
			}
			else{  cursorPixelPosX_p=0;
				KeyBoard.cursorPixelPosX += ReturnWidthFontID(Field[k].font,buf)+Field[k].space;   }
}
else
	KeyBoard.cursorPixelPosX += ReturnWidthFontID(Field[k].font,buf)+Field[k].space;




			if(KEYBOARD_GetTxtLines(k)-1 > KeyBoard.nextLine)
			{
				dbg("gggggg ");
				KeyBoard.nextLine++;
				TOUCH_WhiteFieldID(2,TOUCH_whiteFieldKey);

				if(1==GetTouchToTemp(TOUCH_whiteFieldKey))
				{
					int diff = touchTemp.y_End-touchTemp.y_Start;
					touchTemp.y_Start= Field[k].y + KeyBoard.nextLine*KeyBoard.pixelBetweenLines;
					touchTemp.y_End= touchTemp.y_Start+diff;
					if(SetTouch(TOUCH_whiteFieldKey+KeyBoard.nextLine)>0) dbg("\r\n iiiiiiiii ");
				}
			}
			KeyBoard.cursorTxtPos++;
			buf[0]=KeyBoard.txt[KeyBoard.cursorTxtPos];
			KeyBoard.cursorPixelWidth=ReturnWidthFontID(Field[k].font,buf);

			if(KeyBoard.cursorPixelPosX + ReturnWidthFontID(Field[k].font,buf)+Field[k].space > endPosX)
			{
				dbg("aaaaa ");
				cursorPixelPosX_p = KeyBoard.cursorPixelPosX; //!!!!!!!!!!!!!!!!!!!!!!!
				letter[0]=buf[0];
				KeyBoard.cursorPixelPosX=startPosX;
				LCD_Txt_AddY(Field[k].txtVar,KeyBoard.pixelBetweenLines);
				dbg("\r\nQQQQQQQQQQQQQQ 5  ");  KeyBoard.cursorPixelPosY=LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);
			}
		}
		else
		{
			dbg("444444-- ");
			KeyBoard.txt[KeyBoard.lenTxt++]=buf[0];     KeyBoard.txt[KeyBoard.lenTxt]=0;
			KeyBoard.cursorTxtPos++;
			KeyBoard.cursorPixelPosX += ReturnWidthFontID(Field[k].font,buf)+Field[k].space;
			if(KeyBoard.cursorPixelPosX > endPosX)
			{
				dbg("aaaaa12 ");
				KeyBoard.cursorPixelPosX= startPosX+ReturnWidthFontID(Field[k].font,buf)+Field[k].space;
				LCD_Txt_AddY(Field[k].txtVar,KeyBoard.pixelBetweenLines);
				dbg("\r\nQQQQQQQQQQQQQQ 6  ");  KeyBoard.cursorPixelPosY=LCD_GetTxtYPos(Field[k].txtVar)+ReturnHeightFontID(Field[k].font);
				KeyBoard.nextLine++;
				TOUCH_WhiteFieldID(2,TOUCH_whiteFieldKey);

				if(1==GetTouchToTemp(TOUCH_whiteFieldKey))
				{
					int diff = touchTemp.y_End-touchTemp.y_Start;
					touchTemp.y_Start= Field[k].y + KeyBoard.nextLine*KeyBoard.pixelBetweenLines;
					touchTemp.y_End= touchTemp.y_Start+diff;
					if(SetTouch(TOUCH_whiteFieldKey+KeyBoard.nextLine)>0) dbg("\r\n iiiiiiiii ");
				}
			}
			KeyBoard.cursorPixelWidth=11;
		}
		return 1;
	}
	else return 0;
  }
}

extern int touchPressed;

static bool IsVarNotSet(uint8_t *pVar)
{
	if(*pVar==0)
	{
		*pVar=1;
		return true;
	}
	else
		return false;
}
static void VarReset(uint8_t *pVar)
{
	*pVar=0;
}

static bool IsBitNotSet(uint64_t *pVar, uint8_t bit)
{
	if(((*pVar>>bit)&0x1)==0)
	{
		*pVar|=((uint64_t)1<<bit);
		return true;
	}
	else
		return false;
}
static void ResetAllBitsWithout(uint64_t *pVar, uint8_t bit)
{
	if(((*pVar>>bit)&0x1)==0)
		*pVar=0;
	else
	{
		*pVar=0;
		*pVar|=((uint64_t)1<<bit);
	}
}
static void AllBitsReset(uint64_t *pVar)
{
	*pVar=0;
}
static int IsAnySetBit(uint64_t *pVar)
{
	for(int i=0;i<64;i++)
	{
		if((*pVar>>i)&0x1)
		{
			*pVar &= ~(((uint64_t)1)<<i);
			return i;
		}
	}
	return -1;
}

void IncScrollVal(uint8_t *val,int incTo)  // przeniesc do utility !!!!
{
	uint8_t val_p=*val;
	val_p++;
	if(val_p>incTo) val_p=0;
	*val=val_p;
}

void TOUCH_KeyboardSet(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y)
{
	static uint8_t onePress=0;    static uint8_t touchPrevious=0;
	static portTickType holdPressTime;
	static int HOLD_PRESS_TIME=700;

	switch (pressRelease)
	{
	case PRESSED:

		switch(ID_touchDetect)
		{
		case TOUCH_capitalLetter:
			if(IsVarNotSet(&onePress))
			{
				IncScrollVal(&KeyBoard.pressCapitalLetter,1);
				SCREEN_Load(SCREEN_Keyboard,FONT_28,1);
			}
		 	break;

		case TOUCH_anotherSigns:
			if(IsVarNotSet(&onePress))
			{
				if(KeyBoard.pressAnotherSigns==0)
					KeyBoard.pressAnotherSigns=1;
				else
					KeyBoard.pressAnotherSigns=0;
				SCREEN_Load(SCREEN_Keyboard,FONT_28,1);
			}
		 	break;

		case TOUCH_back:
			dbg("\r\nBACK ");
			TOUCH_ButtonID(0,ID_touchDetect);
			DbgVar(1,500,"\r\n\r\nTXT: %s\r\n\r\n",KeyBoard.txt);
			osDelay(500);
			SCREEN_Load(SCREEN_Wifi,FONT_12_bold,0);
			break;

		default:
			if((ID_touchDetect>=TOUCH_whiteFieldKey)&&(ID_touchDetect<=(TOUCH_whiteFieldKey+MAX_WHITE_FIELD)))
			{
				dbg("\r\nTOUCH_whiteFieldKey ");
				KEYBOARD_CursorPosChange(TOUCH_whiteFieldKey,x,y,ID_touchDetect-TOUCH_whiteFieldKey,WHITE);
				//KEYBOARD_CursorPosChange(x,y,ID_touchDetect-TOUCH_whiteFieldKey);

			}
			else if(((ID_touchDetect>=TOUCH_firstLetter)&&(ID_touchDetect<=TOUCH_backspace))||
						(ID_touchDetect==TOUCH_nextAnotherSigns) )  //'m' potrzebuke 3 linie !!!!!!!!!
			{
				if(IsVarNotSet(&onePress))
				{
					if(touchPrevious==0)
						touchPrevious=ID_touchDetect;
					else if(touchPrevious!=ID_touchDetect)
						break;

					if(ID_touchDetect!=TOUCH_nextAnotherSigns){
						if(0==KEYBOARD_SetChar(ID_touchDetect))
							break;
					}

					holdPressTime=xTaskGetTickCount();
					TOUCH_KeyID(PRESSED,ID_touchDetect);

					if(ID_touchDetect==TOUCH_nextAnotherSigns)
					{
						if(KeyBoard.pressAnotherSigns==1)
							KeyBoard.pressAnotherSigns=2;
						else if(KeyBoard.pressAnotherSigns==2)
							KeyBoard.pressAnotherSigns=1;
						SCREEN_Load(SCREEN_Keyboard,FONT_28,1);
					}
					else
						KEYBOARD_DisplayTxt();

				}
				else
				{
					if(ID_touchDetect!=TOUCH_nextAnotherSigns)
					{
						if((xTaskGetTickCount()-holdPressTime)>HOLD_PRESS_TIME)
						{
							VarReset(&onePress);
							HOLD_PRESS_TIME=10;
						}
					}
				}
			}
			break;
		}
		break;


	case RELEASED:

		touchPressed=0;
		switch(ID_touchDetect)
		{
		case TOUCH_capitalLetter:
		case TOUCH_anotherSigns:
			VarReset(&onePress);
			break;

		default:
			if(((ID_touchDetect>=TOUCH_firstLetter)&&(ID_touchDetect<=TOUCH_backspace))||
				 (ID_touchDetect==TOUCH_nextAnotherSigns) )
			{
				TOUCH_KeyID(RELEASED,ID_touchDetect);
				if(((touchPrevious>=TOUCH_firstLetter)&&(touchPrevious<=TOUCH_backspace))||
					 (touchPrevious==TOUCH_nextAnotherSigns))
				{
					TOUCH_KeyID(RELEASED,touchPrevious);
				}
				VarReset(&onePress);
				touchPrevious=0;
				HOLD_PRESS_TIME=700;
			}
			break;
		}
		break;
  }
}

