/*
 * fonts.c
 *
 *  Created on: 16.07.2020
 *      Author: Elektronika RM
 */

//Webdings:   ac ginqrsxy:;<Í1234567890
//Wingdings:  xyÊÍÒÛè
//Wingdings2:  88UUUVWXOPQRST£5
//Wingdings3:  abcdefghijklmnopqrstuvwDEHIJKLMNOPQRXYZ:;[]}<>'~*()&#^=_$%∞|?!•π∆Ê Í£≥—  strazalki

//UWAGA !!!!  -- Zmienne na max 40KB !!! --------- wiecej to powoli chodzπ wπtki http i https !!!!
//definiuj tyle pamieci ile trzeba, nadmiar zdefiniowanej powoduje ze http i https wolno chodzi !!!!

//R 255 120 120
//B 80  200 255
//G 80  255  80

#include <LCD_BasicGaphics.h>
#include <LCD_Hardware.h>
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include "LCD_Common.h"

#include "fonts_images.h"
#include "ff.h"
#include "SDCard.h"
#include "dbg.h"
#include "sdram.h"
#include "touch.h"
#include "mini_printf.h"

#define MAX_FONTS_AND_IMAGES_MEMORY_SIZE	0x600000

#define MAX_OPEN_FONTS_SIMULTANEOUSLY	 17
#define MAX_CHARS		256
#define POSITION_AND_WIDTH		2

#define MAX_OPEN_IMAGES_SIMULTANEOUSLY	 300
#define MAX_IMAGE_NAME_LEN		30

SDRAM static char fontsImagesMemoryBuffer[MAX_FONTS_AND_IMAGES_MEMORY_SIZE];

typedef struct{
	uint32_t size;
	uint32_t style;
	uint32_t bkColor;
	uint32_t color;
} ID_FONT;
static ID_FONT FontID[MAX_OPEN_FONTS_SIMULTANEOUSLY];

typedef struct{
	uint32_t fontSizeToIndex;
	uint32_t fontStyleToIndex;
	uint32_t fontBkColorToIndex;
	uint32_t fontColorToIndex;
	int fontsTabPos[MAX_CHARS][POSITION_AND_WIDTH];
	int height;
	int heightHalf;
	char *pointerToMemoryFont;
	uint32_t fontSdramLenght;
} FONTS_SETTING;
SRAM static FONTS_SETTING Font[MAX_OPEN_FONTS_SIMULTANEOUSLY];

typedef struct{
	uint16_t imagesNumber;
	uint16_t actualImage;
	portTickType everyTime;
} ANIMATION_SETTING;

typedef struct{
	char name[MAX_IMAGE_NAME_LEN];
	uint8_t *pointerToMemory;
	uint32_t sdramLenght;
	ANIMATION_SETTING Animation;
} IMAGES_SETTING;
SRAM static IMAGES_SETTING Image[MAX_OPEN_IMAGES_SIMULTANEOUSLY];

static uint32_t CounterBusyBytesForFontsImages=0;

void DisplayFontsStructState(void)
{
	for(int i=0; i < MAX_OPEN_FONTS_SIMULTANEOUSLY; i++){
		if(Font[i].fontSizeToIndex)
			//DbgSprintf(250,"\r\nIdx:%d  Font:%d  pointerToMemoryFont:0x%08x   fontSdramLenght:0x%06x-%d   ",i,Font[i].fontSizeToIndex-1, Font[i].pointerToMemoryFont, Font[i].fontSdramLenght, Font[i].fontSdramLenght);
			DbgSprintf(250,"\r\nIdx:%d  FontSize:%d  fontSdramLenght:%d",i,Font[i].fontSizeToIndex-1,Font[i].fontSdramLenght);
	}
	//DbgSprintf(250,"\r\nFONTS_SDRAM_ADDRESS: 0x%08x   MAX_FONTS_MEMORY_SIZE: 0x%x   CounterBusyBytesForFonts: 0x%06x\r\n",fontsImagesMemoryBuffer, MAX_FONTS_AND_IMAGES_MEMORY_SIZE, CounterBusyBytesForFontsImages);
	DbgSprintf(250,"\r\nMAX_FONTS_MEMORY_SIZE: 0x%x   CounterBusyBytesForFonts: 0x%06x\r\n",fontsImagesMemoryBuffer, MAX_FONTS_AND_IMAGES_MEMORY_SIZE, CounterBusyBytesForFontsImages);
}

void InfoForImagesFonts(void)
{
	DbgSprintf(250,"\r\nImages SDRAM size: 0x%08x\r\nCounterBusyBytesForFontsImages: 0x%08x\r\nStruct size Font+Image: %d+%d=%d  ",MAX_FONTS_AND_IMAGES_MEMORY_SIZE,CounterBusyBytesForFontsImages,sizeof(Font),sizeof(Image), sizeof(Font)+sizeof(Image));
}

static int LoadFontIndex(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
{
    int i;
    for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
    {
    	if(0==Font[i].fontSizeToIndex)
    	{
    		Font[i].fontSizeToIndex = fontSize+1;
    		Font[i].fontStyleToIndex = fontStyle;
    		Font[i].fontBkColorToIndex = backgroundColor;
    		Font[i].fontColorToIndex = fontColor;
    		return i;
    	}
    }
    return -1;
}

static int LoadImageName(char *imageName)
{
    int i;
    for(i=0; i<MAX_OPEN_IMAGES_SIMULTANEOUSLY; i++)
    {
    	if(0==Image[i].name[0])
    	{
    		strcpy(Image[i].name,imageName);
    		return i;
    	}
    }
    return -1;
}

static int SearchFontIndex(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
{
    int i;
    for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
    {
    	if(((fontSize+1)	==	Font[i].fontSizeToIndex)&&
    		(fontStyle	    ==	Font[i].fontStyleToIndex)&&
			(backgroundColor==	Font[i].fontBkColorToIndex)&&
			(fontColor	    ==	Font[i].fontColorToIndex))
    		return i;
    }
    return -1;
}

int ReturnWidthFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, char *txt)
{
	int lenTxtInPixel=0;
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return -1;
	else
	{
		for(int i=0;i<strlen(txt);i++)
		{
			lenTxtInPixel += Font[fontIndex].fontsTabPos[ (int)txt[i] ][1];
		}
		return lenTxtInPixel;
	}
}

int LCD_GetFontID_size(int fontID)
{
	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
		return FontID[fontID].size;
	else
		return 0;
}
int LCD_GetFontID_style(int fontID)
{
	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
		return FontID[fontID].style;
	else
		return 0;
}
int LCD_GetFontID_bkColor(int fontID)
{
	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
		return FontID[fontID].bkColor;
	else
		return 0;
}
int LCD_GetFontID_color(int fontID)
{
	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
		return FontID[fontID].color;
	else
		return 0;
}
int LCD_GetFontID(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
{
    int i;
    for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
    {
    	if((fontSize		==	FontID[i].size)&&
    	   (fontStyle	    ==	FontID[i].style)&&
		   (backgroundColor	==	FontID[i].bkColor)&&
		   (fontColor	    ==	FontID[i].color))
    			return i;
    }
    return -1;
}

int ReturnHeightFont(int fontSize,int fontStyle,uint32_t backgroundColor,uint32_t fontColor)
{
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return -1;
	else
		return Font[fontIndex].height;
}

int ReturnWidthFontID(int fontID, char *txt)
{
	int lenTxtInPixel=0;
	int fontIndex=SearchFontIndex(LCD_GetFontID_size(fontID), LCD_GetFontID_style(fontID), LCD_GetFontID_bkColor(fontID), LCD_GetFontID_color(fontID));
	if(fontIndex==-1)
		return -1;
	else
	{
		for(int i=0;i<strlen(txt);i++)
		{
			lenTxtInPixel += Font[fontIndex].fontsTabPos[ (int)txt[i] ][1];
		}
		return lenTxtInPixel;
	}
}
int ReturnHeightFontID(int fontID)
{
	int fontIndex=SearchFontIndex(LCD_GetFontID_size(fontID), LCD_GetFontID_style(fontID), LCD_GetFontID_bkColor(fontID), LCD_GetFontID_color(fontID));
	if(fontIndex==-1)
		return -1;
	else
		return Font[fontIndex].height;
}
int ReturnHeightHalfFontID(int fontID)
{
	int fontIndex=SearchFontIndex(LCD_GetFontID_size(fontID), LCD_GetFontID_style(fontID), LCD_GetFontID_bkColor(fontID), LCD_GetFontID_color(fontID));
	if(fontIndex==-1)
		return -1;
	else
		return Font[fontIndex].heightHalf;
}

static int SearchImageName(char *imageName)
{
    int i;
    for(i=0; i<MAX_OPEN_IMAGES_SIMULTANEOUSLY; i++)
    {
    	if(strstr(Image[i].name,imageName))
    		return i;
    }
    return -1;
}

static uint8_t ReturnFontSize(int fontIndex)
{
	return Font[fontIndex].fontSizeToIndex-1;
}

static void SetFontHeightHalf(int fontIndex, int heightHalf)
{
	Font[fontIndex].heightHalf = heightHalf;
}

//static char* ReturnImageName(int imageIndex)
//{
//	return Image[imageIndex].name;
//}

static bool DynamicFontMemoryAllocation(uint32_t fontFileSize, int fontIndex)
{
	if(CounterBusyBytesForFontsImages+fontFileSize < MAX_FONTS_AND_IMAGES_MEMORY_SIZE)
	{
		Font[fontIndex].fontSdramLenght=fontFileSize;
		Font[fontIndex].pointerToMemoryFont=(char*)( fontsImagesMemoryBuffer + CounterBusyBytesForFontsImages );
		CounterBusyBytesForFontsImages += fontFileSize;
		return true;
	}
	else
		return false;
}

static bool DynamicImageMemoryAllocation(uint32_t imageFileSize, int imageIndex)
{
	if(CounterBusyBytesForFontsImages+imageFileSize < MAX_FONTS_AND_IMAGES_MEMORY_SIZE)
	{
		Image[imageIndex].sdramLenght=imageFileSize;
		Image[imageIndex].pointerToMemory=(uint8_t*)( fontsImagesMemoryBuffer + CounterBusyBytesForFontsImages );
		CounterBusyBytesForFontsImages += imageFileSize;
		return true;
	}
	else
		return false;
}

void LCD_DeleteAllFontAndImages(void)
{
	int i;
	CounterBusyBytesForFontsImages=0;
	for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
	{
		Font[i].fontSizeToIndex=0;
		Font[i].fontStyleToIndex=0;
		Font[i].fontBkColorToIndex=0;
		Font[i].fontColorToIndex=0;
		Font[i].pointerToMemoryFont=0;
		Font[i].fontSdramLenght=0;
	}
	for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
	{
		FontID[i].size=0;
		FontID[i].style=0;
		FontID[i].bkColor=0;
		FontID[i].color=0;
	}
	for(i=0; i<MAX_OPEN_IMAGES_SIMULTANEOUSLY; i++)
	{
		Image[i].name[0]=0;
		Image[i].pointerToMemory=0;
		Image[i].sdramLenght=0;
	}
}

bool LCD_DeleteFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
{
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return false;
	Font[fontIndex].fontSizeToIndex=0;
	Font[fontIndex].fontStyleToIndex=0;
	Font[fontIndex].fontBkColorToIndex=0;
	Font[fontIndex].fontColorToIndex=0;
	Font[fontIndex].pointerToMemoryFont=0;
	if( CounterBusyBytesForFontsImages >= Font[fontIndex].fontSdramLenght )
	{
		CounterBusyBytesForFontsImages -= Font[fontIndex].fontSdramLenght;
		Font[fontIndex].fontSdramLenght=0;
		return true;
	}
	else
	{
		CounterBusyBytesForFontsImages=0;
		Font[fontIndex].fontSdramLenght=0;
		return false;
	}
}

bool LCD_DeleteImage(char *imageName)
{
	int imageIndex=SearchImageName(imageName);
	if(imageIndex==-1)
		return false;
	Image[imageIndex].name[0]=0;
	Image[imageIndex].pointerToMemory=0;
	if( CounterBusyBytesForFontsImages >= Image[imageIndex].sdramLenght )
	{
		CounterBusyBytesForFontsImages -= Image[imageIndex].sdramLenght;
		Image[imageIndex].sdramLenght=0;
		return true;
	}
	else
	{
		CounterBusyBytesForFontsImages=0;
		Image[imageIndex].sdramLenght=0;
		return false;
	}
}

static void SearchFontsField(char *pbmp, uint32_t width, uint32_t height, uint32_t bit_pixel, uint32_t *shiftXpos, uint8_t *backGround)
{
	int i,j,k;
	char *pbmp1;
	j=0;
	do
	{
		pbmp1=pbmp+3*(j+*shiftXpos);
		k=0;
		for(i=0; i < height; i++)
		{
			if((*(pbmp1+0)==backGround[0])&&(*(pbmp1+1)==backGround[1])&&(*(pbmp1+2)==backGround[2]))
				k++;
			else
				break;
			pbmp1 -= width*bit_pixel;
		}
		if(k!=height)
		{
			*shiftXpos+=j;
			break;
		}
		j++;
	}while(1);
}

static uint32_t CountCharLenght(char *pbmp, uint32_t width, uint32_t height, uint32_t bit_pixel, uint32_t *shiftXpos, uint8_t *backGround)
{
	int i,j,k;
	char *pbmp1;
	uint32_t charLenght=0;
	j=0;
	do
	{
		pbmp1=pbmp+3*(j+*shiftXpos);
		k=0;
		for(i=0; i < height; i++)
		{
			if((*(pbmp1+0)==backGround[0])&&(*(pbmp1+1)==backGround[1])&&(*(pbmp1+2)==backGround[2]))
				k++;
			else
			{
				k=0;
				break;
			}
			pbmp1 -= width*bit_pixel;
		}
		if(k==height)
		{
			charLenght+=j;
			break;
		}
		j++;
	}while(1);
	return charLenght;
}

static int CountHalfHeightForDot(char *pbmp, uint32_t width, uint32_t height, uint32_t bit_pixel, uint32_t shiftXpos, uint8_t *backGround)
{
	int i,j=0,m=0;
	char *pbmp1;

		pbmp1=pbmp+3*(j+shiftXpos);
		m=0;
		for(i=0; i < height; i++)
		{
			if((*(pbmp1+0)==backGround[0])&&(*(pbmp1+1)==backGround[1])&&(*(pbmp1+2)==backGround[2]))
			{
				if(m==1) return i;
			}
			else
			{
				 m=1;
			}
			pbmp1 -= width*bit_pixel;
		}

	return -1;
}

static void SearchCurrentFont_TablePos(char *pbmp, int fontIndex, int constWidth)
{
	char tabChar[256];
	uint8_t fontSize=ReturnFontSize(fontIndex);

	if((FONT_72==fontSize)||(FONT_72_bold==fontSize)||(FONT_72_italics==fontSize))
		strcpy(tabChar,"+-.,[]{}<>'~*()=%∞|?/!1234567890");
	else if(FONT_36_dings2==fontSize)
		strcpy(tabChar,"abcdefghijklmnopq");
	else if((FONT_130==fontSize)||(FONT_130_bold==fontSize))
		strcpy(tabChar,"+-1234567890.");
	else
		strcpy(tabChar,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-.,:;[]{}<>'~*()&#^=_$%∞@|?!•π∆Ê Í£≥—Ò”ÛåúèüØø/1234567890"); //dax do potegi 2 i 3 omega np m3 m2 !!!!!!

	int j, lenTab=strlen(tabChar);
	uint32_t shiftXpos=0, index = 0, width = 0, height = 0, bit_pixel = 0;
	uint8_t backGround[3];

	/* Get bitmap data address offset */
	index = pbmp[10] + (pbmp[11] << 8) + (pbmp[12] << 16)  + (pbmp[13] << 24);

	/* Read bitmap width */
	width = pbmp[18] + (pbmp[19] << 8) + (pbmp[20] << 16)  + (pbmp[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = pbmp[22] + (pbmp[23] << 8) + (pbmp[24] << 16)  + (pbmp[25] << 24);

	/* Read bit/pixel */
	bit_pixel = pbmp[28] + (pbmp[29] << 8);
	bit_pixel/=8;

	pbmp += (index + (width * height * bit_pixel));
	pbmp -= width*bit_pixel;

	backGround[0]=pbmp[0];
	backGround[1]=pbmp[1];
	backGround[2]=pbmp[2];

	Font[fontIndex].height=height;

	for(j=0; j < lenTab; j++)
	{
		SearchFontsField(pbmp,width,height,bit_pixel,&shiftXpos,backGround);
		if(tabChar[j]=='.')  SetFontHeightHalf(fontIndex, CountHalfHeightForDot(pbmp,width,height,bit_pixel,shiftXpos,backGround)+2);
		Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][1] = CountCharLenght(pbmp,width,height,bit_pixel,&shiftXpos,backGround);
		Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][0] = shiftXpos;
		shiftXpos += (Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][1]+1);
		if(j==0)
			Font[fontIndex].fontsTabPos[(int)' '][1] = (2*Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][1])/3;
	}
	Font[fontIndex].fontsTabPos[(int)' '][0] = shiftXpos;  //spacja na podstawie 2/3 d≥ugosci pierwszej litery z tabChar

	if(constWidth) //cyfry muszπ miec stala szerokosc
	{
		int maxWidth=0;
		for(j=0; j < lenTab; j++)
		{
			if(((tabChar[j]>0x2F)&&(tabChar[j]<0x3A))||(tabChar[j]==':')||(tabChar[j]=='-'))
			{
				if(Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][1] > maxWidth)  maxWidth = Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][1];
			}
		}
		for(j=0; j < lenTab; j++)
		{
			if(((tabChar[j]>0x2F)&&(tabChar[j]<0x3A))||(tabChar[j]==':')||(tabChar[j]=='-'))
			{
				Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][0] -= (maxWidth - Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][1])/2;
				Font[fontIndex].fontsTabPos[ (int)tabChar[j] ][1] = maxWidth;
			}
		}
		Font[fontIndex].fontsTabPos[(int)' '][1]=maxWidth;
	}
}

int LCD_LoadFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, uint32_t fontID, int constWidth)
{
	uint32_t fontFileSize, readBytesSize;	int resultSearch;

	resultSearch=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(-1!=resultSearch)
	{
		if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
		{
			FontID[fontID].size = fontSize;
			FontID[fontID].style = fontStyle;
			FontID[fontID].bkColor = backgroundColor;
			FontID[fontID].color = fontColor;
		}
		return 1;
	}
	int fontIndex=LoadFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(-1==fontIndex)
		return 2;
	char fileOpenName[100]="Fonts/";

	switch(backgroundColor)
	{
	case DARKGRAY:
		strncat(fileOpenName,"BackGround_darkGray/",20);
		break;
	case BLACK:
		strncat(fileOpenName,"BackGround_black/",17);
		break;
	case BROWN:
		strncat(fileOpenName,"BackGround_brown/",17);
		break;
	case WHITE:
		strncat(fileOpenName,"BackGround_white/",17);
		break;
	default:
		break;
	}

	switch(fontColor)
	{
	case WHITE:
		strncat(fileOpenName,"Color_white/",12);
		break;
	case BLUE:
		strncat(fileOpenName,"Color_blue/",11);
		break;
	case RED:
		strncat(fileOpenName,"Color_red/",10);
		break;
	case GREEN:
		strncat(fileOpenName,"Color_green/",12);
		break;
	case BLACK:
		strncat(fileOpenName,"Color_black/",12);
		break;
	default:
		break;
	}

	switch(fontStyle)
	{
	case Arial_Narrow:
		strncat(fileOpenName,"Arial_Narrow/",13);
		break;
	case Calibri_Light:
		strncat(fileOpenName,"Calibri_Light/",14);
		break;
	case Bodoni_MT_Condenset:
		strncat(fileOpenName,"Bodoni_MT_Condenset/",20);
		break;
	case Comic_Saens_MS:
		strncat(fileOpenName,"Comic_Saens_MS/",15);
		break;
	case Dings_2:
		strncat(fileOpenName,"Dings_2/",8);
		break;
	case Arial:
		strncat(fileOpenName,"Arial/",6);
		break;
	default:
		break;
	}

	switch(fontSize)
	{
	case FONT_8:
		strncat(fileOpenName,"font_8.bmp",10);
		break;
	case FONT_9:
		strncat(fileOpenName,"font_9.bmp",10);
		break;
	case FONT_10:
		strncat(fileOpenName,"font_10.bmp",11);
		break;
	case FONT_11:
		strncat(fileOpenName,"font_11.bmp",11);
		break;
	case FONT_12:
		strncat(fileOpenName,"font_12.bmp",11);
		break;
	case FONT_14:
		strncat(fileOpenName,"font_14.bmp",11);
		break;
	case FONT_16:
		strncat(fileOpenName,"font_16.bmp",11);
		break;
	case FONT_18:
		strncat(fileOpenName,"font_18.bmp",11);
		break;
	case FONT_20:
		strncat(fileOpenName,"font_20.bmp",11);
		break;
	case FONT_22:
		strncat(fileOpenName,"font_22.bmp",11);
		break;
	case FONT_24:
		strncat(fileOpenName,"font_24.bmp",11);
		break;
	case FONT_26:
		strncat(fileOpenName,"font_26.bmp",11);
		break;
	case FONT_28:
		strncat(fileOpenName,"font_28.bmp",11);
		break;
	case FONT_36:
		strncat(fileOpenName,"font_36.bmp",11);
		break;
	case FONT_48:
		strncat(fileOpenName,"font_48.bmp",11);
		break;
	case FONT_72:
		strncat(fileOpenName,"font_72.bmp",11);
		break;
	case FONT_130:
		strncat(fileOpenName,"font_130.bmp",12);
		break;
	case FONT_8_bold:
		strncat(fileOpenName,"font_8_bold.bmp",15);
		break;
	case FONT_9_bold:
		strncat(fileOpenName,"font_9_bold.bmp",15);
		break;
	case FONT_10_bold:
		strncat(fileOpenName,"font_10_bold.bmp",16);
		break;
	case FONT_11_bold:
		strncat(fileOpenName,"font_11_bold.bmp",16);
		break;
	case FONT_12_bold:
		strncat(fileOpenName,"font_12_bold.bmp",16);
		break;
	case FONT_14_bold:
		strncat(fileOpenName,"font_14_bold.bmp",16);
		break;
	case FONT_16_bold:
		strncat(fileOpenName,"font_16_bold.bmp",16);
		break;
	case FONT_18_bold:
		strncat(fileOpenName,"font_18_bold.bmp",16);
		break;
	case FONT_20_bold:
		strncat(fileOpenName,"font_20_bold.bmp",16);
		break;
	case FONT_22_bold:
		strncat(fileOpenName,"font_22_bold.bmp",16);
		break;
	case FONT_24_bold:
		strncat(fileOpenName,"font_24_bold.bmp",16);
		break;
	case FONT_26_bold:
		strncat(fileOpenName,"font_26_bold.bmp",16);
		break;
	case FONT_28_bold:
		strncat(fileOpenName,"font_28_bold.bmp",16);
		break;
	case FONT_36_bold:
		strncat(fileOpenName,"font_36_bold.bmp",16);
		break;
	case FONT_48_bold:
		strncat(fileOpenName,"font_48_bold.bmp",16);
		break;
	case FONT_72_bold:
		strncat(fileOpenName,"font_72_bold.bmp",16);
		break;
	case FONT_130_bold:
		strncat(fileOpenName,"font_130_bold.bmp",17);
		break;
	case FONT_8_italics:
		strncat(fileOpenName,"font_8_italics.bmp",18);
		break;
	case FONT_9_italics:
		strncat(fileOpenName,"font_9_italics.bmp",18);
		break;
	case FONT_10_italics:
		strncat(fileOpenName,"font_10_italics.bmp",19);
		break;
	case FONT_11_italics:
		strncat(fileOpenName,"font_11_italics.bmp",19);
		break;
	case FONT_12_italics:
		strncat(fileOpenName,"font_12_italics.bmp",19);
		break;
	case FONT_14_italics:
		strncat(fileOpenName,"font_14_italics.bmp",19);
		break;
	case FONT_16_italics:
		strncat(fileOpenName,"font_16_italics.bmp",19);
		break;
	case FONT_18_italics:
		strncat(fileOpenName,"font_18_italics.bmp",19);
		break;
	case FONT_20_italics:
		strncat(fileOpenName,"font_20_italics.bmp",19);
		break;
	case FONT_22_italics:
		strncat(fileOpenName,"font_22_italics.bmp",19);
		break;
	case FONT_24_italics:
		strncat(fileOpenName,"font_24_italics.bmp",19);
		break;
	case FONT_26_italics:
		strncat(fileOpenName,"font_26_italics.bmp",19);
		break;
	case FONT_28_italics:
		strncat(fileOpenName,"font_28_italics.bmp",19);
		break;
	case FONT_36_italics:
		strncat(fileOpenName,"font_36_italics.bmp",19);
		break;
	case FONT_48_italics:
		strncat(fileOpenName,"font_48_italics.bmp",19);
		break;
	case FONT_72_italics:
		strncat(fileOpenName,"font_72_italics.bmp",19);
		break;
	case FONT_36_dings2:
		strncat(fileOpenName,"font_36_dings2.bmp",18);
		break;
	default:
		break;
	}

	if(FR_OK!=SDCardFileInfo(fileOpenName,&fontFileSize))
		return 3;

	while((fontFileSize%4)!=0)  //wyrÛwnanie do 4bajtÛw dla odczytu SDRAM
		fontFileSize++;

	if(true==DynamicFontMemoryAllocation( fontFileSize, fontIndex) )
	{
		if(FR_OK!=SDCardFileOpen(1,fileOpenName,FA_READ))
			return 4;
		if(FR_OK!=SDCardFileRead(1,Font[fontIndex].pointerToMemoryFont,MAX_FONTS_AND_IMAGES_MEMORY_SIZE,&readBytesSize))
			return 5;
		if(FR_OK!=SDCardFileClose(1))
			return 6;
		SearchCurrentFont_TablePos(Font[fontIndex].pointerToMemoryFont, fontIndex, constWidth);
	}
	else
		return 7;

	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
	{
		FontID[fontID].size = fontSize;
		FontID[fontID].style = fontStyle;
		FontID[fontID].bkColor = backgroundColor;
		FontID[fontID].color = fontColor;
	}

	return 0;
}

int LCD_LoadImage(char *pathName)
{
  uint32_t imageFileSize, readBytesSize;
  int imageIndex;

  if(-1!=SearchImageName(pathName))
	return 1;
  imageIndex=LoadImageName(pathName);
  if(-1==imageIndex)
	return 2;

  if(FR_OK!=SDCardFileInfo(pathName,&imageFileSize))
    return 3;

  while((imageFileSize%4)!=0)  //wyrÛwnanie do 4bajtÛw dla odczytu SDRAM
	  imageFileSize++;

  if(true==DynamicImageMemoryAllocation(imageFileSize,imageIndex))
  {
    if(FR_OK!=SDCardFileOpen(1,pathName,FA_READ))
	  return 4;
	if(FR_OK!=SDCardFileRead(1,(char*)Image[imageIndex].pointerToMemory,MAX_FONTS_AND_IMAGES_MEMORY_SIZE,&readBytesSize))
	  return 5;
	if(FR_OK!=SDCardFileClose(1))
	  return 6;
  }
  else
  {
	  Image[imageIndex].name[0]=0;
	  Image[imageIndex].pointerToMemory=0;
	  return 7;
  }
  return 0;
}

//bool LCD_DeleteAnimation(char *folderPathName)
//{
//
//}

//int LCD_LoadAnimation(char *folderPathName, int fromImage, int imagesNumber, int everyImage)
//{
//	char path[100];
//	int result,j=everyImage,nrImage=0,i;
//	for(i=(fromImage-1); i<(imagesNumber*everyImage); ++i)
//	{
//		if(j==everyImage)
//		{
//			j=0;
//			nrImage++;
//			mini_snprintf(path,100,"%s/%d.bmp",folderPathName,i+1);
//			result=LCD_LoadImage(path);
//			if(0!=result)
//				return result;
//		}
//		j++;
//	}
//	mini_snprintf(path,100,"%s/%d.bmp",folderPathName,fromImage);
//	i=SearchImageName(path);
//	if(i!=0)
//		Image[i].Animation.numberImage=nrImage;
//	return 0;
//}

//void Test_LoadFonts(int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
//{
//	//LCD_LoadFont(FONT_8, fontStyle, backgroundColor, fontColor);
//	//LCD_LoadFont(FONT_9, fontStyle, backgroundColor, fontColor);
//	//LCD_LoadFont(FONT_10, fontStyle, backgroundColor, fontColor);
//	//LCD_LoadFont(FONT_11, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_12, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_14, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_16, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_18, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_20, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_22, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_24, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_26, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_28, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_36, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_48, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_72, fontStyle, backgroundColor, fontColor);
//}
//
//void Test_LoadBoldFonts(int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
//{
//	LCD_LoadFont(FONT_8_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_9_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_10_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_11_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_12_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_14_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_16_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_18_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_20_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_22_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_24_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_26_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_28_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_36_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_48_bold, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_72_bold, fontStyle, backgroundColor, fontColor);
//}
//
//void Test_LoadItalicsFonts(int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
//{
//	LCD_LoadFont(FONT_8_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_9_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_10_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_11_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_12_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_14_italics, fontStyle, backgroundColor, fontColor);
//    LCD_LoadFont(FONT_16_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_18_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_20_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_22_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_24_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_26_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_28_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_36_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_48_italics, fontStyle, backgroundColor, fontColor);
//	LCD_LoadFont(FONT_72_italics, fontStyle, backgroundColor, fontColor);
//}

bool LCD_DrawTxtIndirect_Buff(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int X, int Y, char *txt, char *LcdBuffer, int OnlyDigits, int space)
{
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return false;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,lenTxt=strlen(txt);
	int posX=0;
	int spaceCorect=0;
	char *pbmp;
	int spaceBetweenLatters=space;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint8_t backGround[3];
	uint32_t pos, pos2, xi;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);

	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround[0]=fontsBuffer[0];
	backGround[1]=fontsBuffer[1];
	backGround[2]=fontsBuffer[2];

	int widthInPixel=0;
	for(n=0;n<lenTxt;n++)
		widthInPixel += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1]+spaceBetweenLatters;

	if(OnlyDigits==halfHight) height=Font[fontIndex].heightHalf;

	for(n=0;n<lenTxt;n++)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= 3*posX;

		for(j=0; j < height; j++)
		{
			pos=pos2+3*widthInPixel*j;
			xi=3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

			for(i=0; i<xi; i++)
			{
				if(spaceCorect != 0)
				{
					if( (LcdBuffer[pos+i]   == backGround[0])&&
						(LcdBuffer[pos+i+1] == backGround[1])&&
						(LcdBuffer[pos+i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
					else
						i+=2;
				}
				else
				{
					 LcdBuffer[pos+i] = *(pbmp+i);
					 i++;
					 LcdBuffer[pos+i] = *(pbmp+i);
					 i++;
					 LcdBuffer[pos+i] = *(pbmp+i);
				}
			}
			if(spaceBetweenLatters>0)
			{
				for(int sbi=0; sbi<3*spaceBetweenLatters; ++sbi)
				{
					LcdBuffer[pos+i+sbi] =backGround[0];  sbi++;
					LcdBuffer[pos+i+sbi] =backGround[1];  sbi++;
					LcdBuffer[pos+i+sbi] =backGround[2];
				}
			}
			pbmp -= width*bit_pixel;
		}
		posX += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

//		if(FONT_48==fontSize)
//		{
//			if((txt[n]=='a')&&(txt[n+1]=='≥')){			spaceCorect=1;		spaceBetweenLatters=-4;		}
//			else if((txt[n]=='i')&&(txt[n+1]=='e')){	spaceCorect=1;		spaceBetweenLatters=8;	}
//			else if((txt[n]=='e')&&(txt[n+1]=='l')){	spaceCorect=1;		spaceBetweenLatters=4;		}
//			else{										spaceCorect=0;		spaceBetweenLatters=0;		}
//
//		}

		posX+=spaceBetweenLatters;

	}
	BSP_LCD_DrawXXX((uint32_t)X,(uint32_t)Y,(uint32_t)posX,(uint32_t)height,(uint8_t*)LcdBuffer);
	return true;
}

bool LCD_DrawTxtIndirect_Buff_color(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int X, int Y, char *txt, char *LcdBuffer, int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int space)
{
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return false;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,lenTxt=strlen(txt);
	int posX=0;
	int spaceCorect=0;
	char *pbmp;
	int spaceBetweenLatters=space;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint8_t backGround[3];
	uint32_t pos, pos2, xi;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);

	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround[0]=fontsBuffer[0];
	backGround[1]=fontsBuffer[1];
	backGround[2]=fontsBuffer[2];

	int widthInPixel=0;
	for(n=0;n<lenTxt;n++)
		widthInPixel += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1]+spaceBetweenLatters;

	if(OnlyDigits==halfHight) height=Font[fontIndex].heightHalf;

	for(n=0;n<lenTxt;n++)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= 3*posX;

		for(j=0; j < height; j++)
		{
			pos=pos2+3*widthInPixel*j;
			xi=3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

			for(i=0; i<xi; i++)
			{
				if(spaceCorect != 0)
				{
					if( (LcdBuffer[pos+i]   == backGround[0])&&
						(LcdBuffer[pos+i+1] == backGround[1])&&
						(LcdBuffer[pos+i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
					else
						i+=2;
				}
				else
				{

					if( (pbmp[i]   == backGround[0])&&
						(pbmp[i+1] == backGround[1])&&
						(pbmp[i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = bkGroundColor;
						 i++;
						 LcdBuffer[pos+i] = bkGroundColor>>8;
						 i++;
						 LcdBuffer[pos+i] = bkGroundColor>>16;

					}

					else
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
				}
			}
			if(spaceBetweenLatters>0)
			{
				for(int sbi=0; sbi<3*spaceBetweenLatters; ++sbi)
				{
					LcdBuffer[pos+i+sbi] =bkGroundColor;     sbi++;
					LcdBuffer[pos+i+sbi] =bkGroundColor>>8;  sbi++;
					LcdBuffer[pos+i+sbi] =bkGroundColor>>16;
				}
			}
			pbmp -= width*bit_pixel;
		}
		posX += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

//		if(FONT_48==fontSize)
//		{
//			if((txt[n]=='a')&&(txt[n+1]=='≥')){			spaceCorect=1;		spaceBetweenLatters=-4;		}
//			else if((txt[n]=='i')&&(txt[n+1]=='e')){	spaceCorect=1;		spaceBetweenLatters=8;	}
//			else if((txt[n]=='e')&&(txt[n+1]=='l')){	spaceCorect=1;		spaceBetweenLatters=4;		}
//			else{										spaceCorect=0;		spaceBetweenLatters=0;		}
//
//		}

		posX+=spaceBetweenLatters;

	}
	BSP_LCD_DrawXXX((uint32_t)X,(uint32_t)Y,(uint32_t)posX,(uint32_t)height,(uint8_t*)LcdBuffer);
	return true;
}

bool LCD_DrawTxt_Buff(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer,int OnlyDigits, int space)
{
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return false;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,lenTxt=strlen(txt);
	int posX=X, posY=Y;
	int spaceCorect=0;
	char *pbmp;
	int spaceBetweenLatters=space;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint8_t backGround[3];
	uint32_t pos, pos2, xi;

	touchTemp.x_Start= posX;
	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);

	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround[0]=fontsBuffer[0];
	backGround[1]=fontsBuffer[1];
	backGround[2]=fontsBuffer[2];

	if(OnlyDigits==halfHight) height=Font[fontIndex].heightHalf;

	for(n=0;n<lenTxt;n++)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= 3*(800*posY + posX);

		for(j=0; j < height; j++)
		{
			pos=pos2+3*800*j;
			xi=3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

			for(i=0; i<xi; i++)
			{
				if(spaceCorect != 0)
				{
					if( (LcdBuffer[pos+i]   == backGround[0])&&
						(LcdBuffer[pos+i+1] == backGround[1])&&
						(LcdBuffer[pos+i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
					else
						i+=2;
				}
				else
				{
//					if( (pbmp[i]   != backGround[0])&&
//						(pbmp[i+1] != backGround[1])&&
//						(pbmp[i+2] != backGround[2]) )
//					{
//						 LcdBuffer[pos+i] = 0xFF;
//						 i++;
//						 LcdBuffer[pos+i] = 0xFF;
//						 i++;
//						 LcdBuffer[pos+i] = 0xFF;
//
//					}
//					else
//					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					//}

				}
			}
			pbmp -= width*bit_pixel;
		}
		posX += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

//		if(FONT_48==fontSize)
//		{
//			if((txt[n]=='a')&&(txt[n+1]=='≥')){			spaceCorect=1;		spaceBetweenLatters=-4;		}
//			else if((txt[n]=='i')&&(txt[n+1]=='e')){	spaceCorect=1;		spaceBetweenLatters=8;	}
//			else if((txt[n]=='e')&&(txt[n+1]=='l')){	spaceCorect=1;		spaceBetweenLatters=4;		}
//			else{										spaceCorect=0;		spaceBetweenLatters=0;		}
//
//		}

		posX+=spaceBetweenLatters;

		if(posX + Font[fontIndex].fontsTabPos[(int)'W'][1] > LCD_GetXSize() )// tu dla FONT72 NIE mAAA BLAD !!!!! do poprawki
		{
			posX=X;
			posY+=height;
		}
	}
	touchTemp.x_End= posX;
	touchTemp.y_End= posY+height;
	return true;
}

bool LCD_DrawTxt_BufferWindow(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer,int OnlyDigits, int widthWindow)
{
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return false;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,lenTxt=strlen(txt);
	int posX=X, posY=Y;
	int spaceCorect=0;
	char *pbmp;
	int spaceBetweenLatters=0;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint8_t backGround[3];
	uint32_t pos, pos2, xi;

	touchTemp.x_Start= posX;
	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);

	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround[0]=fontsBuffer[0];
	backGround[1]=fontsBuffer[1];
	backGround[2]=fontsBuffer[2];

	if(OnlyDigits==halfHight) height=Font[fontIndex].heightHalf;

	for(n=0;n<lenTxt;n++)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= 3*(widthWindow*posY + posX);

		for(j=0; j < height; j++)
		{
			pos=pos2+3*widthWindow*j;
			xi=3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

			for(i=0; i<xi; i++)
			{
				if(spaceCorect != 0)
				{
					if( (LcdBuffer[pos+i]   == backGround[0])&&
						(LcdBuffer[pos+i+1] == backGround[1])&&
						(LcdBuffer[pos+i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
					else
						i+=2;
				}
				else
				{
//					if( (pbmp[i]   != backGround[0])&&
//						(pbmp[i+1] != backGround[1])&&
//						(pbmp[i+2] != backGround[2]) )
//					{
//						 LcdBuffer[pos+i] = 0xFF;
//						 i++;
//						 LcdBuffer[pos+i] = 0xFF;
//						 i++;
//						 LcdBuffer[pos+i] = 0xFF;
//
//					}
//					else
//					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					//}

				}
			}
			pbmp -= width*bit_pixel;
		}
		posX += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

//		if(FONT_48==fontSize)
//		{
//			if((txt[n]=='a')&&(txt[n+1]=='≥')){			spaceCorect=1;		spaceBetweenLatters=-4;		}
//			else if((txt[n]=='i')&&(txt[n+1]=='e')){	spaceCorect=1;		spaceBetweenLatters=8;	}
//			else if((txt[n]=='e')&&(txt[n+1]=='l')){	spaceCorect=1;		spaceBetweenLatters=4;		}
//			else{										spaceCorect=0;		spaceBetweenLatters=0;		}
//
//		}

		posX+=spaceBetweenLatters;

//		if(posX + Font[fontIndex].fontsTabPos[(int)'W'][1] > widthWindow )// tu dla FONT72 NIE mAAA BLAD !!!!! do poprawki
//		{
//			posX=X;
//			posY+=height;
//		}
	}
	touchTemp.x_End= posX;
	touchTemp.y_End= posY+height;
	return true;
}

bool LCD_DrawTxt_Buff_color(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer,int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int space)
{
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return false;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,lenTxt=strlen(txt);
	int posX=X, posY=Y;
	int spaceCorect=0;
	char *pbmp;
	int spaceBetweenLatters=space;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint8_t backGround[3];
	uint32_t pos, pos2, xi;

	touchTemp.x_Start= posX;
	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);

	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround[0]=fontsBuffer[0];
	backGround[1]=fontsBuffer[1];
	backGround[2]=fontsBuffer[2];

	if(OnlyDigits==halfHight) height=Font[fontIndex].heightHalf;

	for(n=0;n<lenTxt;n++)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= 3*(800*posY + posX);

		for(j=0; j < height; j++)
		{
			pos=pos2+3*800*j;
			xi=3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

			for(i=0; i<xi; i++)
			{
				if(spaceCorect != 0)
				{
					if( (LcdBuffer[pos+i]   == backGround[0])&&
						(LcdBuffer[pos+i+1] == backGround[1])&&
						(LcdBuffer[pos+i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
					else
						i+=2;
				}
				else
				{
					if( (pbmp[i]   == backGround[0])&&
						(pbmp[i+1] == backGround[1])&&
						(pbmp[i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = bkGroundColor;
						 i++;
						 LcdBuffer[pos+i] = bkGroundColor>>8;
						 i++;
						 LcdBuffer[pos+i] = bkGroundColor>>16;

					}
					else
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
				}
			}
			pbmp -= width*bit_pixel;
		}
		posX += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

//		if(FONT_48==fontSize)
//		{
//			if((txt[n]=='a')&&(txt[n+1]=='≥')){			spaceCorect=1;		spaceBetweenLatters=-4;		}
//			else if((txt[n]=='i')&&(txt[n+1]=='e')){	spaceCorect=1;		spaceBetweenLatters=8;	}
//			else if((txt[n]=='e')&&(txt[n+1]=='l')){	spaceCorect=1;		spaceBetweenLatters=4;		}
//			else{										spaceCorect=0;		spaceBetweenLatters=0;		}
//
//		}

		posX+=spaceBetweenLatters;

		if(posX + Font[fontIndex].fontsTabPos[(int)'W'][1] > LCD_GetXSize() )// tu dla FONT72 NIE mAAA BLAD !!!!! do poprawki
		{
			posX=X;
			posY+=height;
		}
	}
	touchTemp.x_End= posX;
	touchTemp.y_End= posY+height;
	return true;
}
bool LCD_DrawTxt_color_BufferWindow(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer,int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int widthWindow)
{
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);   int widthWin=widthWindow;  // while((widthWin%4)!=0) widthWin++;
	if(fontIndex==-1)
		return false;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,lenTxt=strlen(txt);
	int posX=X, posY=Y;
	int spaceCorect=0;
	char *pbmp;
	int spaceBetweenLatters=0;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint8_t backGround[3];
	uint32_t pos, pos2, xi;

	touchTemp.x_Start= posX;
	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);

	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround[0]=fontsBuffer[0];
	backGround[1]=fontsBuffer[1];
	backGround[2]=fontsBuffer[2];

	if(OnlyDigits==halfHight) height=Font[fontIndex].heightHalf;

	for(n=0;n<lenTxt;n++)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= 3*(widthWin*posY + posX);

		for(j=0; j < height; j++)
		{
			pos=pos2+3*widthWin*j;
			xi=3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

			for(i=0; i<xi; i++)
			{
				if(spaceCorect != 0)
				{
					if( (LcdBuffer[pos+i]   == backGround[0])&&
						(LcdBuffer[pos+i+1] == backGround[1])&&
						(LcdBuffer[pos+i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
					else
						i+=2;
				}
				else
				{
					if( (pbmp[i]   == backGround[0])&&
						(pbmp[i+1] == backGround[1])&&
						(pbmp[i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = bkGroundColor;
						 i++;
						 LcdBuffer[pos+i] = bkGroundColor>>8;
						 i++;
						 LcdBuffer[pos+i] = bkGroundColor>>16;

					}

					else
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}

				}
			}
			pbmp -= width*bit_pixel;
		}
		posX += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

//		if(FONT_48==fontSize)
//		{
//			if((txt[n]=='a')&&(txt[n+1]=='≥')){			spaceCorect=1;		spaceBetweenLatters=-4;		}
//			else if((txt[n]=='i')&&(txt[n+1]=='e')){	spaceCorect=1;		spaceBetweenLatters=8;	}
//			else if((txt[n]=='e')&&(txt[n+1]=='l')){	spaceCorect=1;		spaceBetweenLatters=4;		}
//			else{										spaceCorect=0;		spaceBetweenLatters=0;		}
//
//		}

		posX+=spaceBetweenLatters;

//		if(posX + Font[fontIndex].fontsTabPos[(int)'W'][1] > widthWin )// tu dla FONT72 NIE mAAA BLAD !!!!! do poprawki
//		{
//			posX=X;
//			posY+=height;
//		}
	}
	touchTemp.x_End= posX;
	touchTemp.y_End= posY+height;
	return true;
}

bool LCD_DrawTxtLine_Buff(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer, int lineWidth, int nrLine)
{
	if(nrLine==0)
		return false;
	int _nrLine=nrLine-1;
	int fontIndex=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(fontIndex==-1)
		return false;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,lenTxt=strlen(txt);
	int posX=X, posY=Y;
	int spaceCorect=0;
	char *pbmp;
	int spaceBetweenLatters=0;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint8_t backGround[3];
	uint32_t pos, pos2, xi;

	touchTemp.x_Start= posX;
	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);

	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround[0]=fontsBuffer[0];
	backGround[1]=fontsBuffer[1];
	backGround[2]=fontsBuffer[2];

	for(n=0;n<lenTxt;n++)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= 3*(800*posY + posX);

		for(j=0; j < height; j++)
		{
			pos=pos2+3*800*j;
			xi=3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

			for(i=0; i<xi; i++)
			{
				if(spaceCorect != 0)
				{
					if( (LcdBuffer[pos+i]   == backGround[0])&&
						(LcdBuffer[pos+i+1] == backGround[1])&&
						(LcdBuffer[pos+i+2] == backGround[2]) )
					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					}
					else
						i+=2;
				}
				else
				{
//					if( (pbmp[i]   != backGround[0])&&
//						(pbmp[i+1] != backGround[1])&&
//						(pbmp[i+2] != backGround[2]) )
//					{
//						 LcdBuffer[pos+i] = 0xFF;
//						 i++;
//						 LcdBuffer[pos+i] = 0xFF;
//						 i++;
//						 LcdBuffer[pos+i] = 0xFF;
//
//					}
//					else
//					{
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
						 i++;
						 LcdBuffer[pos+i] = *(pbmp+i);
					//}

				}
			}
			pbmp -= width*bit_pixel;
		}
		posX += Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

//		if(FONT_48==fontSize)
//		{
//			if((txt[n]=='a')&&(txt[n+1]=='≥')){			spaceCorect=1;		spaceBetweenLatters=-4;		}
//			else if((txt[n]=='i')&&(txt[n+1]=='e')){	spaceCorect=1;		spaceBetweenLatters=8;	}
//			else if((txt[n]=='e')&&(txt[n+1]=='l')){	spaceCorect=1;		spaceBetweenLatters=4;		}
//			else{										spaceCorect=0;		spaceBetweenLatters=0;		}
//
//		}

		if(posX + Font[fontIndex].fontsTabPos[ (int)txt[n+1] ][1] - X > lineWidth )
		{
			if((posY-Y)/height < _nrLine)
			{
				posX=X;
				posY+=height;
			}
			else
				return true;
		}
		else
			posX+=spaceBetweenLatters;

	}
	touchTemp.x_End= posX;
	touchTemp.y_End= posY+height;
	return true;
}

bool LCD_TxtID(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int OnlyDigits, int space)
{
	if(text_ID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
	{
		return LCD_DrawTxt_Buff(FontID[text_ID].size, FontID[text_ID].style, FontID[text_ID].bkColor, FontID[text_ID].color, Xpos,Ypos, txt, LcdBuffer,OnlyDigits,space);
	}
	else
		return false;
}
bool LCD_TxtID_BufferWindow(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int OnlyDigits, int widthWindow)
{
	if(text_ID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
	{
		return LCD_DrawTxt_BufferWindow(FontID[text_ID].size, FontID[text_ID].style, FontID[text_ID].bkColor, FontID[text_ID].color, Xpos,Ypos, txt, LcdBuffer,OnlyDigits,widthWindow);
	}
	else
		return false;
}
bool LCD_TxtID_color(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int OnlyDigits, uint32_t bkGroundColor, uint32_t fontColor, int space)
{
	if(text_ID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
	{
		return LCD_DrawTxt_Buff_color(FontID[text_ID].size, FontID[text_ID].style, FontID[text_ID].bkColor, FontID[text_ID].color, Xpos,Ypos, txt, LcdBuffer,OnlyDigits,bkGroundColor,fontColor,space);
	}
	else
		return false;
}
bool LCD_TxtID_color_BufferWindow(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int OnlyDigits, uint32_t bkGroundColor, uint32_t fontColor, int widthWindow)
{
	if(text_ID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
	{
		return LCD_DrawTxt_color_BufferWindow(FontID[text_ID].size, FontID[text_ID].style, FontID[text_ID].bkColor, FontID[text_ID].color, Xpos,Ypos, txt, LcdBuffer,OnlyDigits,bkGroundColor,fontColor, widthWindow);
	}
	else
		return false;
}

bool LCD_TxtLineID(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int lineWidth, int nrLine)
{
	if(text_ID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
	{
		return LCD_DrawTxtLine_Buff(FontID[text_ID].size, FontID[text_ID].style, FontID[text_ID].bkColor, FontID[text_ID].color, Xpos,Ypos, txt, LcdBuffer, lineWidth, nrLine);
	}
	else
		return false;
}

bool LCD_DrawImageIndirect(char *imageName,int X, int Y)
{
	int imageIndex=SearchImageName(imageName);
	if(imageIndex==-1)
		return false;
	uint8_t *imageBuffer=Image[imageIndex].pointerToMemory;
	BSP_LCD_DrawBitmap(X,Y,imageBuffer);
	return true;
}
int32_t LCD_GetWidthHeightBmp(char *imageName)
{
	int imageIndex=SearchImageName(imageName);
	if(imageIndex==-1)
		return -1;
	char *imageBuffer=(char*)Image[imageIndex].pointerToMemory;
	uint32_t width=0, height=0;
	/* Read bitmap width */
	width = imageBuffer[18] + (imageBuffer[19] << 8) + (imageBuffer[20] << 16)  + (imageBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4
	/* Read bitmap height */
	height = imageBuffer[22] + (imageBuffer[23] << 8) + (imageBuffer[24] << 16)  + (imageBuffer[25] << 24);
	return ((0xFFFF0000&(width<<16)) | (0x0000FFFF&height));
}

bool LCD_DrawBmpBuff_(char *imageName, char *LcdBuffer)
{
	int imageIndex=SearchImageName(imageName);
	if(imageIndex==-1)
		return false;
	char *imageBuffer=(char*)Image[imageIndex].pointerToMemory;
	int i,j;
	char *pbmp;
	uint32_t index=0, width=0, height=0, bit_pixel=0;

//	touchTemp.x_Start= posX;
//	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = imageBuffer[10] + (imageBuffer[11] << 8) + (imageBuffer[12] << 16)  + (imageBuffer[13] << 24);

	/* Read bitmap width */
	width = imageBuffer[18] + (imageBuffer[19] << 8) + (imageBuffer[20] << 16)  + (imageBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = imageBuffer[22] + (imageBuffer[23] << 8) + (imageBuffer[24] << 16)  + (imageBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = imageBuffer[28] + (imageBuffer[29] << 8);
	bit_pixel/=8;
	if(3!=bit_pixel)
		return false;

	imageBuffer += (index + (width * (height-1) * bit_pixel));   DbgSprintf(50,"\r\nBBBBBBBB: %d %d %d",width,height,bit_pixel );
	pbmp=imageBuffer;


	int k=4;
	LcdBuffer[0]=width>>8;
	LcdBuffer[1]=width;
	LcdBuffer[2]=height>>8;
	LcdBuffer[3]=height;

	for(j=0; j < height; j++)
	{
		//pos=pos2+3*width*j;
		for(i=0; i<(3*width); i++)
		{
			LcdBuffer[k++] = *(pbmp+i);
			i++;
			LcdBuffer[k++] = *(pbmp+i);
			i++;
		    LcdBuffer[k++] = *(pbmp+i);
		}
		pbmp -= width*bit_pixel;
	}
//	touchTemp.x_End= touchTemp.x_Start+width;
//	touchTemp.y_End= touchTemp.y_Start+height;
	return true;
}

bool LCD_DrawImage_Buff(char *imageName,int X, int Y, char *LcdBuffer)
{
	int imageIndex=SearchImageName(imageName);
	if(imageIndex==-1)
		return false;
	char *imageBuffer=(char*)Image[imageIndex].pointerToMemory;
	int i,j;
	int posX=X, posY=Y;
	char *pbmp;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint32_t pos, pos2;

	touchTemp.x_Start= posX;
	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = imageBuffer[10] + (imageBuffer[11] << 8) + (imageBuffer[12] << 16)  + (imageBuffer[13] << 24);

	/* Read bitmap width */
	width = imageBuffer[18] + (imageBuffer[19] << 8) + (imageBuffer[20] << 16)  + (imageBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = imageBuffer[22] + (imageBuffer[23] << 8) + (imageBuffer[24] << 16)  + (imageBuffer[25] << 24);

	/* Read bit/pixel */
	bit_pixel = imageBuffer[28] + (imageBuffer[29] << 8);
	bit_pixel/=8;
	if(3!=bit_pixel)
		return false;

	imageBuffer += (index + (width * (height-1) * bit_pixel));
	pbmp=imageBuffer;
	pos2= 3*(LCD_GetXSize()*posY + posX);

	for(j=0; j < height; j++)
	{
		pos=pos2+3*LCD_GetXSize()*j;
		for(i=0; i<(3*width); i++)
		{
			LcdBuffer[pos+i] = *(pbmp+i);
			i++;
			LcdBuffer[pos+i] = *(pbmp+i);
			i++;
		    LcdBuffer[pos+i] = *(pbmp+i);
		}
		pbmp -= width*bit_pixel;
	}
	touchTemp.x_End= touchTemp.x_Start+width;
	touchTemp.y_End= touchTemp.y_Start+height;
	return true;
}

int LCD_LoadAnimation(char *folderPathName, int imagesNumber, int everyImage)
{
	char path[100];
	int result,j=everyImage,nrImage=0,i;
	for(i=0; i<(imagesNumber*everyImage); ++i)
	{
		if(j==everyImage)
		{
			j=0;
			nrImage++;
			mini_snprintf(path,100,"%s/%d.bmp",folderPathName,i+1); //dbg("\r\n"); dbg(path);
			result=LCD_LoadImage(path);
			if(0!=result)
				return result;
		}
		j++;
	}
	mini_snprintf(path,100,"%s/1.bmp",folderPathName);
	i=SearchImageName(path);
	if(i>-1)
	{
		Image[i].Animation.imagesNumber=nrImage;
		Image[i].Animation.actualImage=0;
		Image[i].Animation.everyTime=0;
	}
	return 0;
}

int LCD_DrawAnimationIndirect(char *folderPathName,int animationSpeed, int Xpos, int Ypos)
{
	char path[100];
	int i;
	mini_snprintf(path,sizeof(path),"%s/1.bmp",folderPathName);
	i=SearchImageName(path);
	if(i>-1)
	{
		if( animationSpeed < (xTaskGetTickCount()-Image[i].Animation.everyTime))
		{
			Image[i].Animation.actualImage++;
			if(Image[i].Animation.actualImage > Image[i].Animation.imagesNumber)
				Image[i].Animation.actualImage=1;
			mini_snprintf(path,sizeof(path),"%s/%d.bmp",folderPathName,Image[i].Animation.actualImage); //dbg("\r\n"); dbg(path);
			if(false==LCD_DrawImageIndirect(path,Xpos,Ypos))
				return -2;
			Image[i].Animation.everyTime=xTaskGetTickCount();
		}
	}
	return i;
}


