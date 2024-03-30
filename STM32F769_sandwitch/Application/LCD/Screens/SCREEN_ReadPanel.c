/*
 * read_panel.c
 *
 *  Created on: 31.01.2021
 *      Author: Elektronika RM
 */

#include <LCD_BasicGaphics.h>
#include <LCD_Hardware.h>
#include <LCD_SelectID.h>
#include <LCD_SliderID.h>
#include <SCREEN_Common.h>
#include <SCREEN_ReadPanel.h>
#include "stm32f7xx_hal.h"
#include "variables.h"
#include "fonts_images.h"
#include "touch.h"
#include "sdram.h"
#include "StartUp.h"
#include "float.h"
#include "LCD_Common.h"


#define VAL_FONT	fontID_1
#define UNIT_FONT	fontID_2
#define DESCR_FONT	fontID_3
#define VAL_SLIDER_FONT	fontID_4
#define VAL_FONT_RED	fontID_5
#define VAL_FONT_BLUE	fontID_6
#define HEADER_FONT		fontID_7

#define FONT_MAIN_MENU			fontID_16
#define FONT_MAIN_MENU_PRESS	fontID_17


#define MAX_BUFTEMP	500
static char bufTemp[MAX_BUFTEMP];

enum typeFrame{
	sliderFrame,
	buttonFrame,
	tempFrame,
	inputFrame
};

static void LCD_HeaderImage(int nrHeader,int headerHeight)
{
	uint32_t frameColor, fillColor;
	int fromUpMargin=5;

	switch(nrHeader)
	{
	case 0:
		frameColor=MYLIGHTGRAY;	fillColor=0x404040;
		LCD_DrawRectangle(0,0, LCD_GetXSize(), headerHeight, frameColor, fillColor,0);
		TxtID(HEADER_FONT, 5,fromUpMargin, "2020-09-18  12:32",fullHight,0);
		TxtID(HEADER_FONT, 200,fromUpMargin, "GSM",halfHight,0);
		TxtID_Var(18,HEADER_FONT,250,fromUpMargin,int2str(bufTemp,GetVal(Var_gsmStrenght)),halfHight,0);




		LCD_LoadPaint(PAINT_StrenghtGsmSign,285,4,20,0xFFFFFF,0xBBBBBB,0x404040,Var_gsmStrenght);
		LCD_LoadPaint(PAINT_StrenghtGsmSign2,435,4,20,YELLOW,BLACK,0x404040,Var_gsmStrenght);
		LCD_LoadPaint(PAINT_StrenghtGsmSign3,585,4,20,LIGHTRED,LIGHTBLUE,0x404040,Var_gsmStrenght);
		//LCD_StrenghtGsmSign(285,4,20,0xFFFFFF,0xBBBBBB,0x404040,GetVal(Var_gsmStrenght)); //LCD_PAINT(ID_PAINT_GSMZASIEG, param...) do buf_ogolnego[]= i wywolanie tylko PAINT(ID_PAINT_GSMZASIEG) z bufora ogolnego


		ClearAllSelect();
		LCD_Select(SELECT_TYPE_MainMenu,LCD_GetXSize(),0,FONT_MAIN_MENU,FONT_MAIN_MENU_PRESS,FONT_MAIN_MENU,SelectTXT_MainMenu,0,TOUCH_mainMenu,6);
		break;

	case 1:
		break;

	default:
		break;
	}
}

static void LCD_MainPanelTextFrame(int x,int y, int widthFrame,int heightFrame, char *valTxt, char *unitTxt, char *descrTxt, int howMuchLinesForDescrTxt, int typeFrame, int xOffset)
{
	int fromUpMargin=5;
	int fromLefMargin=5;

	int VAL_FONT_;		int sliderSize=15;
	if(typeFrame==sliderFrame)
	{
		VAL_FONT_=VAL_FONT;//VAL_SLIDER_FONT;
		switch(LCD_GetFontID_size(VAL_FONT_))
		{
		case FONT_130_bold:
		case FONT_130:	sliderSize=28;	break;
		case FONT_72_bold:
		case FONT_72:	sliderSize=23;	break;
		case FONT_48_bold:
		case FONT_48:	sliderSize=20;	break;
		case FONT_36_bold:
		case FONT_36:	sliderSize=15;	break;
		case FONT_28_bold:
		case FONT_28:	sliderSize=15;	break;
		case FONT_26_bold:
		case FONT_26:	sliderSize=15;	break;
		case FONT_24_bold:
		case FONT_24:	sliderSize=15;	break;
		case FONT_22_bold:
		case FONT_22:	sliderSize=15;	break;
		case FONT_20_bold:
		case FONT_20:	sliderSize=10;	break;
		case FONT_18_bold:
		case FONT_18:	sliderSize=10;	break;
		case FONT_16_bold:
		case FONT_16:	sliderSize=10;	break;
		case FONT_14_bold:
		case FONT_14:	sliderSize=10;	break;
		case FONT_12_bold:
		case FONT_12:	sliderSize=10;	break;
		case FONT_11_bold:
		case FONT_11:	sliderSize=10;	break;
		case FONT_10_bold:
		case FONT_10:	sliderSize=10;	break;
		case FONT_9_bold:
		case FONT_9:	sliderSize=10;	break;
		case FONT_8_bold:
		case FONT_8:	sliderSize=10;	break;
		default: 	sliderSize=10;	break;
		}

	}
	else
		VAL_FONT_=VAL_FONT;

	int ValFontSize=LCD_GetFontID_size(VAL_FONT_);

	int WidthDescrTxt = ReturnWidthFontID(DESCR_FONT,descrTxt);
	int HeightValTxt = ReturnHeightFontID(VAL_FONT_);
	int HalfHeightValTxt = ReturnHeightHalfFontID(VAL_FONT_);

	int valframePosX = (widthFrame-ReturnWidthFontID(VAL_FONT_, valTxt))/2 - xOffset;
	int valframePosY = fromUpMargin;

	int unitframePosX = valframePosX+ReturnWidthFontID(VAL_FONT_, valTxt)+ReturnWidthFontID(VAL_FONT_, " ");
	int unitframePosY = valframePosY+(HalfHeightValTxt-ReturnHeightHalfFontID(UNIT_FONT));

	int descrframePosX = WidthDescrTxt>(widthFrame-2*fromLefMargin) ? fromLefMargin: (widthFrame-WidthDescrTxt)/2;
	int descrframePosY = valframePosY;

    if((ValFontSize==FONT_130)||(ValFontSize==FONT_130_bold))
    	descrframePosY += HeightValTxt;
    else descrframePosY += (10*HeightValTxt)/11;

	int LinesForDescrTxt = WidthDescrTxt<=(widthFrame-2*fromLefMargin) ? 1: 1+WidthDescrTxt/(widthFrame-2*fromLefMargin);
	int _howMuchLinesForDescrTxt;
    switch(typeFrame)
    {
    case sliderFrame:
    	_howMuchLinesForDescrTxt = howMuchLinesForDescrTxt-1;
    	 descrframePosY += ReturnHeightFontID(DESCR_FONT);
    	 break;
    default:
    	_howMuchLinesForDescrTxt = howMuchLinesForDescrTxt;
    	break;
    }
    int varTemp = ((_howMuchLinesForDescrTxt-LinesForDescrTxt)*ReturnHeightFontID(DESCR_FONT))/2;
    if(varTemp>0) descrframePosY += varTemp;


	switch(typeFrame)
	{
	case sliderFrame:
			 if(ValFontSize<FONT_16) 						   LCD_DrawImage("LCD_images/slider_vlow.bmp", x+fromLefMargin, y+fromUpMargin);
		else if((ValFontSize>=FONT_16)&&(ValFontSize<FONT_22)) LCD_DrawImage("LCD_images/slider_low.bmp",  x+fromLefMargin, y+fromUpMargin);
		else if((ValFontSize>=FONT_22)&&(ValFontSize<FONT_36)) LCD_DrawImage("LCD_images/slider_med.bmp",  x+fromLefMargin, y+fromUpMargin);
		else											       LCD_DrawImage("LCD_images/slider_high.bmp", x+fromLefMargin, y+fromUpMargin);

    	SetVal(Var_slider0, 51);

    	//LCD_DrawSliderWithButtons_H(SLIDER_ID1, x+fromLefMargin, y+fromUpMargin+HalfHeightValTxt+3, widthFrame-2*fromLefMargin, sliderSize, GetVal(Var_slider0),slider0, slider0_left, slider0_right);
    	ClearAllSlider();
    	LCD_DrawSliderWithButtons(SLIDER_ID1,HORIZONTAL, x+fromLefMargin, y+fromUpMargin+HalfHeightValTxt+3, widthFrame-2*fromLefMargin, sliderSize, Var_slider0, slider0, slider0_left, slider0_right);


    	//mini_snprintf(bufTemp,50,"%03d ",GetVal(Var_slider0));
    	//TxtID_Var(1,VAL_FONT_, x+valframePosX, y+valframePosY, bufTemp,halfHight,0);  //SetTouch(image_2);
    	/////LCD_TxtIDSlider(SLIDER_ID1,VAL_FONT_, x+valframePosX,y+valframePosY,TXT_ID2);
    	TxtID_Var(TXT_ID2,VAL_FONT_, x+valframePosX,y+valframePosY, int2perctStr(GetVal(Var_slider0),'0'),halfHight,0);


		TxtID(UNIT_FONT, x+unitframePosX,   y+unitframePosY, unitTxt,halfHight,0);
    	TxtLineID(DESCR_FONT, x+descrframePosX,  y+descrframePosY, descrTxt, widthFrame-2*fromLefMargin, _howMuchLinesForDescrTxt);
		break;

	case buttonFrame:
		 	 if(ValFontSize<FONT_16) 						   LCD_DrawImage("LCD_images/wilg_vlow.bmp", x+fromLefMargin, y+fromUpMargin);
		else if((ValFontSize>=FONT_16)&&(ValFontSize<FONT_22)) LCD_DrawImage("LCD_images/wilg_low.bmp",  x+fromLefMargin, y+fromUpMargin);
		else if((ValFontSize>=FONT_22)&&(ValFontSize<FONT_36)) LCD_DrawImage("LCD_images/wilg_med.bmp",  x+fromLefMargin, y+fromUpMargin);
		else											       LCD_DrawImage("LCD_images/wilg_high.bmp", x+fromLefMargin, y+fromUpMargin);

    	TxtID_Var(19,VAL_FONT_, x+valframePosX, y+valframePosY, valTxt,halfHight,0); SetTouch(mainFrame_1);
    	TxtID(UNIT_FONT, x+unitframePosX,   y+unitframePosY, unitTxt,0,0);
    	TxtLineID(DESCR_FONT, x+descrframePosX,  y+descrframePosY, descrTxt, widthFrame-2*fromLefMargin, _howMuchLinesForDescrTxt);
		break;

	default:
	 	  	 if(ValFontSize<FONT_16) 						   LCD_DrawImage("LCD_images/term_vlow.bmp", x+fromLefMargin, y+fromUpMargin);
	 	else if((ValFontSize>=FONT_16)&&(ValFontSize<FONT_22)) LCD_DrawImage("LCD_images/term_low.bmp",  x+fromLefMargin, y+fromUpMargin);
	 	else if((ValFontSize>=FONT_22)&&(ValFontSize<FONT_36)) LCD_DrawImage("LCD_images/term_med.bmp",  x+fromLefMargin, y+fromUpMargin);
	 	else											       LCD_DrawImage("LCD_images/term_high.bmp", x+fromLefMargin, y+fromUpMargin);

    	TxtID(VAL_FONT_, x+valframePosX,    y+valframePosY, valTxt,halfHight,0);
    	TxtID(UNIT_FONT, x+unitframePosX,   y+unitframePosY, unitTxt,0,0);
    	TxtLineID(DESCR_FONT, x+descrframePosX,  y+descrframePosY, descrTxt, widthFrame-2*fromLefMargin, _howMuchLinesForDescrTxt);
		break;
	}



}

void SCREEN_MainPanel(int screenID, int ValuefontSize, int resumeScreen)  //main
{
	int widthLcd = LCD_GetXSize();
	int heightLcd = LCD_GetYSize();

	uint32_t frameBkColor=0xFF000000;
	uint32_t frameFrColor=LIGHTGRAY;
	uint32_t frameFillColor=0xFFFF0000;

	DeleteAllTouch();
	LCD_DeleteAllFontAndImages();
	ClearAllPaints();

	LCD_ClearScreen(0xFF000000);

	LCD_DrawRoundRectangle3_AA_TEST(100,100, 200,50, frameFrColor, frameFillColor,frameBkColor);
	LCD_DrawRoundRectangle3_AA_TEST(400,100, 200,50, frameFrColor, frameFillColor,frameBkColor);

	LCD_DrawRoundRectangle3_AA_TEST(100,300, 200,50, YELLOW, LIGHTRED,frameBkColor);
	LCD_DrawRoundRectangle3_AA_TEST(400,300, 200,50, LIGHTBLUE, BROWN,frameBkColor);




	LCD_Display(0,(uint32_t)0,(uint32_t)0,(uint32_t)widthLcd,(uint32_t)heightLcd);
	//LCD_DisplayBuff(0,(uint32_t)0,(uint32_t)0,(uint32_t)widthLcd,(uint32_t)heightLcd);
}

void SCREEN_MainPanel__(int screenID, int ValuefontSize, int resumeScreen)  //main
{
	char valTxt[10];
	char unitTxt[10];
	char descrTxt[150];
	int typeFrame, xOffset;

	int valFontSize,   valFontStyle,   valFontBkColor,	 valFontColor;
	int unitFontSize,  unitFontStyle,  unitFontBkColor,	 unitFontColor;
	int descrFontSize, descrFontStyle, descrFontBkColor, descrFontColor;

	int headerMarginFromUp=5;

	int heightMenu, spaceFromMenu;
	int spaceBetwenFrame;
	int widthFrameMarginFactor;
	int txtMarginFromUpDown;
	int howMuchLinesForDescrTxt;
	int nrFrame=0;

	int widthLcd = LCD_GetXSize();
	int heightLcd = LCD_GetYSize();

	int smallerUnitFontFactor=3;

	SetVal(Var_gsmStrenght,15);

	switch(screenID)
	{
	case 0:
			 if(ValuefontSize>=FONT_130) smallerUnitFontFactor=2;
		else if((ValuefontSize>=FONT_72)&&(ValuefontSize<FONT_130)) smallerUnitFontFactor=1;
		else if(ValuefontSize<FONT_48) smallerUnitFontFactor=5;
		else smallerUnitFontFactor=3;

		valFontSize=ValuefontSize;	 		valFontStyle=Arial;		valFontBkColor=DARKGRAY;	 valFontColor=WHITE;
		unitFontSize=valFontSize-3*smallerUnitFontFactor;		unitFontStyle=Arial;	unitFontBkColor=DARKGRAY;	 unitFontColor=BLUE;
		descrFontSize=FONT_8;		descrFontStyle=Arial;	descrFontBkColor=DARKGRAY;	 descrFontColor=GREEN;

		while(unitFontSize<0) unitFontSize+=3;
		/*heightMenu=30;*/	spaceFromMenu=10;	spaceBetwenFrame=10;	txtMarginFromUpDown=2;
		widthFrameMarginFactor=3;
		howMuchLinesForDescrTxt=3;
		break;

	default:
		     if(ValuefontSize>=FONT_130) smallerUnitFontFactor=2;
		else if((ValuefontSize>=FONT_72)&&(ValuefontSize<FONT_130)) smallerUnitFontFactor=1;
		else smallerUnitFontFactor=3;

		valFontSize=ValuefontSize;	 		valFontStyle=Arial;		valFontBkColor=DARKGRAY;	 valFontColor=WHITE;
		unitFontSize=valFontSize-3*smallerUnitFontFactor;		unitFontStyle=Arial;	unitFontBkColor=DARKGRAY;	 unitFontColor=BLUE;
		descrFontSize=FONT_8;		descrFontStyle=Arial;	descrFontBkColor=DARKGRAY;	 descrFontColor=GREEN;

		while(unitFontSize<0) unitFontSize+=3;
		/*heightMenu=50;*/	spaceFromMenu=15;	spaceBetwenFrame=10;	txtMarginFromUpDown=2;
		widthFrameMarginFactor=3;
		howMuchLinesForDescrTxt=3;
		break;
	}
	DeleteAllTouch();

	if(resumeScreen==0)
	{
		LCD_DeleteAllFontAndImages();
		ClearAllPaints();

		LCD_LoadFont(FONT_11,Arial,DARKGRAY,WHITE, HEADER_FONT,1);
		LCD_LoadFont(FONT_11,Arial,DARKGRAY,BLUE, FONT_MAIN_MENU_PRESS,0);
		LCD_LoadFont(FONT_11,Arial,BROWN,WHITE, FONT_MAIN_MENU,0);
		LCD_LoadFont(descrFontSize, descrFontStyle, descrFontBkColor, descrFontColor, DESCR_FONT,0);

		LCD_LoadFont(valFontSize, valFontStyle, valFontBkColor, valFontColor, VAL_FONT,1);
		LCD_LoadFont(valFontSize, valFontStyle, valFontBkColor, RED, VAL_FONT_RED,1);
		LCD_LoadFont(valFontSize, valFontStyle, valFontBkColor, BLUE, VAL_FONT_BLUE,1);
		LCD_LoadFont(unitFontSize, unitFontStyle, unitFontBkColor, unitFontColor, UNIT_FONT,0);

		LCD_LoadImage("LCD_images/slider_vlow.bmp");
		LCD_LoadImage("LCD_images/slider_low.bmp");
		LCD_LoadImage("LCD_images/slider_med.bmp");
		LCD_LoadImage("LCD_images/slider_high.bmp");

		LCD_LoadImage("LCD_images/term_vlow.bmp");
		LCD_LoadImage("LCD_images/term_low.bmp");
		LCD_LoadImage("LCD_images/term_med.bmp");
		LCD_LoadImage("LCD_images/term_high.bmp");

		LCD_LoadImage("LCD_images/wilg_vlow.bmp");
		LCD_LoadImage("LCD_images/wilg_low.bmp");
		LCD_LoadImage("LCD_images/wilg_med.bmp");
		LCD_LoadImage("LCD_images/wilg_high.bmp");

		LCD_LoadImage("pex.bmp");
	}

	int widthFrameMargin = widthFrameMarginFactor*ReturnWidthFontID(VAL_FONT, "2");
	int widthFrame = widthFrameMargin + ReturnWidthFontID(VAL_FONT, "2222") + widthFrameMargin;
	int heightFrame = txtMarginFromUpDown+ReturnHeightFontID(VAL_FONT)+howMuchLinesForDescrTxt*ReturnHeightFontID(DESCR_FONT)+txtMarginFromUpDown;

	uint32_t frameBkColor=0x000000;
	uint32_t frameFrColor=LIGHTGRAY;
	uint32_t frameFillColor=DARKGRAY;   //DbgSprintf(50,"\r\nHHH:  %d  ", ValuefontSize);

	heightMenu = ReturnHeightFontID(HEADER_FONT)+2*headerMarginFromUp;

	LCD_Clear(0x000000);
	LCD_HeaderImage(0,heightMenu);

	int ix=0, iy=heightMenu+spaceFromMenu;
	do
	{
		xOffset=0;
	    switch(nrFrame)
		{
		case 1:
			typeFrame=sliderFrame;
			mini_snprintf(valTxt,10,"000");
			mini_snprintf(unitTxt,10," "); unitTxt[0]='%';
			mini_snprintf(descrTxt,150,"Slider suwaki przesuwne");
			break;
		case 2:
			typeFrame=buttonFrame;
			mini_snprintf(valTxt,10,"-99.9");
			mini_snprintf(unitTxt,10,"kg/m3");  xOffset=ReturnWidthFontID(UNIT_FONT, "2");
			mini_snprintf(descrTxt,150,"Jestem tu i teraz na dole");
			break;
		default:
			typeFrame=tempFrame;
			mini_snprintf(valTxt,10,"+%02d.0",nrFrame);
			mini_snprintf(unitTxt,10,"°C");
			mini_snprintf(descrTxt,150,"Temperatura w pomieszczeniu gospodarczym Temperatura w pomieszczeniu gospodarczym");
			break;
		}


		if((ix+spaceBetwenFrame+widthFrame) <= widthLcd)
		{
			//LCD_DrawRectangle(ix,iy, widthFrame,heightFrame, frameFrColor, frameFillColor);
			LCD_DrawRoundRectangle3(ix,iy, widthFrame,heightFrame, frameFrColor, frameFillColor,frameBkColor);
			LCD_MainPanelTextFrame(ix,iy, widthFrame,heightFrame,valTxt, unitTxt, descrTxt, howMuchLinesForDescrTxt, typeFrame, xOffset);
			nrFrame++;
			ix+=spaceBetwenFrame+widthFrame;
		}
		else
		{
			iy+=spaceBetwenFrame+heightFrame;
			if((iy+spaceBetwenFrame+heightFrame) <= heightLcd)
				ix=0;
			else
				break;
		}

	}while(1);

	LCD_DisplayBuff(0,(uint32_t)0,(uint32_t)0,(uint32_t)widthLcd,(uint32_t)heightLcd);
}

extern int varTemp;



extern int touchPressed;
extern int prescVal;

void TOUCH_MainPanel(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y)
{
	int temp;

	vTOUCH_SelectService(ID_touchDetect,pressRelease);

	if(0==TOUCH_SliderService(ID_touchDetect,pressRelease,x,y))
		touchPressed=1;
	if(PRESSED==pressRelease){
	if(IsTouchSlider(SLIDER_ID1,ID_touchDetect))
	{
		if((GetVal(Var_slider0)>40)&&(GetVal(Var_slider0)<60))  //tu jezeli dotyk odpowiedni to realizuj to a nie tak!!!!!!!
			LCD_Txt(TXT_ID2, int2perctStr(GetVal(Var_slider0),'0'), fontNoChange, halfHight,0);
		else if(GetVal(Var_slider0)>=60)
			LCD_Txt(TXT_ID2, int2perctStr(GetVal(Var_slider0),'0'), VAL_FONT_RED, halfHight,0);
		else
			LCD_Txt(TXT_ID2, int2perctStr(GetVal(Var_slider0),'0'), VAL_FONT_BLUE, halfHight,0);
	}}



	switch (pressRelease)
	{
	case PRESSED:

		switch(ID_touchDetect)
		{


		case mainFrame_1:
			varTemp++;
			if((varTemp>40)&&(varTemp<60))	temp=fontNoChange;
			else if(varTemp>=60)			temp=VAL_FONT_RED;
			else							temp=VAL_FONT_BLUE;
			LCD_Txt(19, Temp2str(varTemp), temp, halfHight,0);
			osDelay(30);
			break;

		default:
			break;
		}
		break;

	case RELEASED:

		touchPressed=0;
		switch(ID_touchDetect)
		{
		case mainFrame_1:
			if((varTemp>40)&&(varTemp<60))	temp=fontNoChange;
			else if(varTemp>=60)			temp=VAL_FONT_RED;
			else							temp=VAL_FONT_BLUE;
			LCD_Txt(19, "1+2=3", temp, halfHight,0);
			osDelay(500);
			LCD_Txt(19, Temp2str(varTemp), temp, halfHight,0);
		   break;

		}
		break;
  }
}

