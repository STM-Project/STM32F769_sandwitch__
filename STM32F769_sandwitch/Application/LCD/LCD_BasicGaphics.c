/*
 * layer.c
 *
 *  Created on: 01.02.2021
 *      Author: RafalMar
 */
#include "LCD_BasicGaphics.h"
#include <LCD_Hardware.h>
#include "LCD_Common.h"
#include <stdbool.h>
#include "sdram.h"
#include "touch.h"
#include "fonts_images.h"
#include "common.h"
#include "SDCard.h"
#include "string.h"

SDRAM char pLcd[1152000];  //800*480*3

//ALIGN_32BYTES(SDRAM char pLcd_[800*480*4]);
SDRAM uint32_t pLcd_[800*480];

#define MAX_NUMBER_TXT_VAR	100
SRAM static int txtTabParam[MAX_NUMBER_TXT_VAR][6];

PolygonPoint Point[10];

void LCD_DrawBoldRectangle(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t bkColor)
{
   uint32_t k,i,j,wier,kol;
    wier=0; kol=0;    j=0;

   k = 3*(y*LCD_GetXSize()+x);
   for(i=0;i<width*height;i++)
   {
       kol=j;
       if(kol==width){    k+=3*(LCD_GetXSize()-j);     wier++; kol=0; j=0;     }
       j++;

       if((kol==0)||(kol==1)||(kol==width-1)||(kol==width-2)||(wier==0)||(wier==1)||(wier==height-1)||(wier==height-2))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else
       {
            pLcd[k]=ColorWypel>>0; k++;
            pLcd[k]=ColorWypel>>8; k++;
            pLcd[k]=ColorWypel>>16; k++;
       }
   }
}

void LCD_DrawRoundRectangle1(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
   uint32_t k,i,j,wier,kol;
    wier=0; kol=0;    j=0;

   k = 3*(y*LCD_GetXSize()+x);
   for(i=0;i<width*height;i++)
   {
       kol=j;
       if(kol==width){    k+=3*(LCD_GetXSize()-j);     wier++; kol=0; j=0;     }
       j++;

       if( ((kol==0)      &&(wier==0))       ||
    	   ((kol==width-1)&&(wier==0))       ||
    	   ((kol==0)      &&(wier==height-1))||
    	   ((kol==width-1)&&(wier==height-1))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }

       else if((kol==0)||(kol==width-1)||(wier==0)||(wier==height-1))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else
       {
            pLcd[k]=ColorWypel>>0; k++;
            pLcd[k]=ColorWypel>>8; k++;
            pLcd[k]=ColorWypel>>16; k++;
       }
   }
}

void LCD_DrawBoldRoundRectangle1(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
   uint32_t k,i,j,wier,kol;
    wier=0; kol=0;    j=0;

   k = 3*(y*LCD_GetXSize()+x);
   for(i=0;i<width*height;i++)
   {
       kol=j;
       if(kol==width){    k+=3*(LCD_GetXSize()-j);     wier++; kol=0; j=0;     }
       j++;

       if( ((kol==0)      &&(wier==0))       ||
    	   ((kol==width-1)&&(wier==0))       ||
    	   ((kol==0)      &&(wier==height-1))||
    	   ((kol==width-1)&&(wier==height-1))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }

       else if((kol==0)||(kol==1)||(kol==width-1)||(kol==width-2)||(wier==0)||(wier==1)||(wier==height-1)||(wier==height-2))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else
       {
            pLcd[k]=ColorWypel>>0; k++;
            pLcd[k]=ColorWypel>>8; k++;
            pLcd[k]=ColorWypel>>16; k++;
       }
   }
}

void LCD_DrawRoundRectangle2(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
   uint32_t k,i,j,wier,kol;
   wier=0; kol=0;    j=0;

   k = 3*(y*LCD_GetXSize()+x);
   for(i=0;i<width*height;i++)
   {
       kol=j;
       if(kol==width){    k+=3*(LCD_GetXSize()-j);     wier++; kol=0; j=0;     }
       j++;

       if( ((kol==0)      &&(wier==0))       ||
    	   ((kol==width-1)&&(wier==0))       ||
    	   ((kol==0)      &&(wier==height-1))||
    	   ((kol==width-1)&&(wier==height-1))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if( ((kol==1)      &&(wier==0))       ||
    	        ((kol==width-2)&&(wier==0))       ||
    	        ((kol==1)      &&(wier==height-1))||
    	        ((kol==width-2)&&(wier==height-1))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if( ((kol==0)      &&(wier==1))       ||
    	        ((kol==width-1)&&(wier==1))       ||
    	        ((kol==0)      &&(wier==height-2))||
    	        ((kol==width-1)&&(wier==height-2))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if( ((kol==1)      &&(wier==1))       ||
    	        ((kol==width-2)&&(wier==1))       ||
    	        ((kol==1)      &&(wier==height-2))||
    	        ((kol==width-2)&&(wier==height-2))  )
       {
           pLcd[k]=ColorObram>>0; k++;
           pLcd[k]=ColorObram>>8; k++;
           pLcd[k]=ColorObram>>16; k++;
       }

       else if((kol==0)||(kol==width-1)||(wier==0)||(wier==height-1))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else
       {
            pLcd[k]=ColorWypel>>0; k++;
            pLcd[k]=ColorWypel>>8; k++;
            pLcd[k]=ColorWypel>>16; k++;
       }
   }
}
void LCD_DrawBoldRoundRectangle2(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
   uint32_t k,i,j,wier,kol;
   wier=0; kol=0;    j=0;

   k = 3*(y*LCD_GetXSize()+x);
   for(i=0;i<width*height;i++)
   {
       kol=j;
       if(kol==width){    k+=3*(LCD_GetXSize()-j);     wier++; kol=0; j=0;     }
       j++;

       if( ((kol==0)      &&(wier==0))       ||
    	   ((kol==width-1)&&(wier==0))       ||
    	   ((kol==0)      &&(wier==height-1))||
    	   ((kol==width-1)&&(wier==height-1))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if( ((kol==1)      &&(wier==0))       ||
    	        ((kol==width-2)&&(wier==0))       ||
    	        ((kol==1)      &&(wier==height-1))||
    	        ((kol==width-2)&&(wier==height-1))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if( ((kol==0)      &&(wier==1))       ||
    	        ((kol==width-1)&&(wier==1))       ||
    	        ((kol==0)      &&(wier==height-2))||
    	        ((kol==width-1)&&(wier==height-2))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if( ((kol==1)      &&(wier==1))       ||
    	        ((kol==width-2)&&(wier==1))       ||
    	        ((kol==1)      &&(wier==height-2))||
    	        ((kol==width-2)&&(wier==height-2))  )
       {
           pLcd[k]=ColorObram>>0; k++;
           pLcd[k]=ColorObram>>8; k++;
           pLcd[k]=ColorObram>>16; k++;
       }

       else if((kol==0)||(kol==1)||(kol==width-1)||(kol==width-2)||(wier==0)||(wier==height-1)||(wier==1)||(wier==height-2))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else
       {
            pLcd[k]=ColorWypel>>0; k++;
            pLcd[k]=ColorWypel>>8; k++;
            pLcd[k]=ColorWypel>>16; k++;
       }
   }
}






void LCD_DrawRoundRectangle3_TEST(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
	uint32_t k;

	void _FillBuff(int itCount, uint32_t color)   //ColorIntensiv(uint32_t colour, int16_t factor)
	{
		uint32_t col=color;

		for(int i=0;i<itCount;++i)
			pLcd_[k++]=col;
	}

	void _StartLine(void){
		k=(y*LCD_GetXSize()+x);
	}

	void _NextLine(void){
		k+=(LCD_GetXSize()-width);
	}

	_StartLine();
	_FillBuff(5, ColorInOut);
	_FillBuff(width-10, ColorObram);
	_FillBuff(5, ColorInOut);
	_NextLine();  //1 ----------------
	_FillBuff(3, ColorInOut);
	_FillBuff(2, ColorObram);
	_FillBuff(width-10, ColorWypel);
	_FillBuff(2, ColorObram);
	_FillBuff(3, ColorInOut);
	_NextLine();  //2 ----------------
	_FillBuff(2, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-6, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(2, ColorInOut);
	_NextLine();  //3 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine();  //4 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine(); //6 ----------------

	for (int j=0; j<(height-10); j++)
	{
		_FillBuff(1, ColorObram);
		_FillBuff(width-2, ColorWypel);
		_FillBuff(1, ColorObram);
		_NextLine();
	}

	_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine();
	_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine(); //2 ----------------
	_FillBuff(2, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-6, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(2, ColorInOut);
	_NextLine();  //1 ----------------
	_FillBuff(3, ColorInOut);
	_FillBuff(2, ColorObram);
	_FillBuff(width-10, ColorWypel);
	_FillBuff(2, ColorObram);
	_FillBuff(3, ColorInOut);
	_NextLine();  //0 ----------------
	_FillBuff(5, ColorInOut);
	_FillBuff(width-10, ColorObram);
	_FillBuff(5, ColorInOut);
}

void LCD_DrawRoundRectangle3_AA_TEST(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
	uint32_t k, a=ColorIntensiv(ColorObram,-75), b=ColorIntensiv(ColorObram,-125);

	void _FillBuff(int itCount, uint32_t color)
	{
		uint32_t col=color;

		for(int i=0;i<itCount;++i)
			pLcd_[k++]=col;
	}

	void _StartLine(void){
		k=(y*LCD_GetXSize()+x);
	}

	void _NextLine(void){
		k+=(LCD_GetXSize()-width);
	}

	_StartLine();
	_FillBuff(4, ColorInOut);
	 _FillBuff(1, a); _FillBuff(width-10, ColorObram);  _FillBuff(1, a);
	_FillBuff(4, ColorInOut);
	_NextLine();  //1 ----------------
	_FillBuff(2, ColorInOut);
	_FillBuff(1, a); _FillBuff(2, ColorObram);
	_FillBuff(width-10, ColorWypel);
	_FillBuff(2, ColorObram); _FillBuff(1, a);
	_FillBuff(2, ColorInOut);
	_NextLine();  //2 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(1, a); _FillBuff(1, ColorObram);
	_FillBuff(width-6, ColorWypel);
	_FillBuff(1, ColorObram); _FillBuff(1, a);
	_FillBuff(1, ColorInOut);
	_NextLine();  //3 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine();  //4 ----------------
	 _FillBuff(1, a);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	 _FillBuff(1, a);
	_NextLine(); //6 ----------------

	for (int j=0; j<(height-10); j++)
	{
		_FillBuff(1, ColorObram);
		_FillBuff(width-2, ColorWypel);
		_FillBuff(1, ColorObram);
		_NextLine();
	}

	 _FillBuff(1, a);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	 _FillBuff(1, a);
	_NextLine();
	_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine(); //2 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(1, a); _FillBuff(1, ColorObram);
	_FillBuff(width-6, ColorWypel);
	_FillBuff(1, ColorObram); _FillBuff(1, a);
	_FillBuff(1, ColorInOut);
	_NextLine();  //1 ----------------
	_FillBuff(2, ColorInOut);
	_FillBuff(1, a); _FillBuff(2, ColorObram);
	_FillBuff(width-10, ColorWypel);
	_FillBuff(2, ColorObram); _FillBuff(1, a);
	_FillBuff(2, ColorInOut);
	_NextLine();  //0 ----------------
	_FillBuff(4, ColorInOut);
	 _FillBuff(1, a); _FillBuff(width-10, ColorObram);  _FillBuff(1, a);
	_FillBuff(4, ColorInOut);
}

void LCD_DrawBoldRoundRectangle3_AA_TEST(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
	uint32_t k, a=ColorIntensiv(ColorObram,-75), b=ColorIntensiv(ColorObram,-125);

	void _FillBuff(int itCount, uint32_t color)
	{
		register uint32_t col=color;

		for(int i=0;i<itCount;i++)
			pLcd_[k++]=col;
	}

	void _StartLine(void){
		k=(y*LCD_GetXSize()+x);
	}

	void _NextLine(void){
		k+=(LCD_GetXSize()-width);
	}

	_StartLine();   //0 ----------------
	_FillBuff(3, ColorInOut);
	_FillBuff(1, b); _FillBuff(1, a); _FillBuff(width-10, ColorObram);  _FillBuff(1, a);_FillBuff(1, b);
	_FillBuff(3, ColorInOut);
	_NextLine();  //1 ----------------
	_FillBuff(2, ColorInOut);
	_FillBuff(1, a);  _FillBuff(width-6, ColorObram); _FillBuff(1, a);
	_FillBuff(2, ColorInOut);
	_NextLine();  //2 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(1, a); _FillBuff(2, ColorObram);
	_FillBuff(width-8, ColorWypel);
	_FillBuff(2, ColorObram); _FillBuff(1, a);
	_FillBuff(1, ColorInOut);
	_NextLine();  //3 ----------------
	_FillBuff(1, b);
	_FillBuff(2, ColorObram);
	_FillBuff(width-6, ColorWypel);
	_FillBuff(2, ColorObram);
	_FillBuff(1, b);
	_NextLine();  //4 ----------------
	 _FillBuff(1, a);//_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	 _FillBuff(1, a);//_FillBuff(1, ColorInOut);
	_NextLine(); //5 ----------------

	for (int j=0; j<(height-10); j++)
	{
		_FillBuff(2, ColorObram);
		_FillBuff(width-4, ColorWypel);
		_FillBuff(2, ColorObram);
		_NextLine();
	}

	 _FillBuff(1, a);//_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	 _FillBuff(1, a);//_FillBuff(1, ColorInOut);
	_NextLine(); //5 ----------------
	_FillBuff(1, b);
	_FillBuff(2, ColorObram);
	_FillBuff(width-6, ColorWypel);
	_FillBuff(2, ColorObram);
	_FillBuff(1, b);
	_NextLine();  //4 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(1, a); _FillBuff(2, ColorObram);
	_FillBuff(width-8, ColorWypel);
	 _FillBuff(2, ColorObram); _FillBuff(1, a);
	_FillBuff(1, ColorInOut);
	_NextLine();  //3 ----------------
	_FillBuff(2, ColorInOut);
	_FillBuff(1, a);  _FillBuff(width-6, ColorObram); _FillBuff(1, a);
	_FillBuff(2, ColorInOut);
	_NextLine();  //2 ----------------
	_FillBuff(3, ColorInOut);
	_FillBuff(1, b);_FillBuff(1, a); _FillBuff(width-10, ColorObram);  _FillBuff(1, a);_FillBuff(1, b);
	_FillBuff(3, ColorInOut);

}



void LCD_DrawBoldRoundRectangle3_TEST(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
	uint32_t k;

	void _FillBuff(int itCount, uint32_t color)   //ColorIntensiv(uint32_t colour, int16_t factor)
	{
		register uint32_t col=color;

		for(int i=0;i<itCount;i++)
			pLcd_[k++]=col;
	}

	void _StartLine(void){
		k=(y*LCD_GetXSize()+x);
	}

	void _NextLine(void){
		k+=(LCD_GetXSize()-width);
	}

	_StartLine();   //0 ----------------
	_FillBuff(5, ColorInOut);
	_FillBuff(width-10, ColorObram);
	_FillBuff(5, ColorInOut);
	_NextLine();  //1 ----------------
	_FillBuff(3, ColorInOut);
	_FillBuff(width-6, ColorObram);
	_FillBuff(3, ColorInOut);
	_NextLine();  //2 ----------------
	_FillBuff(2, ColorInOut);
	_FillBuff(2, ColorObram);
	_FillBuff(width-8, ColorWypel);
	_FillBuff(2, ColorObram);
	_FillBuff(2, ColorInOut);
	_NextLine();  //3 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(2, ColorObram);
	_FillBuff(width-6, ColorWypel);
	_FillBuff(2, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine();  //4 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine(); //5 ----------------

	for (int j=0; j<(height-10); j++)
	{
		_FillBuff(2, ColorObram);
		_FillBuff(width-4, ColorWypel);
		_FillBuff(2, ColorObram);
		_NextLine();
	}

	_FillBuff(1, ColorInOut);
	_FillBuff(1, ColorObram);
	_FillBuff(width-4, ColorWypel);
	_FillBuff(1, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine(); //5 ----------------
	_FillBuff(1, ColorInOut);
	_FillBuff(2, ColorObram);
	_FillBuff(width-6, ColorWypel);
	_FillBuff(2, ColorObram);
	_FillBuff(1, ColorInOut);
	_NextLine();  //4 ----------------
	_FillBuff(2, ColorInOut);
	_FillBuff(2, ColorObram);
	_FillBuff(width-8, ColorWypel);
	_FillBuff(2, ColorObram);
	_FillBuff(2, ColorInOut);
	_NextLine();  //3 ----------------
	_FillBuff(3, ColorInOut);
	_FillBuff(width-6, ColorObram);
	_FillBuff(3, ColorInOut);
	_NextLine();  //2 ----------------
	_FillBuff(5, ColorInOut);
	_FillBuff(width-10, ColorObram);
	_FillBuff(5, ColorInOut);

}




void LCD_DrawRoundRectangle3(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
   uint32_t k,i,j;


   k = 3*(y*LCD_GetXSize()+x);

   for(i=0;i<5;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     for(i=0;i<width-10;i++)
     {
        pLcd[k]=ColorObram>>0; k++;
        pLcd[k]=ColorObram>>8; k++;
        pLcd[k]=ColorObram>>16; k++;
     }
     for(i=0;i<5;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     k+=3*(LCD_GetXSize()-width);  //1 ----------------
     for(i=0;i<3;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     for(i=0;i<2;i++)
     {
        pLcd[k]=ColorObram>>0; k++;
        pLcd[k]=ColorObram>>8; k++;
        pLcd[k]=ColorObram>>16; k++;
     }
     for(i=0;i<width-10;i++)
     {
     	*((uint32_t *)(pLcd+k))=ColorWypel;
     	k+=3;
     }
     for(i=0;i<2;i++)
     {
        pLcd[k]=ColorObram>>0; k++;
        pLcd[k]=ColorObram>>8; k++;
        pLcd[k]=ColorObram>>16; k++;
     }
     for(i=0;i<3;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     k+=3*(LCD_GetXSize()-width);  //2 ----------------
     for(i=0;i<2;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     for(i=0;i<width-6;i++)
     {
     	*((uint32_t *)(pLcd+k))=ColorWypel;
     	k+=3;
     }
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     for(i=0;i<2;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     k+=3*(LCD_GetXSize()-width);  //3 ----------------
     pLcd[k]=ColorInOut>>0; k++;
     pLcd[k]=ColorInOut>>8; k++;
     pLcd[k]=ColorInOut>>16; k++;
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     for(i=0;i<width-4;i++)
     {
     	*((uint32_t *)(pLcd+k))=ColorWypel;
     	k+=3;
     }
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     pLcd[k]=ColorInOut>>0; k++;
     pLcd[k]=ColorInOut>>8; k++;
     pLcd[k]=ColorInOut>>16; k++;
     k+=3*(LCD_GetXSize()-width);  //4 ----------------
     pLcd[k]=ColorInOut>>0; k++;
     pLcd[k]=ColorInOut>>8; k++;
     pLcd[k]=ColorInOut>>16; k++;
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     for(i=0;i<width-4;i++)
     {
     	*((uint32_t *)(pLcd+k))=ColorWypel;
     	k+=3;
     }
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     pLcd[k]=ColorInOut>>0; k++;
     pLcd[k]=ColorInOut>>8; k++;
     pLcd[k]=ColorInOut>>16; k++;
     k+=3*(LCD_GetXSize()-width);  //6 ----------------


     for(j=0;j<(height-10);j++)
     {
        pLcd[k]=ColorObram>>0; k++;
        pLcd[k]=ColorObram>>8; k++;
        pLcd[k]=ColorObram>>16; k++;

        for(i=0;i<width-2;i++)
        {
        	*((uint32_t *)(pLcd+k))=ColorWypel;
        	k+=3;
        }

        pLcd[k]=ColorObram>>0; k++;
        pLcd[k]=ColorObram>>8; k++;
        pLcd[k]=ColorObram>>16; k++;

        k+=3*(LCD_GetXSize()-width);
     }

     pLcd[k]=ColorInOut>>0; k++;
     pLcd[k]=ColorInOut>>8; k++;
     pLcd[k]=ColorInOut>>16; k++;
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     for(i=0;i<width-4;i++)
     {
     	*((uint32_t *)(pLcd+k))=ColorWypel;
     	k+=3;
     }
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     pLcd[k]=ColorInOut>>0; k++;
     pLcd[k]=ColorInOut>>8; k++;
     pLcd[k]=ColorInOut>>16; k++;
     k+=3*(LCD_GetXSize()-width);
     pLcd[k]=ColorInOut>>0; k++;
     pLcd[k]=ColorInOut>>8; k++;
     pLcd[k]=ColorInOut>>16; k++;
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     for(i=0;i<width-4;i++)
     {
     	*((uint32_t *)(pLcd+k))=ColorWypel;
     	k+=3;
     }
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     pLcd[k]=ColorInOut>>0; k++;
     pLcd[k]=ColorInOut>>8; k++;
     pLcd[k]=ColorInOut>>16; k++;
     k+=3*(LCD_GetXSize()-width);  //2 ----------------
     for(i=0;i<2;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     for(i=0;i<width-6;i++)
     {
     	*((uint32_t *)(pLcd+k))=ColorWypel;
     	k+=3;
     }
     pLcd[k]=ColorObram>>0; k++;
     pLcd[k]=ColorObram>>8; k++;
     pLcd[k]=ColorObram>>16; k++;
     for(i=0;i<2;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     k+=3*(LCD_GetXSize()-width);  //1 ----------------
     for(i=0;i<3;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     for(i=0;i<2;i++)
     {
        pLcd[k]=ColorObram>>0; k++;
        pLcd[k]=ColorObram>>8; k++;
        pLcd[k]=ColorObram>>16; k++;
     }
     for(i=0;i<width-10;i++)
     {
     	*((uint32_t *)(pLcd+k))=ColorWypel;
     	k+=3;
     }
     for(i=0;i<2;i++)
     {
        pLcd[k]=ColorObram>>0; k++;
        pLcd[k]=ColorObram>>8; k++;
        pLcd[k]=ColorObram>>16; k++;
     }
     k+=3*(LCD_GetXSize()-width);  //0 ----------------
     for(i=0;i<3;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     for(i=0;i<5;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }
     for(i=0;i<width-10;i++)
     {
        pLcd[k]=ColorObram>>0; k++;
        pLcd[k]=ColorObram>>8; k++;
        pLcd[k]=ColorObram>>16; k++;
     }
     for(i=0;i<5;i++)
     {
        pLcd[k]=ColorInOut>>0; k++;
        pLcd[k]=ColorInOut>>8; k++;
        pLcd[k]=ColorInOut>>16; k++;
     }

//   uint32_t wier,kol;
//    wier=0; kol=0;    j=0;
//
//   for(i=0;i<width*height;i++)
//     {
//         kol=j;
//         if(kol==width){    k+=3*(LCD_GetXSize()-j);     wier++; kol=0; j=0;     }
//         j++;
//
//         if( ((kol==4)      &&(wier==1))       ||
//      	   ((kol==width-5)&&(wier==1))       ||
//      	   ((kol==4)      &&(wier==height-2))||
//      	   ((kol==width-5)&&(wier==height-2))  )
//         {
//             pLcd[k]=ColorObram>>0; k++;
//             pLcd[k]=ColorObram>>8; k++;
//             pLcd[k]=ColorObram>>16; k++;
//         }
//         else if( ((kol==4)      &&(wier<1))       ||
//             	    ((kol==width-5)&&(wier<1))       ||
//             	    ((kol==4)      &&(wier>height-2))||
//             	    ((kol==width-5)&&(wier>height-2))  )
//         {
//             pLcd[k]=ColorInOut>>0; k++;
//             pLcd[k]=ColorInOut>>8; k++;
//             pLcd[k]=ColorInOut>>16; k++;
//         }
//         else if(((kol==3)      &&(wier==1))       ||
//          	   ((kol==width-4)&&(wier==1))       ||
//          	   ((kol==3)      &&(wier==height-2))||
//          	   ((kol==width-4)&&(wier==height-2))  )
//         {
//             pLcd[k]=ColorObram>>0; k++;
//             pLcd[k]=ColorObram>>8; k++;
//             pLcd[k]=ColorObram>>16; k++;
//         }
//         else if(((kol==3)      &&(wier<1))       ||
//          	   ((kol==width-4)&&(wier<1))       ||
//          	   ((kol==3)      &&(wier>height-2))||
//          	   ((kol==width-4)&&(wier>height-2))  )
//         {
//             pLcd[k]=ColorInOut>>0; k++;
//             pLcd[k]=ColorInOut>>8; k++;
//             pLcd[k]=ColorInOut>>16; k++;
//         }
//         else if(((kol==2)      &&(wier==2))       ||
//          	   ((kol==width-3)&&(wier==2))       ||
//          	   ((kol==2)      &&(wier==height-3))||
//          	   ((kol==width-3)&&(wier==height-3))  )
//         {
//             pLcd[k]=ColorObram>>0; k++;
//             pLcd[k]=ColorObram>>8; k++;
//             pLcd[k]=ColorObram>>16; k++;
//         }
//         else if(((kol==2)      &&(wier<2))       ||
//          	   ((kol==width-3)&&(wier<2))       ||
//          	   ((kol==2)      &&(wier>height-3))||
//          	   ((kol==width-3)&&(wier>height-3))  )
//         {
//             pLcd[k]=ColorInOut>>0; k++;
//             pLcd[k]=ColorInOut>>8; k++;
//             pLcd[k]=ColorInOut>>16; k++;
//         }
//         else if(((kol==1)      &&(wier==3))       ||
//          	   ((kol==width-2)&&(wier==3))       ||
//          	   ((kol==1)      &&(wier==height-4))||
//          	   ((kol==width-2)&&(wier==height-4))  )
//         {
//             pLcd[k]=ColorObram>>0; k++;
//             pLcd[k]=ColorObram>>8; k++;
//             pLcd[k]=ColorObram>>16; k++;
//         }
//         else if(((kol==1)      &&(wier<3))       ||
//          	   ((kol==width-2)&&(wier<3))       ||
//          	   ((kol==1)      &&(wier>height-4))||
//          	   ((kol==width-2)&&(wier>height-4))  )
//         {
//             pLcd[k]=ColorInOut>>0; k++;
//             pLcd[k]=ColorInOut>>8; k++;
//             pLcd[k]=ColorInOut>>16; k++;
//         }
//         else if(((kol==1)      &&(wier<3))       ||
//          	   ((kol==width-2)&&(wier<3))       ||
//          	   ((kol==1)      &&(wier>height-4))||
//          	   ((kol==width-2)&&(wier>height-4))  )
//         {
//             pLcd[k]=ColorInOut>>0; k++;
//             pLcd[k]=ColorInOut>>8; k++;
//             pLcd[k]=ColorInOut>>16; k++;
//         }
//         else if(((kol==1)      &&(wier==4))       ||
//          	   ((kol==width-2)&&(wier==4))       ||
//          	   ((kol==1)      &&(wier==height-5))||
//          	   ((kol==width-2)&&(wier==height-5))  )
//         {
//             pLcd[k]=ColorObram>>0; k++;
//             pLcd[k]=ColorObram>>8; k++;
//             pLcd[k]=ColorObram>>16; k++;
//         }
//         else if(((kol==1)      &&(wier<4))       ||
//          	   ((kol==width-2)&&(wier<4))       ||
//          	   ((kol==1)      &&(wier>height-5))||
//          	   ((kol==width-2)&&(wier>height-5))  )
//         {
//             pLcd[k]=ColorInOut>>0; k++;
//             pLcd[k]=ColorInOut>>8; k++;
//             pLcd[k]=ColorInOut>>16; k++;
//         }
//         else if(((kol==0)      &&(wier<5))       ||
//          	   ((kol==width-1)&&(wier<5))       ||
//          	   ((kol==0)      &&(wier>height-6))||
//          	   ((kol==width-1)&&(wier>height-6))  )
//         {
//             pLcd[k]=ColorInOut>>0; k++;
//             pLcd[k]=ColorInOut>>8; k++;
//             pLcd[k]=ColorInOut>>16; k++;
//         }
//         else if(((kol>4)&&(kol<width-5))&&((wier==0)||(wier==height-1)))
//         {
//              pLcd[k]=ColorObram>>0; k++;
//              pLcd[k]=ColorObram>>8; k++;
//              pLcd[k]=ColorObram>>16; k++;
//         }
//         else if(((wier>4)&&(wier<height-5))&&((kol==0)||(kol==width-1)))
//         {
//              pLcd[k]=ColorObram>>0; k++;
//              pLcd[k]=ColorObram>>8; k++;
//              pLcd[k]=ColorObram>>16; k++;
//         }
//         else
//         {
//              pLcd[k]=ColorWypel>>0; k++;
//              pLcd[k]=ColorWypel>>8; k++;
//              pLcd[k]=ColorWypel>>16; k++;
//         }
//     }

}
void LCD_DrawBoldRoundRectangle3(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut)
{
   uint32_t k,i,j,wier,kol;
    wier=0; kol=0;    j=0;

   k = 3*(y*LCD_GetXSize()+x);
   for(i=0;i<width*height;i++)
   {
       kol=j;
       if(kol==width){    k+=3*(LCD_GetXSize()-j);     wier++; kol=0; j=0;     }
       j++;

       if( ((kol==4)      &&(wier==1))       ||
    	   ((kol==width-5)&&(wier==1))       ||
    	   ((kol==4)      &&(wier==height-2))||
    	   ((kol==width-5)&&(wier==height-2))  )
       {
           pLcd[k]=ColorObram>>0; k++;
           pLcd[k]=ColorObram>>8; k++;
           pLcd[k]=ColorObram>>16; k++;
       }
       else if( ((kol==4)      &&(wier<1))       ||
           	    ((kol==width-5)&&(wier<1))       ||
           	    ((kol==4)      &&(wier>height-2))||
           	    ((kol==width-5)&&(wier>height-2))  )
       {
             pLcd[k]=ColorInOut>>0; k++;
             pLcd[k]=ColorInOut>>8; k++;
             pLcd[k]=ColorInOut>>16; k++;
       }
       else if(((kol==3)      &&((wier==1)||(wier==2)))       ||
        	   ((kol==width-4)&&((wier==1)||(wier==2)))       ||
        	   ((kol==3)      &&((wier==height-2)||(wier==height-3)))||
        	   ((kol==width-4)&&((wier==height-2)||(wier==height-3)))  )
       {
           pLcd[k]=ColorObram>>0; k++;
           pLcd[k]=ColorObram>>8; k++;
           pLcd[k]=ColorObram>>16; k++;
       }
       else if(((kol==3)      &&(wier<1))       ||
        	   ((kol==width-4)&&(wier<1))       ||
        	   ((kol==3)      &&(wier>height-2))||
        	   ((kol==width-4)&&(wier>height-2))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if(((kol==2)      &&((wier==2)||(wier==3)))       ||
        	   ((kol==width-3)&&((wier==2)||(wier==3)))       ||
        	   ((kol==2)      &&((wier==height-3)||(wier==height-4)))||
        	   ((kol==width-3)&&((wier==height-3)||(wier==height-4)))  )
       {
           pLcd[k]=ColorObram>>0; k++;
           pLcd[k]=ColorObram>>8; k++;
           pLcd[k]=ColorObram>>16; k++;
       }
       else if(((kol==2)      &&(wier<2))       ||
        	   ((kol==width-3)&&(wier<2))       ||
        	   ((kol==2)      &&(wier>height-3))||
        	   ((kol==width-3)&&(wier>height-3))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if(((kol==1)      &&(wier==3))       ||
        	   ((kol==width-2)&&(wier==3))       ||
        	   ((kol==1)      &&(wier==height-4))||
        	   ((kol==width-2)&&(wier==height-4))  )
       {
           pLcd[k]=ColorObram>>0; k++;
           pLcd[k]=ColorObram>>8; k++;
           pLcd[k]=ColorObram>>16; k++;
       }
       else if(((kol==1)      &&(wier<3))       ||
        	   ((kol==width-2)&&(wier<3))       ||
        	   ((kol==1)      &&(wier>height-4))||
        	   ((kol==width-2)&&(wier>height-4))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if(((kol==1)      &&(wier<3))       ||
        	   ((kol==width-2)&&(wier<3))       ||
        	   ((kol==1)      &&(wier>height-4))||
        	   ((kol==width-2)&&(wier>height-4))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if(((kol==1)      &&(wier==4))       ||
        	   ((kol==width-2)&&(wier==4))       ||
        	   ((kol==1)      &&(wier==height-5))||
        	   ((kol==width-2)&&(wier==height-5))  )
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else if(((kol==1)      &&(wier<4))       ||
        	   ((kol==width-2)&&(wier<4))       ||
        	   ((kol==1)      &&(wier>height-5))||
        	   ((kol==width-2)&&(wier>height-5))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if(((kol==0)      &&(wier<5))       ||
        	   ((kol==width-1)&&(wier<5))       ||
        	   ((kol==0)      &&(wier>height-6))||
        	   ((kol==width-1)&&(wier>height-6))  )
       {
           pLcd[k]=ColorInOut>>0; k++;
           pLcd[k]=ColorInOut>>8; k++;
           pLcd[k]=ColorInOut>>16; k++;
       }
       else if(((kol>4)&&(kol<width-5))&&((wier==0)||(wier==height-1)||(wier==1)||(wier==height-2)))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else if(((wier>4)&&(wier<height-5))&&((kol==0)||(kol==1)||(kol==width-1)||(kol==width-2)))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else
       {
            pLcd[k]=ColorWypel>>0; k++;
            pLcd[k]=ColorWypel>>8; k++;
            pLcd[k]=ColorWypel>>16; k++;
       }
   }
}

void LCD_DrawDownTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h,height=width/2;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<height+1;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if((w>-1+h)&&(w<width-h))
	         {
	        	 if(((w==h)||(w==width-h-1))||(h==0))
	        	 {
	        		 pLcd[k++]=frameColor>>0;
	        		 pLcd[k++]=frameColor>>8;
	        		 pLcd[k++]=frameColor>>16;
	        	 }
	        	 else
	        	 {
	        		 pLcd[k++]=fillColor>>0;
	        		 pLcd[k++]=fillColor>>8;
	        		 pLcd[k++]=fillColor>>16;
	        	 }
	         }
	    	 else
	    	 {
	    		 pLcd[k++]=BkColor>>0;
	    		 pLcd[k++]=BkColor>>8;
	    		 pLcd[k++]=BkColor>>16;
	    	 }
	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width;
	touchTemp.y_End= y+width/2+1;
}

void LCD_DrawRightUpTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<width;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if(w>=h)
	         {
	        	 if(((w==h)||(w==width-1))||(h==0))
	        	 {
	        		 pLcd[k++]=frameColor>>0;
	        		 pLcd[k++]=frameColor>>8;
	        		 pLcd[k++]=frameColor>>16;
	        	 }
	        	 else
	        	 {
	        		 pLcd[k++]=fillColor>>0;
	        		 pLcd[k++]=fillColor>>8;
	        		 pLcd[k++]=fillColor>>16;
	        	 }
	         }
	    	 else
	    	 {
	    		 pLcd[k++]=BkColor>>0;
	    		 pLcd[k++]=BkColor>>8;
	    		 pLcd[k++]=BkColor>>16;
	    	 }
	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width;
	touchTemp.y_End= y+width;
}

void LCD_DrawLeftTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<width+1;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-(width/2+1));
		for(w=width/2;w>-1;--w)
	    {
	    	if(h<=width/2)
	    	{
				 if(w<=h)
		         {
		        	 if((w==h)||(w==0))
		        	 {
		        		 pLcd[k++]=frameColor>>0;
		        		 pLcd[k++]=frameColor>>8;
		        		 pLcd[k++]=frameColor>>16;
		        	 }
		        	 else
		        	 {
		        		 pLcd[k++]=fillColor>>0;
		        		 pLcd[k++]=fillColor>>8;
		        		 pLcd[k++]=fillColor>>16;
		        	 }
		         }
				 else
				 {
		    		 pLcd[k++]=BkColor>>0;
		    		 pLcd[k++]=BkColor>>8;
		    		 pLcd[k++]=BkColor>>16;
				 }

	    	}
	    	else
	    	{
				 if(w<=width-h)
		         {
		        	 if((w==width-h)||(w==0))
		        	 {
		        		 pLcd[k++]=frameColor>>0;
		        		 pLcd[k++]=frameColor>>8;
		        		 pLcd[k++]=frameColor>>16;
		        	 }
		        	 else
		        	 {
		        		 pLcd[k++]=fillColor>>0;
		        		 pLcd[k++]=fillColor>>8;
		        		 pLcd[k++]=fillColor>>16;
		        	 }
		         }
				 else
				 {
		    		 pLcd[k++]=BkColor>>0;
		    		 pLcd[k++]=BkColor>>8;
		    		 pLcd[k++]=BkColor>>16;
				 }
	    	}


	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width/2+1;
	touchTemp.y_End= y+width+1;
}

void LCD_DrawRightTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<width+1;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-(width/2+1));
		for(w=0;w<width/2+1;++w)
	    {
	    	if(h<=width/2)
	    	{
				 if(w<=h)
		         {
		        	 if((w==h)||(w==0))
		        	 {
		        		 pLcd[k++]=frameColor>>0;
		        		 pLcd[k++]=frameColor>>8;
		        		 pLcd[k++]=frameColor>>16;
		        	 }
		        	 else
		        	 {
		        		 pLcd[k++]=fillColor>>0;
		        		 pLcd[k++]=fillColor>>8;
		        		 pLcd[k++]=fillColor>>16;
		        	 }
		         }
				 else
				 {
		    		 pLcd[k++]=BkColor>>0;
		    		 pLcd[k++]=BkColor>>8;
		    		 pLcd[k++]=BkColor>>16;
				 }

	    	}
	    	else
	    	{
				 if(w<=width-h)
		         {
		        	 if((w==width-h)||(w==0))
		        	 {
		        		 pLcd[k++]=frameColor>>0;
		        		 pLcd[k++]=frameColor>>8;
		        		 pLcd[k++]=frameColor>>16;
		        	 }
		        	 else
		        	 {
		        		 pLcd[k++]=fillColor>>0;
		        		 pLcd[k++]=fillColor>>8;
		        		 pLcd[k++]=fillColor>>16;
		        	 }
		         }
				 else
				 {
		    		 pLcd[k++]=BkColor>>0;
		    		 pLcd[k++]=BkColor>>8;
		    		 pLcd[k++]=BkColor>>16;
				 }
	    	}


	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width/2+1+10;
	touchTemp.y_End= y+width+1;
}

void LCD_DrawRightDownTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<width;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if(w+h>=width)
	         {
	        	 if(((w+h==width)||(w==width-1))||(h==width-1))
	        	 {
	        		 pLcd[k++]=frameColor>>0;
	        		 pLcd[k++]=frameColor>>8;
	        		 pLcd[k++]=frameColor>>16;
	        	 }
	        	 else
	        	 {
	        		 pLcd[k++]=fillColor>>0;
	        		 pLcd[k++]=fillColor>>8;
	        		 pLcd[k++]=fillColor>>16;
	        	 }
	         }
	    	 else
	    	 {
	    		 pLcd[k++]=BkColor>>0;
	    		 pLcd[k++]=BkColor>>8;
	    		 pLcd[k++]=BkColor>>16;
	    	 }
	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width;
	touchTemp.y_End= y+width;
}

void LCD_DrawRightDownTriangle_xPosFill(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int xPosFill)
{
	int k,w,h;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<width;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if(w+h>=width)
	         {
	        	 if(((w+h==width)||(w==width-1))||(h==width-1))
	        	 {
	        		 pLcd[k++]=frameColor>>0;
	        		 pLcd[k++]=frameColor>>8;
	        		 pLcd[k++]=frameColor>>16;
	        	 }
	        	 else
	        	 {
	        		 if(w<=xPosFill)
	        		 {
		        		 pLcd[k++]=fillColor>>0;
		        		 pLcd[k++]=fillColor>>8;
		        		 pLcd[k++]=fillColor>>16;
	        		 }
	        		 else
	        		 {
	    	    		 pLcd[k++]=BkColor>>0;
	    	    		 pLcd[k++]=BkColor>>8;
	    	    		 pLcd[k++]=BkColor>>16;
	        		 }
	        	 }
	         }
	    	 else
	    	 {
	    		 pLcd[k++]=BkColor>>0;
	    		 pLcd[k++]=BkColor>>8;
	    		 pLcd[k++]=BkColor>>16;
	    	 }
	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width;
	touchTemp.y_End= y+width;
}

void LCD_DrawLeftDownTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<width;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-width);
		for(w=width;w>-1;--w)
	    {
	    	 if(w<=h)
	         {
	        	 if(((w==h)||(w==0))||(h==width-1))
	        	 {
	        		 pLcd[k++]=frameColor>>0;
	        		 pLcd[k++]=frameColor>>8;
	        		 pLcd[k++]=frameColor>>16;
	        	 }
	        	 else
	        	 {
	        		 pLcd[k++]=fillColor>>0;
	        		 pLcd[k++]=fillColor>>8;
	        		 pLcd[k++]=fillColor>>16;
	        	 }
	         }
	    	 else
	    	 {
	    		 pLcd[k++]=BkColor>>0;
	    		 pLcd[k++]=BkColor>>8;
	    		 pLcd[k++]=BkColor>>16;
	    	 }
	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width;
	touchTemp.y_End= y+width;
}

void LCD_DrawBoldDownTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h,height=width/2;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<height+1;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if((w>-1+h)&&(w<width-h))
	         {
	    		 if( (((w==h)||(w==width-h-1))||(h==0)) || (((w==h+1)||(w==width-h-2))||(h==1)) )
	        	 {
	        		 pLcd[k++]=frameColor>>0;
	        		 pLcd[k++]=frameColor>>8;
	        		 pLcd[k++]=frameColor>>16;
	        	 }
	        	 else
	        	 {
	        		 pLcd[k++]=fillColor>>0;
	        		 pLcd[k++]=fillColor>>8;
	        		 pLcd[k++]=fillColor>>16;
	        	 }
	         }
	    	 else
	    	 {
	    		 pLcd[k++]=BkColor>>0;
	    		 pLcd[k++]=BkColor>>8;
	    		 pLcd[k++]=BkColor>>16;
	    	 }
	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width;
	touchTemp.y_End= y+width;
}

void LCD_DrawUpTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h,height=width/2;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=height;h>-1;--h)
	{
		if(h<height) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if((w>-1+h)&&(w<width-h))
	         {
	        	 if(((w==h)||(w==width-h-1))||(h==0))
	        	 {
	        		 pLcd[k++]=frameColor>>0;
	        		 pLcd[k++]=frameColor>>8;
	        		 pLcd[k++]=frameColor>>16;
	        	 }
	        	 else
	        	 {
	        		 pLcd[k++]=fillColor>>0;
	        		 pLcd[k++]=fillColor>>8;
	        		 pLcd[k++]=fillColor>>16;
	        	 }
	         }
	    	 else
	    	 {
	    		 pLcd[k++]=BkColor>>0;
	    		 pLcd[k++]=BkColor>>8;
	    		 pLcd[k++]=BkColor>>16;
	    	 }
	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width;
	touchTemp.y_End= y+width/2+1;
}

void LCD_DrawBoldUpTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k,w,h,height=width/2;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=height;h>-1;--h)
	{
		if(h<height) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if((w>-1+h)&&(w<width-h))
	         {
	    		 if( (((w==h)||(w==width-h-1))||(h==0)) || (((w==h+1)||(w==width-h-2))||(h==1)) )
	        	 {
	        		 pLcd[k++]=frameColor>>0;
	        		 pLcd[k++]=frameColor>>8;
	        		 pLcd[k++]=frameColor>>16;
	        	 }
	        	 else
	        	 {
	        		 pLcd[k++]=fillColor>>0;
	        		 pLcd[k++]=fillColor>>8;
	        		 pLcd[k++]=fillColor>>16;
	        	 }
	         }
	    	 else
	    	 {
	    		 pLcd[k++]=BkColor>>0;
	    		 pLcd[k++]=BkColor>>8;
	    		 pLcd[k++]=BkColor>>16;
	    	 }
	    }
	}
	touchTemp.x_Start= x;
	touchTemp.y_Start= y;
	touchTemp.x_End= x+width;
	touchTemp.y_End= y+width;
}

bool TxtID_color(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, uint32_t bkGroundColor, uint32_t fontColor, int space)
{
	return LCD_TxtID_color(fontID, Xpos,Ypos, txt, pLcd,OnlyDigits, bkGroundColor,fontColor,space);
}

bool TxtID_Var_color(int nrTxt, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, uint32_t bkGroundColor, uint32_t fontColor, int space)
{
	if(nrTxt < MAX_NUMBER_TXT_VAR)
	{
		txtTabParam[nrTxt][0]=LCD_GetFontID_size(fontID);
		txtTabParam[nrTxt][1]=LCD_GetFontID_style(fontID);
		txtTabParam[nrTxt][2]=LCD_GetFontID_bkColor(fontID);
		txtTabParam[nrTxt][3]=LCD_GetFontID_color(fontID);
		txtTabParam[nrTxt][4]=Xpos;
		txtTabParam[nrTxt][5]=Ypos;
		return LCD_DrawTxt_Buff_color(txtTabParam[nrTxt][0],txtTabParam[nrTxt][1],txtTabParam[nrTxt][2],txtTabParam[nrTxt][3],Xpos,Ypos,txt,pLcd,OnlyDigits,bkGroundColor,fontColor,space);
	}
	else
		return false;
}

bool LCD_DrawTxt_Var(int nrTxt,int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int Xpos, int Ypos, char *txt, int OnlyDigits)
{
	if(nrTxt < MAX_NUMBER_TXT_VAR)
	{
		txtTabParam[nrTxt][0]=fontSize;
		txtTabParam[nrTxt][1]=fontStyle;
		txtTabParam[nrTxt][2]=backgroundColor;
		txtTabParam[nrTxt][3]=fontColor;
		txtTabParam[nrTxt][4]=Xpos;
		txtTabParam[nrTxt][5]=Ypos;
		return LCD_DrawTxt_Buff(fontSize,fontStyle,backgroundColor,fontColor,Xpos,Ypos,txt,pLcd,OnlyDigits,0);
	}
	else
		return false;
}

bool TxtID_Var(int nrTxt, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space)
{
	if(nrTxt < MAX_NUMBER_TXT_VAR)
	{
		txtTabParam[nrTxt][0]=LCD_GetFontID_size(fontID);
		txtTabParam[nrTxt][1]=LCD_GetFontID_style(fontID);
		txtTabParam[nrTxt][2]=LCD_GetFontID_bkColor(fontID);
		txtTabParam[nrTxt][3]=LCD_GetFontID_color(fontID);
		txtTabParam[nrTxt][4]=Xpos;
		txtTabParam[nrTxt][5]=Ypos;
		return LCD_DrawTxt_Buff(txtTabParam[nrTxt][0],txtTabParam[nrTxt][1],txtTabParam[nrTxt][2],txtTabParam[nrTxt][3],Xpos,Ypos,txt,pLcd,OnlyDigits,space);
	}
	else
		return false;
}







void LCD_SetTxtYPos(int nrTxt, int posY)
{
	txtTabParam[nrTxt][5]=posY;
}
void LCD_SetTxtXPos(int nrTxt, int posX)
{
	txtTabParam[nrTxt][4]=posX;
}
void LCD_Txt_AddY(int nrTxt, int Ypos)
{
	if(nrTxt<MAX_NUMBER_TXT_VAR) txtTabParam[nrTxt][5]+=Ypos;
}
int LCD_GetTxtXPos(int nrTxt)
{
	if(nrTxt<MAX_NUMBER_TXT_VAR)
		return txtTabParam[nrTxt][4];
	else
		return 0;
}
int LCD_GetTxtYPos(int nrTxt)
{
	if(nrTxt<MAX_NUMBER_TXT_VAR)
		return txtTabParam[nrTxt][5];
	else
		return 0;
}
void LCD_Txt_SubY(int nrTxt, int Ypos)
{
	if(nrTxt<MAX_NUMBER_TXT_VAR) txtTabParam[nrTxt][5]-=Ypos;
}
bool LCD_DrawTxtIndirect_color(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int Xpos, int Ypos, char *txt, int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int space)
{
	return LCD_DrawTxtIndirect_Buff_color(fontSize,fontStyle,backgroundColor,fontColor,Xpos,Ypos, txt,pLcd, OnlyDigits,bkGroundColor,FontColor,space);
}
bool LCD_DrawTxtIndirect(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int Xpos, int Ypos, char *txt, int OnlyDigits, int space)
{
	return LCD_DrawTxtIndirect_Buff(fontSize,fontStyle,backgroundColor,fontColor,Xpos,Ypos, txt,pLcd, OnlyDigits,space);
}
void LCD_Txt(int nrTxt, char *txt, int otherFont, int OnlyDigits, int space)
{
	if(nrTxt<MAX_NUMBER_TXT_VAR)
	{
		if(fontNoChange==otherFont)
			LCD_DrawTxtIndirect(txtTabParam[nrTxt][0],txtTabParam[nrTxt][1],txtTabParam[nrTxt][2],txtTabParam[nrTxt][3],txtTabParam[nrTxt][4],txtTabParam[nrTxt][5], txt,OnlyDigits,space);
		else
			LCD_DrawTxtIndirect(LCD_GetFontID_size(otherFont),LCD_GetFontID_style(otherFont),LCD_GetFontID_bkColor(otherFont),LCD_GetFontID_color(otherFont),txtTabParam[nrTxt][4],txtTabParam[nrTxt][5], txt,OnlyDigits,space);
	}
}
void LCD_Txt_color(int nrTxt, char *txt, int otherFont, int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int space)
{
	if(nrTxt<MAX_NUMBER_TXT_VAR)
	{
		if(fontNoChange==otherFont)
			LCD_DrawTxtIndirect_color(txtTabParam[nrTxt][0],txtTabParam[nrTxt][1],txtTabParam[nrTxt][2],txtTabParam[nrTxt][3],txtTabParam[nrTxt][4],txtTabParam[nrTxt][5], txt,OnlyDigits,bkGroundColor,FontColor,space);
		else
			LCD_DrawTxtIndirect_color(LCD_GetFontID_size(otherFont),LCD_GetFontID_style(otherFont),LCD_GetFontID_bkColor(otherFont),LCD_GetFontID_color(otherFont),txtTabParam[nrTxt][4],txtTabParam[nrTxt][5], txt,OnlyDigits,bkGroundColor,FontColor,space);
	}
}

void BSP_LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* Decision Variable */
  uint32_t  current_x;   /* Current X Value */
  uint32_t  current_y;   /* Current Y Value */
  int color=0x00FF00;

  decision = 3 - (Radius << 1);
  current_x = 0;
  current_y = Radius;

  while (current_x <= current_y)
  {
    BSP_LCD_DrawPixel((Xpos + current_x), (Ypos - current_y), color);

    BSP_LCD_DrawPixel((Xpos - current_x), (Ypos - current_y), color);

    BSP_LCD_DrawPixel((Xpos + current_y), (Ypos - current_x), color);

    BSP_LCD_DrawPixel((Xpos - current_y), (Ypos - current_x), color);

    BSP_LCD_DrawPixel((Xpos + current_x), (Ypos + current_y), color);

    BSP_LCD_DrawPixel((Xpos - current_x), (Ypos + current_y), color);

    BSP_LCD_DrawPixel((Xpos + current_y), (Ypos + current_x), color);

    BSP_LCD_DrawPixel((Xpos - current_y), (Ypos + current_x), color);

    if (decision < 0)
    {
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }
}

void BSP_LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  decision;     /* Decision Variable */
  uint32_t  current_x;   /* Current X Value */
  uint32_t  current_y;   /* Current Y Value */

  decision = 3 - (Radius << 1);

  current_x = 0;
  current_y = Radius;

  while (current_x <= current_y)
  {
    if(current_y > 0)
    {
      BSP_LCD_DrawHLine(Xpos - current_y, Ypos + current_x, 2*current_y);
      BSP_LCD_DrawHLine(Xpos - current_y, Ypos - current_x, 2*current_y);
    }

    if(current_x > 0)
    {
      BSP_LCD_DrawHLine(Xpos - current_x, Ypos - current_y, 2*current_x);
      BSP_LCD_DrawHLine(Xpos - current_x, Ypos + current_y, 2*current_x);
    }
    if (decision < 0)
    {
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }

  BSP_LCD_DrawCircle(Xpos, Ypos, Radius);
}

void BSP_LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius;

  k = (float)(rad2/rad1);

  do {
    BSP_LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos+y), WHITE);
    BSP_LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos+y), WHITE);
    BSP_LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos-y), WHITE);
    BSP_LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos-y), WHITE);

    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}


void LCD_DrawRectangleBuff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,w,h;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=height>>8;
   pLcd[k++]=height;

   for(h=0;h<height;++h)
   {
      for(w=0;w<width;++w)
      {
      	if((w==0)||(w==width-1)||(h==0)||(h==height-1))
         {
      		pLcd[k++]=frameColor>>0;
      		pLcd[k++]=frameColor>>8;
      		pLcd[k++]=frameColor>>16;
         }
      	else
      	{
      		pLcd[k++]=fillColor>>0;
      		pLcd[k++]=fillColor>>8;
      		pLcd[k++]=fillColor>>16;

      	}
      }
   }
}

void LCD_DrawRoundRectangle1Buff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,w,h;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=height>>8;
   pLcd[k++]=height;

   for(h=0;h<height;++h)
   {
      for(w=0;w<width;++w)
      {
         if( ((w==0)      &&(h==0))       ||
      	     ((w==width-1)&&(h==0))       ||
      	     ((w==0)      &&(h==height-1))||
      	     ((w==width-1)&&(h==height-1))  )
         {
         	pLcd[k++]=BkColor>>0;
         	pLcd[k++]=BkColor>>8;
         	pLcd[k++]=BkColor>>16;
         }

         else if((w==0)||(w==width-1)||(h==0)||(h==height-1))
         {
         	pLcd[k++]=frameColor>>0;
         	pLcd[k++]=frameColor>>8;
         	pLcd[k++]=frameColor>>16;
         }
         else
         {
         	pLcd[k++]=fillColor>>0;
         	pLcd[k++]=fillColor>>8;
         	pLcd[k++]=fillColor>>16;
         }
      }
   }
}

void LCD_DrawBoldRectangleBuff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,w,h;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=height>>8;
   pLcd[k++]=height;

   for(h=0;h<height;++h)
   {
      for(w=0;w<width;++w)
      {
      	if((w==0)||(w==1)||(w==width-1)||(w==width-2)||(h==0)||(h==1)||(h==height-1)||(h==height-2))
         {
      		pLcd[k++]=frameColor>>0;
      		pLcd[k++]=frameColor>>8;
      		pLcd[k++]=frameColor>>16;
         }
      	else
      	{
      		pLcd[k++]=fillColor>>0;
      		pLcd[k++]=fillColor>>8;
      		pLcd[k++]=fillColor>>16;
      	}
      }
   }
}

void LCD_DrawBoldRoundRectangle1Buff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	   int k=posBuff,w,h;
	   pLcd[k++]=width>>8;
	   pLcd[k++]=width;
	   pLcd[k++]=height>>8;
	   pLcd[k++]=height;

	   for(h=0;h<height;++h)
	   {
	      for(w=0;w<width;++w)
	      {
	         if( ((w==0)      &&(h==0))       ||
	      	     ((w==width-1)&&(h==0))       ||
	      	     ((w==0)      &&(h==height-1))||
	      	     ((w==width-1)&&(h==height-1))  )
	         {
	         	pLcd[k++]=BkColor>>0;
	         	pLcd[k++]=BkColor>>8;
	         	pLcd[k++]=BkColor>>16;
	         }
	         else if((w==0)||(w==1)||(w==width-1)||(w==width-2)||(h==0)||(h==1)||(h==height-1)||(h==height-2))
	         {
	         	pLcd[k++]=frameColor>>0;
	         	pLcd[k++]=frameColor>>8;
	         	pLcd[k++]=frameColor>>16;
	         }
	         else
	         {
	         	pLcd[k++]=fillColor>>0;
	         	pLcd[k++]=fillColor>>8;
	         	pLcd[k++]=fillColor>>16;
	         }
	      }
	   }
}

void LCD_DrawCheckedBuff(int posBuff, int width, uint32_t fillColor,uint32_t bkColor)
{
   int k=posBuff,w,h,height=width/2;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=(height+1)>>8;
   pLcd[k++]=height+1;

   for(h=0;h<height+1;++h)
   {
      for(w=0;w<width;++w)
      {
    	 if((w>-1+h)&&(w<width-h))
         {
        	 if(((w==h)||(w==width-h-1))||(h==0))
        	 {
        		 if(((w<=width/2)&&(h<=height/2))||(h==0))
        		 {
        			pLcd[k++]=bkColor>>0;
        			pLcd[k++]=bkColor>>8;
        			pLcd[k++]=bkColor>>16;
        		 }
        		 else
        		 {
        			pLcd[k++]=fillColor>>0;
        			pLcd[k++]=fillColor>>8;
        			pLcd[k++]=fillColor>>16;
        		 }
        	 }
        	 else
        	 {
        		pLcd[k++]=bkColor>>0;
        		pLcd[k++]=bkColor>>8;
        		pLcd[k++]=bkColor>>16;
        	 }
         }
    	 else
    	 {
    		pLcd[k++]=bkColor>>0;
    		pLcd[k++]=bkColor>>8;
    		pLcd[k++]=bkColor>>16;
    	 }
      }
   }
}
void LCD_DrawBoldCheckedBuff(int posBuff, int width, uint32_t fillColor,uint32_t bkColor)
{
	   int k=posBuff,w,h,height=width/2;
	   pLcd[k++]=width>>8;
	   pLcd[k++]=width;
	   pLcd[k++]=height>>8;
	   pLcd[k++]=height;

	   for(h=0;h<height+1;++h)
	   {
	      for(w=0;w<width;++w)
	      {
	    	 if((w>-1+h)&&(w<width-h))
	         {
	        	 if( (((w==h)||(w==width-h-1))||(h==0)) || (((w==h+1)||(w==width-h-2))||(h==1)) )
	        	 {
	        		 if(((w<=width/2)&&(h<=height/2))||(h==0)||(h==1))
	        		 {
	        			pLcd[k++]=bkColor>>0;
	        			pLcd[k++]=bkColor>>8;
	        			pLcd[k++]=bkColor>>16;
	        		 }
	        		 else
	        		 {
	        			pLcd[k++]=fillColor>>0;
	        			pLcd[k++]=fillColor>>8;
	        			pLcd[k++]=fillColor>>16;
	        		 }
	        	 }
	        	 else
	        	 {
	        		pLcd[k++]=bkColor>>0;
	        		pLcd[k++]=bkColor>>8;
	        		pLcd[k++]=bkColor>>16;
	        	 }
	         }
        	 else
        	 {
        		pLcd[k++]=bkColor>>0;
        		pLcd[k++]=bkColor>>8;
        		pLcd[k++]=bkColor>>16;
        	 }
	      }
	   }
}

void LCD_DrawDownTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,w,h,height=width/2;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=(height+1)>>8;
   pLcd[k++]=height+1;

   for(h=0;h<height+1;++h)
   {
      for(w=0;w<width;++w)
      {
    	 if((w>-1+h)&&(w<width-h))
         {
        	 if(((w==h)||(w==width-h-1))||(h==0))
        	 {
        		pLcd[k++]=frameColor>>0;
        		pLcd[k++]=frameColor>>8;
        		pLcd[k++]=frameColor>>16;
        	 }
        	 else
        	 {
        		pLcd[k++]=fillColor>>0;
        		pLcd[k++]=fillColor>>8;
        		pLcd[k++]=fillColor>>16;
        	 }
         }
    	 else
    	 {
    		pLcd[k++]=BkColor>>0;
    		pLcd[k++]=BkColor>>8;
    		pLcd[k++]=BkColor>>16;
    	 }
      }
   }
}

void LCD_DrawBoldDownTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,w,h,height=width/2;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=height>>8;
   pLcd[k++]=height;

   for(h=0;h<height+1;++h)
   {
      for(w=0;w<width;++w)
      {
      	if((w>-1+h)&&(w<width-h))
         {
      		if( (((w==h)||(w==width-h-1))||(h==0)) || (((w==h+1)||(w==width-h-2))||(h==1)) )
      		{
      			pLcd[k++]=frameColor>>0;
      			pLcd[k++]=frameColor>>8;
      			pLcd[k++]=frameColor>>16;
      		}
      		else
      		{
      			pLcd[k++]=fillColor>>0;
      			pLcd[k++]=fillColor>>8;
      			pLcd[k++]=fillColor>>16;
      		}
         }
    	 else
    	 {
    		 pLcd[k++]=BkColor>>0;
    		 pLcd[k++]=BkColor>>8;
    		 pLcd[k++]=BkColor>>16;
    	 }
      }
   }
}

void LCD_DrawUpTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,w,h,height=width/2;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=(height+1)>>8;
   pLcd[k++]=height+1;

   for(h=height;h>-1;--h)
   {
      for(w=0;w<width;++w)
      {
    	 if((w>-1+h)&&(w<width-h))
         {
        	 if(((w==h)||(w==width-h-1))||(h==0))
        	 {
        		pLcd[k++]=frameColor>>0;
        		pLcd[k++]=frameColor>>8;
        		pLcd[k++]=frameColor>>16;
        	 }
        	 else
        	 {
        		pLcd[k++]=fillColor>>0;
        		pLcd[k++]=fillColor>>8;
        		pLcd[k++]=fillColor>>16;
        	 }
         }
    	 else
    	 {
    		pLcd[k++]=BkColor>>0;
    		pLcd[k++]=BkColor>>8;
    		pLcd[k++]=BkColor>>16;
    	 }
      }
   }
}

void LCD_DrawRightTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k=posBuff, w, h;
	pLcd[k++]=(width/2+1)>>8;
	pLcd[k++]=(width/2+1);
	pLcd[k++]=(width+1)>>8;
	pLcd[k++]=(width+1);

	for (h=0; h<width+1; ++h)
	{
		for (w=0; w<width/2+1; ++w)
		{
			if (h<=width/2)
			{
				if (w<=h)
				{
					if ((w==h)||(w==0))
					{
						pLcd[k++]=frameColor>>0;
						pLcd[k++]=frameColor>>8;
						pLcd[k++]=frameColor>>16;
					}
					else
					{
						pLcd[k++]=fillColor>>0;
						pLcd[k++]=fillColor>>8;
						pLcd[k++]=fillColor>>16;
					}
				}
				else
				{
					pLcd[k++]=BkColor>>0;
					pLcd[k++]=BkColor>>8;
					pLcd[k++]=BkColor>>16;
				}

			}
			else
			{
				if (w<=width-h)
				{
					if ((w==width-h)||(w==0))
					{
						pLcd[k++]=frameColor>>0;
						pLcd[k++]=frameColor>>8;
						pLcd[k++]=frameColor>>16;
					}
					else
					{
						pLcd[k++]=fillColor>>0;
						pLcd[k++]=fillColor>>8;
						pLcd[k++]=fillColor>>16;
					}
				}
				else
				{
					pLcd[k++]=BkColor>>0;
					pLcd[k++]=BkColor>>8;
					pLcd[k++]=BkColor>>16;
				}
			}
		}
	}
}

void LCD_DrawLeftTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	int k=posBuff, w, h;
	pLcd[k++]=(width/2+1)>>8;
	pLcd[k++]=(width/2+1);
	pLcd[k++]=(width+1)>>8;
	pLcd[k++]=(width+1);

	for (h=0; h<width+1; ++h)
	{
		for (w=width/2; w>-1; --w)
		{
			if (h<=width/2)
			{
				if (w<=h)
				{
					if ((w==h)||(w==0))
					{
						pLcd[k++]=frameColor>>0;
						pLcd[k++]=frameColor>>8;
						pLcd[k++]=frameColor>>16;
					}
					else
					{
						pLcd[k++]=fillColor>>0;
						pLcd[k++]=fillColor>>8;
						pLcd[k++]=fillColor>>16;
					}
				}
				else
				{
					pLcd[k++]=BkColor>>0;
					pLcd[k++]=BkColor>>8;
					pLcd[k++]=BkColor>>16;
				}

			}
			else
			{
				if (w<=width-h)
				{
					if ((w==width-h)||(w==0))
					{
						pLcd[k++]=frameColor>>0;
						pLcd[k++]=frameColor>>8;
						pLcd[k++]=frameColor>>16;
					}
					else
					{
						pLcd[k++]=fillColor>>0;
						pLcd[k++]=fillColor>>8;
						pLcd[k++]=fillColor>>16;
					}
				}
				else
				{
					pLcd[k++]=BkColor>>0;
					pLcd[k++]=BkColor>>8;
					pLcd[k++]=BkColor>>16;
				}
			}
		}
	}
}

void LCD_DrawBoldUpTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,w,h,height=width/2;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=height>>8;
   pLcd[k++]=height;

   for(h=height;h>-1;--h)
   {
      for(w=0;w<width;++w)
      {
      	if((w>-1+h)&&(w<width-h))
         {
      		if( (((w==h)||(w==width-h-1))||(h==0)) || (((w==h+1)||(w==width-h-2))||(h==1)) )
      		{
      			pLcd[k++]=frameColor>>0;
      			pLcd[k++]=frameColor>>8;
      			pLcd[k++]=frameColor>>16;
      		}
      		else
      		{
      			pLcd[k++]=fillColor>>0;
      			pLcd[k++]=fillColor>>8;
      			pLcd[k++]=fillColor>>16;
      		}
         }
    	 else
    	 {
    		pLcd[k++]=BkColor>>0;
    		pLcd[k++]=BkColor>>8;
    		pLcd[k++]=BkColor>>16;
    	 }
      }
   }
}

void LCD_DrawRightDownTriangle_xPosFillBuff(char *pImg, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int xPosFill)
{
   int k=4,w,h,height=width;
   pImg[0]=width>>8;
   pImg[1]=width;
   pImg[2]=height>>8;
   pImg[3]=height;

   for(h=0;h<width;++h)
   {
	  for(w=0;w<width;++w)
      {
		 if(w+h>=width)
         {
			 if(((w+h==width)||(w==width-1))||(h==width-1))
        	 {
        		 pImg[k++]=frameColor>>0;
            	 pImg[k++]=frameColor>>8;
                 pImg[k++]=frameColor>>16;
        	 }
        	 else
        	 {
        		 if(w<=xPosFill)
        		 {
            		 pImg[k++]=fillColor>>0;
                	 pImg[k++]=fillColor>>8;
                     pImg[k++]=fillColor>>16;
        		 }
        		 else
        		 {
            		 pImg[k++]=BkColor>>0;
                	 pImg[k++]=BkColor>>8;
                     pImg[k++]=BkColor>>16;
        		 }
        	 }
         }
    	 else
    	 {
        	 pImg[k++]=BkColor>>0;
        	 pImg[k++]=BkColor>>8;
             pImg[k++]=BkColor>>16;
    	 }
      }
   }
}

void LCD_DrawRoundRectangle3Buff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,wier,kol;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=height>>8;
   pLcd[k++]=height;

   for(wier=0;wier<height;++wier)
   {
      for(kol=0;kol<width;++kol)
      {
          if(((kol==4)      &&(wier==1))       ||
       	   ((kol==width-5)&&(wier==1))       ||
       	   ((kol==4)      &&(wier==height-2))||
       	   ((kol==width-5)&&(wier==height-2))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if( ((kol==4)      &&(wier<1))       ||
              	    ((kol==width-5)&&(wier<1))       ||
              	    ((kol==4)      &&(wier>height-2))||
              	    ((kol==width-5)&&(wier>height-2))  )
          {
                pLcd[k]=BkColor>>0; k++;
                pLcd[k]=BkColor>>8; k++;
                pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==3)      &&(wier==1))       ||
           	   ((kol==width-4)&&(wier==1))       ||
           	   ((kol==3)      &&(wier==height-2))||
           	   ((kol==width-4)&&(wier==height-2))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((kol==3)      &&(wier<1))       ||
           	   ((kol==width-4)&&(wier<1))       ||
           	   ((kol==3)      &&(wier>height-2))||
           	   ((kol==width-4)&&(wier>height-2))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==2)      &&(wier==2))       ||
           	   ((kol==width-3)&&(wier==2))       ||
           	   ((kol==2)      &&(wier==height-3))||
           	   ((kol==width-3)&&(wier==height-3))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((kol==2)      &&(wier<2))       ||
           	   ((kol==width-3)&&(wier<2))       ||
           	   ((kol==2)      &&(wier>height-3))||
           	   ((kol==width-3)&&(wier>height-3))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==1)      &&(wier==3))       ||
           	   ((kol==width-2)&&(wier==3))       ||
           	   ((kol==1)      &&(wier==height-4))||
           	   ((kol==width-2)&&(wier==height-4))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((kol==1)      &&(wier<3))       ||
           	   ((kol==width-2)&&(wier<3))       ||
           	   ((kol==1)      &&(wier>height-4))||
           	   ((kol==width-2)&&(wier>height-4))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==1)      &&(wier<3))       ||
           	   ((kol==width-2)&&(wier<3))       ||
           	   ((kol==1)      &&(wier>height-4))||
           	   ((kol==width-2)&&(wier>height-4))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==1)      &&(wier==4))       ||
           	   ((kol==width-2)&&(wier==4))       ||
           	   ((kol==1)      &&(wier==height-5))||
           	   ((kol==width-2)&&(wier==height-5))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((kol==1)      &&(wier<4))       ||
           	   ((kol==width-2)&&(wier<4))       ||
           	   ((kol==1)      &&(wier>height-5))||
           	   ((kol==width-2)&&(wier>height-5))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==0)      &&(wier<5))       ||
           	   ((kol==width-1)&&(wier<5))       ||
           	   ((kol==0)      &&(wier>height-6))||
           	   ((kol==width-1)&&(wier>height-6))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol>4)&&(kol<width-5))&&((wier==0)||(wier==height-1)))
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((wier>4)&&(wier<height-5))&&((kol==0)||(kol==width-1)))
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else
          {
               pLcd[k]=fillColor>>0; k++;
               pLcd[k]=fillColor>>8; k++;
               pLcd[k]=fillColor>>16; k++;
          }
      }
   }
}

void LCD_DrawBoldRoundRectangle3Buff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
   int k=posBuff,wier,kol;
   pLcd[k++]=width>>8;
   pLcd[k++]=width;
   pLcd[k++]=height>>8;
   pLcd[k++]=height;

   for(wier=0;wier<height;++wier)
   {
      for(kol=0;kol<width;++kol)
      {
          if(((kol==4)      &&(wier==1))       ||
       	   ((kol==width-5)&&(wier==1))       ||
       	   ((kol==4)      &&(wier==height-2))||
       	   ((kol==width-5)&&(wier==height-2))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if( ((kol==4)      &&(wier<1))       ||
              	    ((kol==width-5)&&(wier<1))       ||
              	    ((kol==4)      &&(wier>height-2))||
              	    ((kol==width-5)&&(wier>height-2))  )
          {
                pLcd[k]=BkColor>>0; k++;
                pLcd[k]=BkColor>>8; k++;
                pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==3)      &&((wier==1)||(wier==2)))       ||
        		  ((kol==width-4)&&((wier==1)||(wier==2)))       ||
				  ((kol==3)      &&((wier==height-2)||(wier==height-3)))||
				  ((kol==width-4)&&((wier==height-2)||(wier==height-3)))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((kol==3)      &&(wier<1))       ||
           	   ((kol==width-4)&&(wier<1))       ||
           	   ((kol==3)      &&(wier>height-2))||
           	   ((kol==width-4)&&(wier>height-2))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==2)      &&((wier==2)||(wier==3)))       ||
           	   ((kol==width-3)&&((wier==2)||(wier==3)))       ||
           	   ((kol==2)      &&((wier==height-3)||(wier==height-4)))||
           	   ((kol==width-3)&&((wier==height-3)||(wier==height-4)))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((kol==2)      &&(wier<2))       ||
           	   ((kol==width-3)&&(wier<2))       ||
           	   ((kol==2)      &&(wier>height-3))||
           	   ((kol==width-3)&&(wier>height-3))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==1)      &&(wier==3))       ||
           	   ((kol==width-2)&&(wier==3))       ||
           	   ((kol==1)      &&(wier==height-4))||
           	   ((kol==width-2)&&(wier==height-4))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((kol==1)      &&(wier<3))       ||
           	   ((kol==width-2)&&(wier<3))       ||
           	   ((kol==1)      &&(wier>height-4))||
           	   ((kol==width-2)&&(wier>height-4))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==1)      &&(wier<3))       ||
           	   ((kol==width-2)&&(wier<3))       ||
           	   ((kol==1)      &&(wier>height-4))||
           	   ((kol==width-2)&&(wier>height-4))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==1)      &&(wier==4))       ||
           	   ((kol==width-2)&&(wier==4))       ||
           	   ((kol==1)      &&(wier==height-5))||
           	   ((kol==width-2)&&(wier==height-5))  )
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((kol==1)      &&(wier<4))       ||
           	   ((kol==width-2)&&(wier<4))       ||
           	   ((kol==1)      &&(wier>height-5))||
           	   ((kol==width-2)&&(wier>height-5))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol==0)      &&(wier<5))       ||
           	   ((kol==width-1)&&(wier<5))       ||
           	   ((kol==0)      &&(wier>height-6))||
           	   ((kol==width-1)&&(wier>height-6))  )
          {
               pLcd[k]=BkColor>>0; k++;
               pLcd[k]=BkColor>>8; k++;
               pLcd[k]=BkColor>>16; k++;
          }
          else if(((kol>4)&&(kol<width-5))&&((wier==0)||(wier==height-1)||(wier==1)||(wier==height-2)))
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else if(((wier>4)&&(wier<height-5))&&((kol==0)||(kol==1)||(kol==width-1)||(kol==width-2)))
          {
               pLcd[k]=frameColor>>0; k++;
               pLcd[k]=frameColor>>8; k++;
               pLcd[k]=frameColor>>16; k++;
          }
          else
          {
               pLcd[k]=fillColor>>0; k++;
               pLcd[k]=fillColor>>8; k++;
               pLcd[k]=fillColor>>16; k++;
          }
      }
   }
}

void LCD_ImageComposition(int posDstImg, int posSrcImg, int posX, int posY)
{
   int h,w;
   int widthDest=256*pLcd[posDstImg]+pLcd[posDstImg+1];
  // int heightDest=256*dstImg[2]+dstImg[3];

   int widthSrc=256*pLcd[posSrcImg+0]+pLcd[posSrcImg+1];
   int heightSrc=256*pLcd[posSrcImg+2]+pLcd[posSrcImg+3];

   for(h=0;h<heightSrc;++h)
   {
      for(w=0;w<3*widthSrc;++w)
      {
         if((pLcd[4+posSrcImg+w+3*h*widthSrc+0]==0)&&(pLcd[4+posSrcImg+w+3*h*widthSrc+1]==0)&&(pLcd[4+posSrcImg+w+3*h*widthSrc+2]==0)) w+=2;
         else
         {
            pLcd[4+posDstImg+3*posX+3*widthDest*posY + w+3*h*widthDest]=pLcd[4+posSrcImg+w+3*h*widthSrc];  w++;
            pLcd[4+posDstImg+3*posX+3*widthDest*posY + w+3*h*widthDest]=pLcd[4+posSrcImg+w+3*h*widthSrc];   w++;
            pLcd[4+posDstImg+3*posX+3*widthDest*posY + w+3*h*widthDest]=pLcd[4+posSrcImg+w+3*h*widthSrc];
         }
      }
   }
}

void BSP_LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;

  rad1 = XRadius;
  rad2 = YRadius;

  k = (float)(rad2/rad1);

  do
  {
    BSP_LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos+y), (2*(uint16_t)(x/k) + 1));
    BSP_LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos-y), (2*(uint16_t)(x/k) + 1));

    e2 = err;
    if (e2 <= x)
    {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}

//#define ABS(X)  ((X) > 0 ? (X) : -(X))
//
//#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
//#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y))

//static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
//{
//  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
//  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
//  curpixel = 0;
//
//  deltax = ABS(x2 - x1);        /* The difference between the x's */
//  deltay = ABS(y2 - y1);        /* The difference between the y's */
//  x = x1;                       /* Start x off at the first pixel */
//  y = y1;                       /* Start y off at the first pixel */
//
//  if (x2 >= x1)                 /* The x-values are increasing */
//  {
//    xinc1 = 1;
//    xinc2 = 1;
//  }
//  else                          /* The x-values are decreasing */
//  {
//    xinc1 = -1;
//    xinc2 = -1;
//  }
//
//  if (y2 >= y1)                 /* The y-values are increasing */
//  {
//    yinc1 = 1;
//    yinc2 = 1;
//  }
//  else                          /* The y-values are decreasing */
//  {
//    yinc1 = -1;
//    yinc2 = -1;
//  }
//
//  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
//  {
//    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
//    yinc2 = 0;                  /* Don't change the y for every iteration */
//    den = deltax;
//    num = deltax / 2;
//    num_add = deltay;
//    num_pixels = deltax;         /* There are more x-values than y-values */
//  }
//  else                          /* There is at least one y-value for every x-value */
//  {
//    xinc2 = 0;                  /* Don't change the x for every iteration */
//    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
//    den = deltay;
//    num = deltay / 2;
//    num_add = deltax;
//    num_pixels = deltay;         /* There are more y-values than x-values */
//  }
//
//  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
//  {
//    BSP_LCD_DrawLine(x, y, x3, y3);
//
//    num += num_add;              /* Increase the numerator by the top of the fraction */
//    if (num >= den)             /* Check if numerator >= denominator */
//    {
//      num -= den;               /* Calculate the new numerator value */
//      x += xinc1;               /* Change the x as appropriate */
//      y += yinc1;               /* Change the y as appropriate */
//    }
//    x += xinc2;                 /* Change the x as appropriate */
//    y += yinc2;                 /* Change the y as appropriate */
//  }
//}

//void BSP_LCD_FillPolygon(uint16_t PointCount)
//{
//	PolygonPoint *Points;
//
//	  int16_t X = (Points+0)->X, Y = (Points+0)->Y, X2 = (Points+0)->X, Y2 = (Points+0)->Y, X_center = (Points+0)->X, Y_center = (Points+0)->Y, X_first = (Points+0)->X, Y_first = (Points+0)->Y, pixelX = (Points+0)->X, pixelY = (Points+0)->Y, counter = 0;
//	  uint16_t  image_left = 0, image_right = 0, image_top = 0, image_bottom = 0;
//
//	  image_left = image_right = Points->X;
//	  image_top= image_bottom = Points->Y;
//
//	  for(counter = 1; counter < PointCount; counter++)
//	  {
//	    pixelX = POLY_X(counter);
//	    if(pixelX < image_left)
//	    {
//	      image_left = pixelX;
//	    }
//	    if(pixelX > image_right)
//	    {
//	      image_right = pixelX;
//	    }
//
//	    pixelY = POLY_Y(counter);
//	    if(pixelY < image_top)
//	    {
//	      image_top = pixelY;
//	    }
//	    if(pixelY > image_bottom)
//	    {
//	      image_bottom = pixelY;
//	    }
//	  }
//
//	  if(PointCount < 2)
//	  {
//	    return;
//	  }
//
//	  X_center = (image_left + image_right)/2;
//	  Y_center = (image_bottom + image_top)/2;
//
//	  X_first = Points->X;
//	  Y_first = Points->Y;
//
//	  while(--PointCount)
//	  {
//	    X = Points->X;
//	    Y = Points->Y;
//	    Points++;
//	    X2 = Points->X;
//	    Y2 = Points->Y;
//
//	    FillTriangle(X, X2, X_center, Y, Y2, Y_center);
//	    FillTriangle(X, X_center, X2, Y, Y_center, Y2);
//	    FillTriangle(X_center, X2, X, Y_center, Y2, Y);
//	  }
//
//	  FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
//	  FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
//	  FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);
//}

bool TxtID_color_BufferWindow(int posBuff,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int widthWindow, uint32_t bkGroundColor, uint32_t fontColor)
{
	return LCD_TxtID_color_BufferWindow(fontID, Xpos,Ypos, txt, pLcd+posBuff+4,OnlyDigits, bkGroundColor,fontColor,widthWindow);
}

void LCD_Clear(uint32_t KolorTla )
{
	uint32_t i,k=0;
	for(i=0;i<800*480;i++)
	{
		pLcd[k++]=KolorTla>>0;
		pLcd[k++]=KolorTla>>8;
		pLcd[k++]=KolorTla>>16;
		//pLcd[i]=KolorTla;
	}
}

void LCD_ClearScreen(uint32_t KolorTla)
{
	for(int i=0;i<800*480;i++)
		pLcd_[i]=KolorTla;
}

bool LCD_DrawImage(char *imageName,int Xpos, int Ypos)
{
	return LCD_DrawImage_Buff(imageName,Xpos,Ypos,pLcd);
}

bool TxtID(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space)
{
	return LCD_TxtID(fontID, Xpos,Ypos, txt, pLcd,OnlyDigits, space);
}

bool TxtLineID(int fontID, int Xpos, int Ypos, char *txt, int lineWidth, int nrLine)
{
	return LCD_TxtLineID(fontID, Xpos,Ypos, txt, pLcd, lineWidth, nrLine);
}

#include "variables.h"
#define MAX_GSM_STRENGHT	30
void LCD_StrenghtGsmSign(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int gsmStrenght)
{
	if(gsmStrenght > MAX_GSM_STRENGHT) gsmStrenght=MAX_GSM_STRENGHT;
	LCD_DrawRightDownTriangle_xPosFill(x,y, width, frameColor, fillColor, BkColor, (GetVal(Var_gsmStrenght)*width)/MAX_GSM_STRENGHT);
}

void LCD_DrawRightDownTriangle_xPosFillIndirect(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int xPosFill)
{
	LCD_DrawRightDownTriangle_xPosFillBuff(pLcd, width, frameColor, fillColor, BkColor,xPosFill);
	BSP_LCD_DrawXXX(x,y,width,width,(uint8_t*)(pLcd+4));
}

void LCD_StrenghtGsmSignIndirect(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int gsmStrenght)
{
	if(gsmStrenght > MAX_GSM_STRENGHT) gsmStrenght=MAX_GSM_STRENGHT;
	LCD_DrawRightDownTriangle_xPosFillIndirect(x,y, width, frameColor, fillColor, BkColor, (GetVal(Var_gsmStrenght)*width)/MAX_GSM_STRENGHT);
}

void LCD_DrawRectangle(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t bkColor)
{
   uint32_t k,i,j,wier,kol;
    wier=0; kol=0;  j=0;

   k = 3*(y*LCD_GetXSize()+x);
   for(i=0;i<width*height;i++)
   {
       kol=j;
       if(kol==width){    k+=3*(LCD_GetXSize()-j);     wier++; kol=0; j=0;     }
       j++;

       if((kol==0)||(kol==width-1)||(wier==0)||(wier==height-1))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else
       {
            pLcd[k]=ColorWypel>>0; k++;
            pLcd[k]=ColorWypel>>8; k++;
            pLcd[k]=ColorWypel>>16; k++;
       }
   }
}

void LCD_DownTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	LCD_DrawDownTriangleBuff(0, width, frameColor, fillColor, BkColor);
	BSP_LCD_DrawXXX(Xpos,Ypos,width,width/2+1,(uint8_t*)(pLcd+4));
}

void LCD_BoldDownTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	LCD_DrawBoldDownTriangleBuff(0, width, frameColor, fillColor, BkColor);
	BSP_LCD_DrawXXX(Xpos,Ypos,width,width/2+1,(uint8_t*)(pLcd+4));
}

void LCD_BoldUpTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	LCD_DrawBoldUpTriangleBuff(0, width, frameColor, fillColor, BkColor);
	BSP_LCD_DrawXXX(Xpos,Ypos,width,width/2+1,(uint8_t*)(pLcd+4));
}

void LCD_DrawRectangleIndirect(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel)
{
   uint32_t k,i,j,wier,kol;
    wier=0; kol=0;  j=0;

   k=0;
   for(i=0;i<width*height;i++)
   {
       kol=j;
       if(kol==width){    k+=3*(width-j);     wier++; kol=0; j=0;     }
       j++;

       if((kol==0)||(kol==width-1)||(wier==0)||(wier==height-1))
       {
            pLcd[k]=ColorObram>>0; k++;
            pLcd[k]=ColorObram>>8; k++;
            pLcd[k]=ColorObram>>16; k++;
       }
       else
       {
            pLcd[k]=ColorWypel>>0; k++;
            pLcd[k]=ColorWypel>>8; k++;
            pLcd[k]=ColorWypel>>16; k++;
       }
   }
   BSP_LCD_DrawXXX(x,y,width,height,(uint8_t*)pLcd);
}






//POPRAW WIDTH i HEIGHT na dwoch bajtach !!!!
// Uwglednij rozmiar elementów dla zapisu typu &pLcd[500000]



void LCD_UpTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	LCD_DrawUpTriangleBuff(0, width, frameColor, fillColor, BkColor);
	BSP_LCD_DrawXXX(Xpos,Ypos,width,width/2+1,(uint8_t*)(pLcd+4));
}




void LCD_RightTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	LCD_DrawRightTriangleBuff(0, width, frameColor, fillColor, BkColor);
	BSP_LCD_DrawXXX(Xpos,Ypos,width/2+1,width+1,(uint8_t*)(pLcd+4));
}
void LCD_LeftTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor)
{
	LCD_DrawLeftTriangleBuff(0, width, frameColor, fillColor, BkColor);
	BSP_LCD_DrawXXX(Xpos,Ypos,width/2+1,width+1,(uint8_t*)(pLcd+4));
}




void SetPolygonPoint(uint16_t x, uint16_t y, uint16_t PointCount)
{
	Point[PointCount].X=x;
	Point[PointCount].Y=y;
}

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
  curpixel = 0;		uint32_t posLcd;

  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */

  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* There are more y-values than x-values */
  }

  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
   // BSP_LCD_DrawPixel(x, y, WHITE);   /* Draw the current pixel */
	  posLcd=3*(x+y*LCD_GetXSize());
	  pLcd[posLcd+0]=color;
	  pLcd[posLcd+1]=color>>8;
	  pLcd[posLcd+2]=color>>16;

    num += num_add;                            /* Increase the numerator by the top of the fraction */
    if (num >= den)                           /* Check if numerator >= denominator */
    {
      num -= den;                             /* Calculate the new numerator value */
      x += xinc1;                             /* Change the x as appropriate */
      y += yinc1;                             /* Change the y as appropriate */
    }
    x += xinc2;                               /* Change the x as appropriate */
    y += yinc2;                               /* Change the y as appropriate */
  }
}

void LCD_DrawPolygon(uint16_t PointCount, uint32_t color)
{
  PolygonPoint *Points=Point;
  int16_t x = 0, y = 0;

  if(PointCount < 2) return;
  LCD_DrawLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y, color);
  while(--PointCount)
  {
    x = Points->X;
    y = Points->Y;
    Points++;
    LCD_DrawLine(x, y, Points->X, Points->Y, color);
  }
}

void LCD_DrawLineBuff(int posBuff, int areaWidth, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
  curpixel = 0;		uint32_t posLcd;

  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */

  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* There are more y-values than x-values */
  }

  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
   // BSP_LCD_DrawPixel(x, y, WHITE);   /* Draw the current pixel */
	  posLcd=3*(x+y*areaWidth);
	  pLcd[4+posBuff+posLcd+0]=color;
	  pLcd[4+posBuff+posLcd+1]=color>>8;
	  pLcd[4+posBuff+posLcd+2]=color>>16;

    num += num_add;                            /* Increase the numerator by the top of the fraction */
    if (num >= den)                           /* Check if numerator >= denominator */
    {
      num -= den;                             /* Calculate the new numerator value */
      x += xinc1;                             /* Change the x as appropriate */
      y += yinc1;                             /* Change the y as appropriate */
    }
    x += xinc2;                               /* Change the x as appropriate */
    y += yinc2;                               /* Change the y as appropriate */
  }
}

void LCD_DrawPolygonBuff(int posBuff,int width, uint16_t PointCount, uint32_t color)
{
  PolygonPoint *Points=Point;
  int16_t x = 0, y = 0;

  if(PointCount < 2) return;
  LCD_DrawLineBuff(posBuff,width, Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y, color);
  while(--PointCount)
  {
    x = Points->X;
    y = Points->Y;
    Points++;
    LCD_DrawLineBuff(posBuff,width, x, y, Points->X, Points->Y, color);
  }
}

void LCD_DrawThemeFrame(int howMuchFrame, int marinesBetweenFrame, uint32_t frameColor, uint32_t fillColor)
{
	int widthFrame=(LCD_GetXSize()-((howMuchFrame-1)*marinesBetweenFrame))/howMuchFrame;
	int width_p=0;
	for(int i=0;i<howMuchFrame;++i){
		LCD_DrawRectangle(width_p,0, widthFrame, LCD_GetYSize(), frameColor, fillColor,0);   width_p+=widthFrame+marinesBetweenFrame;
	}
}

void LCD_DrawChecked(uint32_t x,uint32_t y, uint32_t width, uint32_t color)
{
	int k,w,h,height=width/2;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<height+1;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if((w>-1+h)&&(w<width-h))
	         {
	        	 if(((w==h)||(w==width-h-1))||(h==0))
	        	 {
	        		 if(((w<=width/2)&&(h<=height/2))||(h==0)) k+=3;
	        		 else
	        		 {
		        		 pLcd[k++]=color>>0;
		        		 pLcd[k++]=color>>8;
		        		 pLcd[k++]=color>>16;
	        		 }
	        	 }
	        	 else k+=3;
	         }
	    	 else  k+=3;
	    }
	}
}

void LCD_DrawBoldChecked(uint32_t x,uint32_t y, uint32_t width, uint32_t color)
{
	int k,w,h,height=width/2;

	k = 3*(y*LCD_GetXSize()+x);
	for(h=0;h<height+1;++h)
	{
		if(h>0) k+=3*(LCD_GetXSize()-width);
		for(w=0;w<width;++w)
	    {
	    	 if((w>-1+h)&&(w<width-h))
	         {
	    		 if( (((w==h)||(w==width-h-1))||(h==0)) || (((w==h+1)||(w==width-h-2))||(h==1)) )
	        	 {
	    			 if(((w<=width/2)&&(h<=height/2))||(h==0)||(h==1)) k+=3;
	        		 else
	        		 {
		    			 pLcd[k++]=color>>0;
		        		 pLcd[k++]=color>>8;
		        		 pLcd[k++]=color>>16;
	        		 }
	        	 }
	        	 else k+=3;
	         }
	    	 else k+=3;
	    }
	}
}

void LCD_DrawCircle(int x0, int y0, int r)
{
	float alfa1, alfa2;    int x2,y2;   uint32_t aa=WHITE;
	float _r=(float)r;
	float _y=(float)y0-_r;
	float _x0=(float)x0;
	float _y0=(float)y0;

	for(float angle=0; angle<360; angle+=0.1)
	{
		alfa1=asin((_y-_y0)/r) * 180/3.14159;
		alfa2=alfa1-angle;

		y2= (int)(r*sin(alfa2*3.14159/180)+_y0);
		x2= (int)(r*cos(alfa2*3.14159/180)+_x0);

		pLcd[y2*3*LCD_GetXSize()+3*x2+0]=aa>>0;
		pLcd[y2*3*LCD_GetXSize()+3*x2+1]=aa>>8;
		pLcd[y2*3*LCD_GetXSize()+3*x2+2]=aa>>16;
	}
}

void LCD_DrawFillCircle(int x0, int y0, int r)
{
	float alfa1, alfa2;    int x2,y2;   uint32_t aa=WHITE;
	float _r=(float)r;
	float _y=(float)y0-_r;
	float _x0=(float)x0;
	float _y0=(float)y0;

	for(float angle=0; angle<360; angle+=0.1)
	{
		alfa1=asin((_y-_y0)/r) * 180/3.14159;
		alfa2=alfa1-angle;

		y2= (int)(r*sin(alfa2*3.14159/180)+_y0);
		x2= (int)(r*cos(alfa2*3.14159/180)+_x0);

		pLcd[y2*3*LCD_GetXSize()+3*x2+0]=aa>>0;
		pLcd[y2*3*LCD_GetXSize()+3*x2+1]=aa>>8;
		pLcd[y2*3*LCD_GetXSize()+3*x2+2]=aa>>16;
	}

	for(int x=x0-r; x<x0+r; x++)
	{
		for(int y=y0-r; y<y0+r; y++)
		{
			if( r*r >= (x0-x)*(x0-x)+(y0-y)*(y0-y ) )
			{
				pLcd[y*3*LCD_GetXSize()+3*x+0]=aa>>0;
				pLcd[y*3*LCD_GetXSize()+3*x+1]=aa>>8;
				pLcd[y*3*LCD_GetXSize()+3*x+2]=aa>>16;
			}
		}
	}
}

void LCD_ImageFromSDandDisplay(char *fileName)
{
   SDCardFileOpen(1,fileName,FA_READ); SDCardFileRead(1,pLcd,2000000,&bytesread);  SDCardFileClose(1);  //!!!!!!! przekroczony SDRAM trzeba *4 !!!!!
   BSP_LCD_DrawBitmap(0,0,(uint8_t*)pLcd);
}

void LCD_DrawWidthFrame(int *width, int howMuchFrame, int marinesBetweenFrame, uint32_t frameColor, uint32_t fillColor)
{
	int width_p=0;
	for(int i=0;i<howMuchFrame;++i){
		LCD_DrawRectangle(width_p,0, width[i], LCD_GetYSize(), frameColor, fillColor,0);   width_p+=width[i]+marinesBetweenFrame;
	}
}

bool LCD_DrawTxt(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int Xpos, int Ypos, char *txt)
{
	return LCD_DrawTxt_Buff(fontSize,fontStyle,backgroundColor,fontColor,Xpos,Ypos,txt,pLcd,0,0);
}

bool TxtID_BufferWindow(char *pBuff, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int widthWindow)
{
	return LCD_TxtID_BufferWindow(fontID, Xpos,Ypos, txt, pBuff+4, OnlyDigits,widthWindow);
}

int LCD_DrawAnimation(char *folderPathName,int animationSpeed, int Xpos, int Ypos)
{
	return LCD_DrawAnimationIndirect(folderPathName,animationSpeed,Xpos,Ypos);
}

void LCD_DisplayBuff(int posBuff, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height)
{
	BSP_LCD_DrawXXX(Xpos,Ypos,width,height, (uint8_t*)(pLcd+posBuff));
}

void LCD_Display(int posBuff, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height)
{
	BSP_LCD_Display(Xpos,Ypos,width,height, pLcd_+posBuff );
}

bool LCD_DrawBmpBuff(int posBuff,char *imageName)
{
	return LCD_DrawBmpBuff_(imageName, pLcd+posBuff);
}

