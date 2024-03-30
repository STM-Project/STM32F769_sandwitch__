/*
 * LCD_sliderID.c
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */
#include "LCD_BasicGaphics.h"
#include "LCD_SliderID.h"
#include "touch.h"
#include "cmsis_os.h"
#include "LCD_Common.h"
#include "fonts_images.h"
#include "math.h"
#include "sdram.h"

#define MAX_NUMBER_SLIDER	10

SRAM static int sliderTabParam[MAX_NUMBER_SLIDER][18];   //Zamieni z int na uint16  bedzie osczedniej !!!!

static void LCD_DrawSlider_H(int ID_Slider,int xPos,int yPos, int width,int height, int value)
{
	int heightS1;	uint32_t bkColor;	uint32_t wskColor;
	int heightS2;	uint32_t frameS1;	uint32_t frameS2;
	int widthS2;	uint32_t fillS1;	uint32_t fillS2;
	int YposS2;
	int val;
	int startEndPos;

	if(ID_Slider>=MAX_NUMBER_SLIDER) return;
	switch(ID_Slider)
	{
	case SLIDER_ID1:
	case SLIDER_ID2:
	case SLIDER_ID3:
	case SLIDER_ID4:
	case SLIDER_ID5:
	case SLIDER_ID6:
	case SLIDER_ID7:
	case SLIDER_ID8:
	case SLIDER_ID9:
	case SLIDER_ID10:
	default:
		heightS1 = height/2;		bkColor=0x404040;		wskColor=0xFF0000;
		heightS2 = height;		frameS1=0xD0D0D0;		frameS2=0xD0D0D0;
		widthS2  = height/2;		fillS1=0x404040;		fillS2=0x0000FF;
		YposS2   =  0;
		startEndPos=0;

		if((height<heightS1)||(height<heightS2)) return;
		val=((value*(width-widthS2))/100);
		if(val<startEndPos) val=startEndPos;
		if(val>width-widthS2-startEndPos) val=width-widthS2-startEndPos;

		sliderTabParam[ID_Slider][0]=xPos;
		sliderTabParam[ID_Slider][1]=yPos;
		sliderTabParam[ID_Slider][2]=width;
		sliderTabParam[ID_Slider][3]=height;
		sliderTabParam[ID_Slider][4]=widthS2;

		LCD_DrawRectangle(xPos, yPos, width,height, bkColor, bkColor,0); //tlo
		LCD_DrawRoundRectangle1(xPos, yPos+height/2-heightS1/2, width, heightS1,frameS1, fillS1,bkColor); //prowadnica
		LCD_DrawRoundRectangle1(xPos, yPos+height/2-heightS1/2, val+widthS2/2, heightS1, frameS2, wskColor,bkColor); //wskaznik w kolorze wskColor
		LCD_DrawRoundRectangle1(xPos+val, yPos+YposS2+height/2-heightS2/2, widthS2, heightS2,frameS2, fillS2,bkColor); //suwak
		LCD_DrawRectangle(xPos+val+widthS2/2-1, yPos+(heightS2-heightS1)/4, 2,2,0xEEEEEE, 0xEEEEEE,0);  // punk na suwaku

		touchTemp.x_Start= xPos+widthS2/2;
		touchTemp.y_Start= yPos;
		touchTemp.x_End= xPos+width-widthS2/2;
		touchTemp.y_End= yPos+height+30;
	}
}

static void LCD_DrawSlider_V(int ID_Slider,int xPos,int yPos, int width,int height, int value)
{
	int heightS1;	uint32_t bkColor;	uint32_t wskColor;
	int heightS2;	uint32_t frameS1;	uint32_t frameS2;
	int widthS2;	uint32_t fillS1;	uint32_t fillS2;
	int YposS2;
	int val;
	int startEndPos;

	if(ID_Slider>=MAX_NUMBER_SLIDER) return;
	switch(ID_Slider)
	{
	case SLIDER_ID1:
	case SLIDER_ID2:
	case SLIDER_ID3:
	case SLIDER_ID4:
	case SLIDER_ID5:
	case SLIDER_ID6:
	case SLIDER_ID7:
	case SLIDER_ID8:
	case SLIDER_ID9:
	case SLIDER_ID10:
	default:
		heightS1 = height/2;		bkColor=0x404040;		wskColor=0xFF0000;
		heightS2 = height;		frameS1=0xD0D0D0;		frameS2=0xD0D0D0;
		widthS2  = height/2;		fillS1=0x404040;		fillS2=0x0000FF;
		YposS2   =  0;
		startEndPos=0;

		if((height<heightS1)||(height<heightS2)) return;
		val=((value*(width-widthS2))/100);
		if(val<startEndPos) val=startEndPos;
		if(val>width-widthS2-startEndPos) val=width-widthS2-startEndPos;

		sliderTabParam[ID_Slider][0]=xPos;
		sliderTabParam[ID_Slider][1]=yPos;
		sliderTabParam[ID_Slider][2]=width;
		sliderTabParam[ID_Slider][3]=height;
		sliderTabParam[ID_Slider][4]=widthS2;

		LCD_DrawRectangle      (xPos,                              yPos,         height,          width,          bkColor, bkColor,0); //tlo
		LCD_DrawRoundRectangle3(xPos+height/2-heightS1/2,          yPos,         heightS1,       width,          frameS1, fillS1,bkColor); //prowadnica
		LCD_DrawRoundRectangle3(xPos+height/2-heightS1/2,          yPos,         heightS1,       val+widthS2/2,  frameS2, wskColor,bkColor); //wskaznik w kolorze wskColor
		LCD_DrawRoundRectangle3(xPos+YposS2+height/2-heightS2/2,   yPos+val ,    heightS2,       widthS2,         frameS2, fillS2,bkColor); //suwak
		LCD_DrawRectangle      (xPos+height/2-heightS1/2,          yPos+val+3,   2,             2,              0xEEEEEE, 0xEEEEEE,0);  // punk na suwaku

		touchTemp.x_Start= xPos;
		touchTemp.y_Start= yPos+widthS2/2;
		touchTemp.x_End= xPos+height+30;
		touchTemp.y_End= yPos+width-widthS2/2;
	}
}

static int LCD_DrawSliderIndirect_H(int ID_Slider, int xPos,int yPos, int width,int height, int value)
{
	int heightS1;	uint32_t bkColor;	uint32_t wskColor;
	int heightS2;	uint32_t frameS1;	uint32_t frameS2;
	int widthS2;	uint32_t fillS1;	uint32_t fillS2;
	int YposS2;
	int val;
	int startEndPos;

	if(ID_Slider>=MAX_NUMBER_SLIDER) return 0;
	switch(ID_Slider)
	{
	case SLIDER_ID1:
	case SLIDER_ID2:
	case SLIDER_ID3:
	case SLIDER_ID4:
	case SLIDER_ID5:
	case SLIDER_ID6:
	case SLIDER_ID7:
	case SLIDER_ID8:
	case SLIDER_ID9:
	case SLIDER_ID10:
	default:
		heightS1 = height/2;		bkColor=0x404040;		wskColor=0xFF0000;
		heightS2 = height;		frameS1=0xD0D0D0;		frameS2=0xD0D0D0;
		widthS2  = height/2;		fillS1=0x404040;		fillS2=0x0000FF;
		YposS2   =  0;
		startEndPos=0;

		if((height<heightS1)||(height<heightS2)) return 0;
		val=value;
		if(val<startEndPos) val=startEndPos;
		if(val>width-widthS2-startEndPos) val=width-widthS2-startEndPos;

		LCD_DrawRectangleBuff(0, width, height,bkColor, bkColor, bkColor); //t這
		LCD_DrawRoundRectangle1Buff(100000, width, heightS1,frameS1, fillS1,bkColor); //prowadnica  DAC ZEBY FUNKCJA ZWRACALA DLUGOSC ZAJECIA pLCD !!!!!!!
		LCD_DrawRoundRectangle1Buff(200000, widthS2, heightS2,frameS2, fillS2,bkColor); //suwak
		LCD_DrawRoundRectangle1Buff(300000, val+widthS2/2, heightS1,frameS2, wskColor,bkColor); //wskaznik w kolorze wskColor
		LCD_DrawRectangleBuff(400000, 2,2,0xEEEEEE, 0xEEEEEE,fillS2);  // punk na suwaku
		//  LCD_DrawBmpBuff(500000,"pex.bmp");

		LCD_ImageComposition(0, 100000, 0, height/2-heightS1/2);
		LCD_ImageComposition(0, 300000, 0, height/2-heightS1/2);
		 // LCD_ImageComposition(0, 500000, width/2, 4);
		LCD_ImageComposition(0, 200000, val, YposS2+height/2-heightS2/2);
		LCD_ImageComposition(0, 400000, val+widthS2/2-1, (heightS2-heightS1)/4);

		LCD_DisplayBuff(4,xPos,yPos,width,height);
		return ((int)(((float)val *100) / ((float)(width-widthS2))));
	}
}

static int LCD_DrawSliderIndirect_V(int ID_Slider, int xPos,int yPos, int width,int height, int value)
{
	int heightS1;	uint32_t bkColor;	uint32_t wskColor;
	int heightS2;	uint32_t frameS1;	uint32_t frameS2;
	int widthS2;	uint32_t fillS1;	uint32_t fillS2;
	int YposS2;
	int val;
	int startEndPos;

	if(ID_Slider>=MAX_NUMBER_SLIDER) return 0;

	switch(ID_Slider)
	{
	case SLIDER_ID1:
	case SLIDER_ID2:
	case SLIDER_ID3:
	case SLIDER_ID4:
	case SLIDER_ID5:
	case SLIDER_ID6:
	case SLIDER_ID7:
	case SLIDER_ID8:
	case SLIDER_ID9:
	case SLIDER_ID10:
	default:
		heightS1 = height/2;	bkColor=0x404040;	wskColor=0xFF0000;
		heightS2 = height;	frameS1=0xD0D0D0;	frameS2=0xD0D0D0;
		widthS2  = height/2;	fillS1=0x404040;	fillS2=0x0000FF;
		YposS2   =  0;
		startEndPos=0;

		if((height<heightS1)||(height<heightS2)) return 0;
		val=value+sliderTabParam[ID_Slider][0]-sliderTabParam[ID_Slider][1];
		if(val<startEndPos) val=startEndPos;
		if(val>width-widthS2-startEndPos) val=width-widthS2-startEndPos;

		LCD_DrawRectangleBuff(0,              height,        width,		 		bkColor, bkColor, bkColor); //t這
		LCD_DrawRoundRectangle3Buff(100000,   heightS1,      width,				frameS1, fillS1,bkColor); //prowadnica
		LCD_DrawRoundRectangle3Buff(200000,   heightS2,      widthS2,			frameS2, fillS2,bkColor); //suwak
		LCD_DrawRoundRectangle3Buff(300000,   heightS1,      val+widthS2/2,		frameS2, wskColor,bkColor); //wskaznik w kolorze wskColor
		LCD_DrawRectangleBuff(400000,         2,            2,					0xEEEEEE,	 0xEEEEEE,fillS2);  // punk na suwaku

		LCD_ImageComposition(0, 100000,   height/2-heightS1/2, 			0);
		LCD_ImageComposition(0, 300000,   height/2-heightS1/2,			0);
		LCD_ImageComposition(0, 200000,   YposS2+height/2-heightS2/2, 	val);
		LCD_ImageComposition(0, 400000,   height/2-heightS1/2, 			val+widthS2/2-1);

		LCD_DisplayBuff(4,xPos,yPos,height,width);
		return ((int)(((float)val *100) / ((float)(width-widthS2))));
	}
}

static int LCD_ReturnPositionSlider_H(int ID_Slider,float percentValue)
{
	return (int)((percentValue * ((float)(sliderTabParam[ID_Slider][2]-sliderTabParam[ID_Slider][4])))/100) + sliderTabParam[ID_Slider][0]+sliderTabParam[ID_Slider][4]/2;
}
static int LCD_ReturnPositionSlider_V(int ID_Slider,float percentValue)
{
	return (int)((percentValue * ((float)(sliderTabParam[ID_Slider][2]-sliderTabParam[ID_Slider][4])))/100) + sliderTabParam[ID_Slider][1]+sliderTabParam[ID_Slider][4]/2;
}

static void LCD_UpSliderIndirect(int nrSlider, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	uint32_t Xpos=sliderTabParam[nrSlider][7];
	uint32_t Ypos=sliderTabParam[nrSlider][5];
	uint32_t width=sliderTabParam[nrSlider][8];

	uint32_t widthTriangle=width;
	LCD_DrawRectangleBuff(0, width, width/2+1,BkColor, BkColor, BkColor); //t這
	LCD_DrawUpTriangleBuff(500000, widthTriangle, frameColor, fillColor, BkColor);
	LCD_ImageComposition(0, 500000, (width-widthTriangle)/2, (width/2-widthTriangle/2)/2);
	LCD_DisplayBuff(4,Xpos,Ypos,width,width/2+1);
}
static void LCD_DownSliderIndirect(int nrSlider, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	uint32_t Xpos=sliderTabParam[nrSlider][7];
	uint32_t Ypos=sliderTabParam[nrSlider][6];
	uint32_t width=sliderTabParam[nrSlider][8];

	uint32_t widthTriangle=width;
	LCD_DrawRectangleBuff(0, width, width/2+1,BkColor, BkColor, BkColor); //t這
	LCD_DrawDownTriangleBuff(500000, widthTriangle, frameColor, fillColor, BkColor);
	LCD_ImageComposition(0, 500000, (width-widthTriangle)/2, (width/2-widthTriangle/2)/2);
	LCD_DisplayBuff(4,Xpos,Ypos,width,width/2+1);
}
static void LCD_RightSliderIndirect(int nrSlider, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)  //TO TYLKO static po wyczyszczeniu sreen readPanel
{
	uint32_t Xpos=sliderTabParam[nrSlider][6];
	uint32_t Ypos=sliderTabParam[nrSlider][7];
	uint32_t width=sliderTabParam[nrSlider][8];

	uint32_t widthTriangle=width;
	LCD_DrawRectangleBuff(0, width/2+1, width+1,BkColor, BkColor, BkColor); //t這
	LCD_DrawRightTriangleBuff(500000, widthTriangle, frameColor, fillColor, BkColor);
	LCD_ImageComposition(0, 500000, (width/2-widthTriangle/2)/2, (width-widthTriangle)/2);
	LCD_DisplayBuff(4,Xpos,Ypos,width/2+1,width+1);
}
static void LCD_LeftSliderIndirect(int nrSlider, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	uint32_t Xpos=sliderTabParam[nrSlider][5];
	uint32_t Ypos=sliderTabParam[nrSlider][7];
	uint32_t width=sliderTabParam[nrSlider][8];

	uint32_t widthTriangle=width;
	LCD_DrawRectangleBuff(0,  width/2+1, width+1, BkColor, BkColor, BkColor); //t這
	LCD_DrawLeftTriangleBuff(500000, widthTriangle, frameColor, fillColor, BkColor);
	LCD_ImageComposition(0, 500000, (width/2-widthTriangle/2)/2, (width-widthTriangle)/2);
	LCD_DisplayBuff(4,Xpos,Ypos,width/2+1,width+1);
}

void LCD_DrawSliderWithButtons_H(int ID_Slider,int xPos,int yPos, int width,int height, int value, uint16_t touchSliderID, uint16_t touchLeftID,uint16_t touchRightID)
{
	uint32_t bkColor; 	uint32_t buttonFrameColor;		uint32_t buttonFillColor;
	uint32_t spaceBetweenSliderButton=5;
	uint32_t widthFieldButton = height/2;
	uint32_t heightFieldButton = height;
	uint32_t widthButton = heightFieldButton;
	uint32_t diffWidthY= (heightFieldButton-widthButton)/2;
	uint32_t diffWidthX= (widthFieldButton-widthButton/2)/2;

	if(ID_Slider>=MAX_NUMBER_SLIDER) return;
	switch(ID_Slider)
	{
	case SLIDER_ID1:
	case SLIDER_ID2:
	case SLIDER_ID3:
	case SLIDER_ID4:
	case SLIDER_ID5:
	case SLIDER_ID6:
	case SLIDER_ID7:
	case SLIDER_ID8:
	case SLIDER_ID9:
	case SLIDER_ID10:
	default:
		bkColor=DARKGRAY;		buttonFrameColor=WHITE;		buttonFillColor=0xBBBBBB;

		LCD_DrawRectangle    (xPos,             yPos,             widthFieldButton+1, heightFieldButton,  bkColor,          bkColor,0);
		LCD_DrawLeftTriangle (xPos+diffWidthX,  yPos+diffWidthY,  widthButton,		                    buttonFrameColor, buttonFillColor, bkColor);
		touchTemp.x_Start= xPos;
		touchTemp.y_Start= yPos;
		touchTemp.x_End= touchTemp.x_Start+widthFieldButton+1;
		touchTemp.y_End= touchTemp.y_Start+heightFieldButton+1;
		if(touchLeftID>0)
			SetTouch(touchLeftID);

		LCD_DrawRectangle    (xPos+width-widthFieldButton,              yPos,             widthFieldButton+1, heightFieldButton,  bkColor,          bkColor,0);
		LCD_DrawRightTriangle(xPos+width-widthFieldButton+diffWidthX,   yPos+diffWidthY,  widthButton, 	                          buttonFrameColor, buttonFillColor, bkColor);
		touchTemp.x_Start= xPos+width-widthFieldButton;
		touchTemp.y_Start= yPos;
		touchTemp.x_End= touchTemp.x_Start+widthFieldButton+1;
		touchTemp.y_End= touchTemp.y_Start+heightFieldButton+1;
		if(touchRightID>0)
			SetTouch(touchRightID);

		sliderTabParam[ID_Slider][5]=xPos+diffWidthX;  //left button
		sliderTabParam[ID_Slider][6]=xPos+diffWidthX+width-widthFieldButton;   //right button
		sliderTabParam[ID_Slider][7]=yPos+diffWidthY;
		sliderTabParam[ID_Slider][8]=widthButton;
		sliderTabParam[ID_Slider][9]=buttonFrameColor;
		sliderTabParam[ID_Slider][10]=buttonFillColor;
		sliderTabParam[ID_Slider][11]=bkColor;
		sliderTabParam[ID_Slider][12]=HORIZONTAL;
		sliderTabParam[ID_Slider][13]=touchSliderID;
		sliderTabParam[ID_Slider][14]=touchLeftID;
		sliderTabParam[ID_Slider][15]=touchRightID;
	}

	LCD_DrawSlider_H(ID_Slider, xPos+widthFieldButton+spaceBetweenSliderButton, yPos, width-2*widthFieldButton-2*spaceBetweenSliderButton, height, value);
	if(touchSliderID>0)
		SetTouch(touchSliderID);
}

void LCD_DrawSliderWithButtons_V(int ID_Slider,int xPos,int yPos, int width,int height, int value, uint16_t touchSliderID, uint16_t touchUpID,uint16_t touchDownID)
{
	uint32_t bkColor;	uint32_t buttonFrameColor;  uint32_t buttonFillColor;
	uint32_t spaceBetweenSliderButton=15;
	uint32_t widthFieldButton = height;
	uint32_t heightFieldButton = height/2;
	uint32_t widthButton = widthFieldButton;
	uint32_t diffWidthY= (heightFieldButton-widthButton/2)/2;
	uint32_t diffWidthX= (widthFieldButton-widthButton)/2;

	if(ID_Slider>=MAX_NUMBER_SLIDER) return;
	switch(ID_Slider)
	{
	case SLIDER_ID1:
	case SLIDER_ID2:
	case SLIDER_ID3:
	case SLIDER_ID4:
	case SLIDER_ID5:
	case SLIDER_ID6:
	case SLIDER_ID7:
	case SLIDER_ID8:
	case SLIDER_ID9:
	case SLIDER_ID10:
	default:
		bkColor=DARKGRAY;		buttonFrameColor=WHITE;   buttonFillColor=0xBBBBBB;

		LCD_DrawRectangle    (xPos,             yPos,             widthFieldButton+10, heightFieldButton+1,  bkColor,          bkColor,0); //tlo ponizszego batona w calosci pokryty tym batonem
		LCD_DrawUpTriangle   (xPos+diffWidthX,  yPos+diffWidthY,  widthButton,		                       buttonFrameColor, buttonFillColor, bkColor);
		touchTemp.x_Start= xPos;
		touchTemp.y_Start= yPos;
		touchTemp.x_End= touchTemp.x_Start+widthFieldButton+1;
		touchTemp.y_End= touchTemp.y_Start+heightFieldButton+1;
		if(touchUpID>0)
			SetTouch(touchUpID);

		LCD_DrawRectangle    (xPos,             yPos+width-heightFieldButton,        	    widthFieldButton, heightFieldButton+1,  bkColor,          bkColor,0);
		LCD_DrawDownTriangle (xPos+diffWidthX,  yPos+width-heightFieldButton+diffWidthY,     widthButton, 	                        buttonFrameColor, buttonFillColor, bkColor);
		touchTemp.x_Start= xPos;
		touchTemp.y_Start= yPos+width-heightFieldButton;
		touchTemp.x_End= touchTemp.x_Start+widthFieldButton+1;
		touchTemp.y_End= touchTemp.y_Start+heightFieldButton+1;
		if(touchDownID>0)
			SetTouch(touchDownID);

		sliderTabParam[ID_Slider][5]=yPos+diffWidthY;  //up button
		sliderTabParam[ID_Slider][6]=yPos+width-heightFieldButton+diffWidthY;   //down button
		sliderTabParam[ID_Slider][7]=xPos+diffWidthX;
		sliderTabParam[ID_Slider][8]=widthButton;
		sliderTabParam[ID_Slider][9]=buttonFrameColor;
		sliderTabParam[ID_Slider][10]=buttonFillColor;
		sliderTabParam[ID_Slider][11]=bkColor;
		sliderTabParam[ID_Slider][12]=VERTICAL;
		sliderTabParam[ID_Slider][13]=touchSliderID;
		sliderTabParam[ID_Slider][14]=touchUpID;
		sliderTabParam[ID_Slider][15]=touchDownID;
	}

	LCD_DrawSlider_V(ID_Slider, xPos,yPos+heightFieldButton+spaceBetweenSliderButton, width-2*heightFieldButton-2*spaceBetweenSliderButton, height, value);
	if(touchSliderID>0)
		SetTouch(touchSliderID);
}

int LCD_Slider(int nrSlider,int x, int y)
{
	switch(sliderTabParam[nrSlider][12])
	{
	case HORIZONTAL:
		return LCD_DrawSliderIndirect_H( nrSlider,
													sliderTabParam[nrSlider][0],sliderTabParam[nrSlider][1],
													sliderTabParam[nrSlider][2],sliderTabParam[nrSlider][3],
													x-sliderTabParam[nrSlider][0]-sliderTabParam[nrSlider][4]/2 );

	case VERTICAL:
		return LCD_DrawSliderIndirect_V( nrSlider,
													sliderTabParam[nrSlider][0],sliderTabParam[nrSlider][1],
													sliderTabParam[nrSlider][2],sliderTabParam[nrSlider][3],
													y-sliderTabParam[nrSlider][0]-sliderTabParam[nrSlider][4]/2 );

	default:
		return 0;
	}
}

int LCD_SliderButton(int nrSlider,int percentVal)
{
	switch(sliderTabParam[nrSlider][12])
	{
	case HORIZONTAL:
		return LCD_DrawSliderIndirect_H( nrSlider,
													sliderTabParam[nrSlider][0],sliderTabParam[nrSlider][1],
													sliderTabParam[nrSlider][2],sliderTabParam[nrSlider][3],
													LCD_ReturnPositionSlider_H(nrSlider,percentVal)-sliderTabParam[nrSlider][0]-sliderTabParam[nrSlider][4]/2 );

	case VERTICAL:
		return LCD_DrawSliderIndirect_V( nrSlider,
													sliderTabParam[nrSlider][0],sliderTabParam[nrSlider][1],
													sliderTabParam[nrSlider][2],sliderTabParam[nrSlider][3],
													LCD_ReturnPositionSlider_V(nrSlider,percentVal)-sliderTabParam[nrSlider][0]-sliderTabParam[nrSlider][4]/2 );

	default:
		return 0;
	}
}

void vLCD_UpSliderTouchEffect(int nrSlider)
{
	LCD_UpSliderIndirect(nrSlider, RED, DARKYELLOW, 0x404040);
	osDelay(100);
	LCD_UpSliderIndirect(nrSlider, sliderTabParam[nrSlider][9], sliderTabParam[nrSlider][10], sliderTabParam[nrSlider][11]);
}

void vLCD_DownSliderTouchEffect(int nrSlider)
{
	LCD_DownSliderIndirect(nrSlider, RED, DARKYELLOW, 0x404040);
	osDelay(100);
	LCD_DownSliderIndirect(nrSlider, sliderTabParam[nrSlider][9], sliderTabParam[nrSlider][10], sliderTabParam[nrSlider][11]);
}

void vLCD_LeftSliderTouchEffect(int nrSlider)
{
	LCD_LeftSliderIndirect(nrSlider, RED, DARKYELLOW, 0x404040);
	osDelay(100);
	LCD_LeftSliderIndirect(nrSlider, sliderTabParam[nrSlider][9], sliderTabParam[nrSlider][10], sliderTabParam[nrSlider][11]);
}

void vLCD_RightSliderTouchEffect(int nrSlider)
{
	LCD_RightSliderIndirect(nrSlider, RED, DARKYELLOW, 0x404040);
	osDelay(100);
	LCD_RightSliderIndirect(nrSlider, sliderTabParam[nrSlider][9], sliderTabParam[nrSlider][10], sliderTabParam[nrSlider][11]);
}


#include "variables.h"
#include "float.h"  //ZMIANA nazwy boo nie wszystki flat funkcje!!!!

void LCD_DrawSliderWithButtons(int ID_Slider,int orient,int xPos,int yPos, int width,int height, int varNameID, uint16_t touchSliderID, uint16_t touchLeftID,uint16_t touchRightID)
{
	switch(orient)
	{
	case HORIZONTAL:
		LCD_DrawSliderWithButtons_H (ID_Slider, xPos,yPos, width,height, GetVal(varNameID), touchSliderID,touchLeftID,touchRightID);
		break;
	case VERTICAL:
		LCD_DrawSliderWithButtons_V (ID_Slider, xPos,yPos, width,height, GetVal(varNameID), touchSliderID,touchLeftID,touchRightID);
		break;
	}
	sliderTabParam[ID_Slider][16]=varNameID;
}

void ClearAllSlider(void)
{
	for(int i=0;i<MAX_NUMBER_SLIDER;++i)
	{
		sliderTabParam[i][13]=0;  //touchSlider
		sliderTabParam[i][14]=0; //touchSliderButton 1
		sliderTabParam[i][15]=0; //touchSliderButton 2
		sliderTabParam[i][16]=0; 		//valueID for slider
		sliderTabParam[i][17]=0xFFFF; //display txtIDvar of slider
	}
}

void LCD_TxtIDSlider(int ID_Slider, int fontID, int Xpos, int Ypos,int nrTxt)
{
	TxtID_Var(nrTxt,fontID, Xpos,Ypos, Int2DecStr(GetVal(sliderTabParam[ID_Slider][16])), halfHight,0);
	sliderTabParam[ID_Slider][17]=nrTxt;
}

int IsTouchSlider(int ID_Slider,int ID_touchDetect)
{
	if ((sliderTabParam[ID_Slider][13]==ID_touchDetect)||
		 (sliderTabParam[ID_Slider][14]==ID_touchDetect)||
		 (sliderTabParam[ID_Slider][15]==ID_touchDetect))
	{
		return 1;
	}
	return 0;
}

int TOUCH_SliderService(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y)
{
	if(ID_touchDetect){
	if(PRESSED==pressRelease)
	{
		for(int i=0;i<MAX_NUMBER_SLIDER;++i)
		{
			if(sliderTabParam[i][13]==ID_touchDetect)		//touchSlider
			{
				SetVal(sliderTabParam[i][16],LCD_Slider(i,x,y));
				if(sliderTabParam[i][17]<0xFFFF)
					LCD_Txt(sliderTabParam[i][17], int2perctStr(GetVal(sliderTabParam[i][16]),'0'), fontNoChange, halfHight,0);
				return 0;

			}
			else if(sliderTabParam[i][14]==ID_touchDetect)		//touchSliderButton 1
			{
				if(GetVal(sliderTabParam[i][16])>0)
				{
					DecrVal(sliderTabParam[i][16],1);
					LCD_SliderButton(i,GetVal(sliderTabParam[i][16]));
				}
				switch(sliderTabParam[i][12])
				{
				case HORIZONTAL:	vLCD_LeftSliderTouchEffect(i); break;
				case VERTICAL:		vLCD_UpSliderTouchEffect(i); break;
				}
				if(sliderTabParam[i][17]<0xFFFF)
					LCD_Txt(sliderTabParam[i][17], int2perctStr(GetVal(sliderTabParam[i][16]),'0'), fontNoChange, halfHight,0);
				return 1;

			}
			else if(sliderTabParam[i][15]==ID_touchDetect) 		//touchSliderButton 2
			{
				if (GetVal(sliderTabParam[i][16]) < 100)
				{
					IncrVal(sliderTabParam[i][16],1);
					LCD_SliderButton(i,GetVal(sliderTabParam[i][16]));
				}
				switch(sliderTabParam[i][12])
				{
				case HORIZONTAL:	vLCD_RightSliderTouchEffect(i); break;
				case VERTICAL:		vLCD_DownSliderTouchEffect(i); break;
				}
				if(sliderTabParam[i][17]<0xFFFF)
					LCD_Txt(sliderTabParam[i][17], int2perctStr(GetVal(sliderTabParam[i][16]),'0'), fontNoChange, halfHight,0);
				return 2;

			}
		}
	}}
	return -1;
}
