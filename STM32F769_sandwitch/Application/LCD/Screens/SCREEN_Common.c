/*
 * common.c
 *
 *  Created on: 02.02.2021
 *      Author: Elektronika RM
 */
#include "SCREEN_Common.h"
#include "fonts_images.h"
#include "task.h"
#include "LCD_SelectID.h"
#include "LCD_BasicGaphics.h"
#include "LCD_Hardware.h"
#include "SCREEN_ReadPanel.h"
#include "SCREEN_Keyboard.h"
#include "mini_printf.h"
#include "sdram.h"

#define VAL_FONT	fontID_1
#define UNIT_FONT	fontID_2
#define DESCR_FONT	fontID_3
#define VAL_SLIDER_FONT	fontID_4
#define VAL_FONT_RED	fontID_5
#define VAL_FONT_BLUE	fontID_6
#define HEADER_FONT		fontID_7

#define MAX_ELEMENTS_REFRESH_IN_SCREEN	40

int touchPressed;

LCD_Screen Screen;
portTickType refreshScreenVar[MAX_ELEMENTS_REFRESH_IN_SCREEN];

char SelectTXT_MainMenu[]="\
Menu,BBBB,\
Panel odczytowy,AAAA,\
Konfiguracja portów,1111,\
Ustawienia sieci LAN,222,\
Ustawienia sieci WiFi,33333,\
Ustawienia sieci GSM,44444444444,\
Ustawienie czasu,555,\
Numery telefonów,6,\
Skrzynki e-mail,7777,\
Operacje logiczne,888,\
Rejestrator,9999,\
Rotacja,00000000000000000,\
";

#define MAX_BUFTEMP	500
#define MAX_PAINT_BUFF	100
#define MAX_POINTERS_PAINT_BUFF	MAX_PAINT_BUFF/10

static char bufTemp[MAX_BUFTEMP];
SRAM static int paintBuff[MAX_PAINT_BUFF];
SRAM static int firstFreePointer2PaintBuff;
SRAM static int pPaintBuff[MAX_POINTERS_PAINT_BUFF];

int varTemp=10;

int ID_touchDetect, ID_previousTouchDetect=-1;

void ClearAllPaints(void)
{
	firstFreePointer2PaintBuff=0;
	for(int i=0;i<MAX_PAINT_BUFF;++i)
		paintBuff[i]=0;
	for(int i=0;i<MAX_POINTERS_PAINT_BUFF;++i)
		pPaintBuff[i]=0;
}

void LCD_LoadPaint(int paintID, ...)
{
	int *p = paintBuff + firstFreePointer2PaintBuff;
	int len;
	va_list va;
	va_start(va,paintID);

	switch(paintID)
	{
	case PAINT_StrenghtGsmSign: len=7; break;
	case PAINT_StrenghtGsmSign2: len=7; break;
	case PAINT_StrenghtGsmSign3: len=7; break;
	}

	if((firstFreePointer2PaintBuff+len>=MAX_PAINT_BUFF)||(paintID>=MAX_POINTERS_PAINT_BUFF))
		return;

	switch(paintID)
	{
	case PAINT_StrenghtGsmSign:
	case PAINT_StrenghtGsmSign2:
	case PAINT_StrenghtGsmSign3:
		for(int i=0;i<len;i++)
			*(p+i)=va_arg(va,int);
		LCD_StrenghtGsmSign(*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),GetVal(*(p+6)));
		pPaintBuff[paintID]=firstFreePointer2PaintBuff;
		firstFreePointer2PaintBuff+=len;
		break;
	}

	va_end(va);
}

void LCD_DrawPaint(int paintID)
{
	if(paintID>=MAX_POINTERS_PAINT_BUFF)
			return;

	int *p = paintBuff + pPaintBuff[paintID];

	switch(paintID)
	{
	case PAINT_StrenghtGsmSign:
	case PAINT_StrenghtGsmSign2:
	case PAINT_StrenghtGsmSign3:
		LCD_StrenghtGsmSignIndirect(*p,*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),GetVal(*(p+6)));
		break;
	}
}

char* Temp2str(int value)
{
	mini_snprintf(bufTemp,MAX_BUFTEMP,"+%02d.%d",value/10, value-10*(value/10));
	return bufTemp;
}

void AllRefreshScreenClear(void)
{
	for(int i=0; i<MAX_ELEMENTS_REFRESH_IN_SCREEN; ++i)
		refreshScreenVar[i] = xTaskGetTickCount();
}
void RefreshScreenClear(int nrRefresh)
{
	refreshScreenVar[nrRefresh] = xTaskGetTickCount();
}
int IsRefreshScreenTimeout(int nrRefresh, int timeout)
{
	if((xTaskGetTickCount()-refreshScreenVar[nrRefresh])>timeout)
	{
		RefreshScreenClear(nrRefresh);
		return 1;
	}
	else return 0;
}

void SCREEN_Load(int nrScreen,  int param, int param2)
{
	switch(nrScreen)
	{
	  case MainPanel:
		  SCREEN_MainPanel(0, 3*param+FONT_8_bold, param2);
		 // SCREEN_ReadPanel(0, 3*param+FONT_8_bold, param2);
		  break;
	  case SCREEN_Wifi:
		  SCREEN_WifiSet(0, param, param2);
		  break;
	  case SCREEN_Keyboard:
		  SCREEN_KeyboardSet(0, param,param2);
		  break;
	  case SCREEN_Video:
		  break;
	}
	Screen.current=nrScreen;
	Screen.param=param;
	AllRefreshScreenClear();
}

void SCREEN_Touch(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y)
{
	switch(Screen.current)
	{
	  case MainPanel:
		  TOUCH_MainPanel(ID_touchDetect,pressRelease,x,y);
		 // TOUCH_ReadPanel(ID_touchDetect,pressRelease,x,y);
		  break;
	  case SCREEN_Wifi:
		  TOUCH_WifiSet(ID_touchDetect,pressRelease,x,y);
		  break;
	  case SCREEN_Keyboard:
		  TOUCH_KeyboardSet(ID_touchDetect,pressRelease,x,y);
		  break;
	  case SCREEN_Video:
		  TOUCH_Video(ID_touchDetect,pressRelease,x,y);
		  break;

	}
}

void TouchService(void)//NIECH TOUCH NIE ZAJMU CZASU PROCESU !!!!!
{
	uint16_t x,y;

	ID_touchDetect=CheckTouch(&x,&y);
	if(ID_touchDetect!=-1)
	{
		ID_previousTouchDetect = ID_touchDetect;
		SCREEN_Touch(ID_touchDetect,PRESSED,x,y);
	}
	else
	{
		if(ID_previousTouchDetect!=-1)      //Service Released Touch
		{
			SCREEN_Touch(ID_previousTouchDetect,RELEASED,x,y);
			ID_previousTouchDetect=-1;
		}
	}
}



void RefreshData(void)  //refr TAM gdzie dant screenn!!!!!!!!!!!!!!!!! umiesc refresha
{
 int temp;
 if(IsSelectRoll())
 {
	switch(Screen.current)
	{
	case MainPanel:
	case SCREEN_Video:
		if(IsRefreshScreenTimeout(0,1000))
		{
			if(13>Screen.param)
			{
				varTemp++;
				if((varTemp>40)&&(varTemp<60))	temp=fontNoChange;
				else if(varTemp>=60)			temp=VAL_FONT_RED;
				else							temp=VAL_FONT_BLUE;
				LCD_Txt(19, Temp2str(varTemp), temp, halfHight,0);
			}
		}
		if(IsRefreshScreenTimeout(1,50))
		{
			temp=GetVal(Var_gsmStrenght);
			SetVal(Var_gsmStrenght, temp<30 ? temp+1: 0);
			//LCD_StrenghtGsmSignIndirect(285,4, 20, 0xFFFFFF, 0xBBBBBB, 0x404040, GetVal(Var_gsmStrenght));
			LCD_DrawPaint(PAINT_StrenghtGsmSign);
			LCD_DrawPaint(PAINT_StrenghtGsmSign2);
			LCD_DrawPaint(PAINT_StrenghtGsmSign3);
			LCD_Txt(18, int2str(bufTemp,GetVal(Var_gsmStrenght)), fontNoChange, halfHight,0);
		}
		break;

	case SCREEN_Keyboard:
		if(IsRefreshScreenTimeout(0,200))
		{
			KEYBOARD_CursorToggle();
		}
		break;

	case SCREEN_Wifi:
		LCD_DrawAnimation("Animation/butterfly1",10, 100, 150);
		LCD_DrawAnimation("Animation/butterfly2",100, 200, 350);
		break;
	}
 }
}

void LCD_Obraz_image1(void)
{
	  dbg("\r\nimage1 ");
	  LCD_ImageFromSDandDisplay("image00.bmp");
	  osDelay(2000); SCREEN_Load(Screen.current,Screen.param,1);
}
void LCD_Obraz_image2(void)
{
	  dbg("\r\nimage2 ");
	  LCD_ImageFromSDandDisplay("image01.bmp");
	  osDelay(2000); SCREEN_Load(Screen.current,Screen.param,1);
}
void LCD_Obraz_image3(void)
{
	  dbg("\r\nimage3 ");
	  LCD_ImageFromSDandDisplay("image03.bmp");
	  osDelay(2000); SCREEN_Load(Screen.current,Screen.param,1);
}
void LCD_Obraz_imageSS(void)
{
	  dbg("\r\ntest1 ");
	  LCD_ImageFromSDandDisplay("test1.bmp");
	  osDelay(2000); SCREEN_Load(Screen.current,Screen.param,1);
}
void LCD_Obraz_image4(void)
{
	  dbg("\r\niii4 ");
	  LCD_ImageFromSDandDisplay("image04.bmp");
	  osDelay(2000); SCREEN_Load(Screen.current,Screen.param,1);
}
void LCD_Obraz_image5(void)
{
	  dbg("\r\niii5 ");
	  LCD_ImageFromSDandDisplay("image05.bmp");
	  osDelay(1000); SCREEN_Load(Screen.current,Screen.param,1);
}

void LCD_FontsTest(int nrTest, int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
{
  int aa=0;
  LCD_Clear(0x000000);

 switch(nrTest)
 {
 case 0:
	 LCD_DeleteAllFontAndImages();
	 LCD_LoadFont(FONT_8, fontStyle, backgroundColor, fontColor, fontID_1,1);    LCD_LoadFont(FONT_8_bold, fontStyle, backgroundColor, fontColor, fontID_2,1);
	 LCD_LoadFont(FONT_9, fontStyle, backgroundColor, fontColor, fontID_3,1);    LCD_LoadFont(FONT_9_bold, fontStyle, backgroundColor, fontColor, fontID_4,1);
	 LCD_LoadFont(FONT_10, fontStyle, backgroundColor, fontColor, fontID_5,1);   LCD_LoadFont(FONT_10_bold, fontStyle, backgroundColor, fontColor, fontID_6,1);
	 LCD_LoadFont(FONT_11, fontStyle, backgroundColor, fontColor, fontID_7,1);   LCD_LoadFont(FONT_11_bold, fontStyle, backgroundColor, fontColor, fontID_8,1);
	 LCD_LoadFont(FONT_12, fontStyle, backgroundColor, fontColor, fontID_9,1);   LCD_LoadFont(FONT_12_bold, fontStyle, backgroundColor, fontColor, fontID_10,1);
	 LCD_LoadFont(FONT_14, fontStyle, backgroundColor, fontColor, fontID_11,1);  LCD_LoadFont(FONT_14_bold, fontStyle, backgroundColor, fontColor, fontID_12,1);
	 LCD_LoadFont(FONT_16, fontStyle, backgroundColor, fontColor, fontID_13,1);  LCD_LoadFont(FONT_16_bold, fontStyle, backgroundColor, fontColor, fontID_14,1);

	 for(int fontNr=fontID_1; fontNr<fontID_14+1; fontNr++){
		TxtID(fontNr, 0,aa, "Markielowski+40",0,0);		TxtID(fontNr+1, 400,aa, "Markielowski+40",0,0);    aa+=ReturnHeightFontID(fontNr++);
	 }
	 break;

 case 1:
	 LCD_DeleteAllFontAndImages();
	 LCD_LoadFont(FONT_18, fontStyle, backgroundColor, fontColor, fontID_1,1);  LCD_LoadFont(FONT_18_bold, fontStyle, backgroundColor, fontColor, fontID_2,1);
	 LCD_LoadFont(FONT_20, fontStyle, backgroundColor, fontColor, fontID_3,1);  LCD_LoadFont(FONT_20_bold, fontStyle, backgroundColor, fontColor, fontID_4,1);
	 LCD_LoadFont(FONT_22, fontStyle, backgroundColor, fontColor, fontID_5,1);  LCD_LoadFont(FONT_22_bold, fontStyle, backgroundColor, fontColor, fontID_6,1);
	 LCD_LoadFont(FONT_24, fontStyle, backgroundColor, fontColor, fontID_7,1);  LCD_LoadFont(FONT_24_bold, fontStyle, backgroundColor, fontColor, fontID_8,1);
	 LCD_LoadFont(FONT_26, fontStyle, backgroundColor, fontColor, fontID_9,1);  LCD_LoadFont(FONT_26_bold, fontStyle, backgroundColor, fontColor, fontID_10,1);
	 LCD_LoadFont(FONT_28, fontStyle, backgroundColor, fontColor, fontID_11,1);  LCD_LoadFont(FONT_28_bold, fontStyle, backgroundColor, fontColor, fontID_12,1);
	 LCD_LoadFont(FONT_36, fontStyle, backgroundColor, fontColor, fontID_13,1);  LCD_LoadFont(FONT_36_bold, fontStyle, backgroundColor, fontColor, fontID_14,1);

	 for(int fontNr=fontID_1; fontNr<fontID_14+1; fontNr++){
		TxtID(fontNr, 0,aa, "Markielowski+40",0,0);		TxtID(fontNr+1, 400,aa, "Markielowski+40",0,0);    aa+=ReturnHeightFontID(fontNr++);
	 }
	 break;

 case 2:
	 LCD_DeleteAllFontAndImages();
	 LCD_LoadFont(FONT_48, fontStyle, backgroundColor, fontColor, fontID_1,1);  LCD_LoadFont(FONT_48_bold, fontStyle, backgroundColor, fontColor, fontID_2,1);
	 LCD_LoadFont(FONT_72, fontStyle, backgroundColor, fontColor, fontID_3,1);  LCD_LoadFont(FONT_72_bold, fontStyle, backgroundColor, fontColor, fontID_4,1);
	 LCD_LoadFont(FONT_130, fontStyle, backgroundColor, fontColor, fontID_5,1); LCD_LoadFont(FONT_130_bold, fontStyle, backgroundColor, fontColor, fontID_6,1);

	 for(int fontNr=fontID_1; fontNr<fontID_6+1; fontNr++){
		TxtID(fontNr, 0,aa, "+23.50",0,0);		TxtID(fontNr+1, 400,aa, "+23.50",0,0);    aa+=ReturnHeightFontID(fontNr++);
	 }
	 break;
 }

 LCD_DisplayBuff(0,(uint32_t)0,(uint32_t)0,(uint32_t)LCD_GetXSize(),(uint32_t)LCD_GetYSize());

}

extern int prescVal;
void SCREEN_LoadFromAVI(void)
{
	SCREEN_Load(MainPanel, prescVal, 0);
}


#include "touch.h"
void FUNCTION_MainMenuSelect(int nr)
{
	switch (nr)  //dla ciekawosci zwieksz ilosc nrSel w MAIN
	{
	case 0:
		SCREEN_Load(MainPanel, prescVal, 0);
		break;
	case 1:
		LCD_Obraz_image2();
		break;
	case 2:
		LCD_Obraz_image3();
		break;
	case 3:
		SCREEN_Load(SCREEN_Wifi, FONT_12_bold, 0);
		break;
	case 4:
		LCD_Obraz_image5();
		break;
	case 5:

		DeleteAllTouch();
		ClearAllSelect();
		touchTemp.x_Start= 0;
		touchTemp.y_Start= 0;

		touchTemp.x_End= 400;
		touchTemp.y_End= 240;
		SetTouch(999);

		CreateAviTask();
		//AVI_test();
		SCREEN_Load(SCREEN_Video, Screen.param, 0);
		break;
	case 6:
		JPEG_test();
		osDelay(3000);
		SCREEN_Load(Screen.current, Screen.param, 1);
		break;
	case 7:
		Touchscreen_Calibration();
		SCREEN_Load(Screen.current, Screen.param, 1);
		break;
	case 8:
		LCD_Obraz_image3();
		break;
	}
}


//------------------------------------ Fonty probne ---------------------------------------
//#include "fonts.h"
//int F_znakkonwert(char zn,int w)
//{
//	int pp=0,x=0;
//	     if(w==12) x=12;
//	else if(w==16) x=32;
//	else if(w==20) x=40;
//	else if(w==24) x=72;
//	else if(w==8) x=8;
//
//
//       if(zn==' ') pp=0*x;
//  else if(zn=='!') pp=1*x;
//  else if(zn=='"') pp=2*x;
//  else if(zn=='#') pp=3*x;
//  else if(zn=='$') pp=4*x;
//  else if(zn=='%') pp=5*x;
//  else if(zn=='&') pp=6*x;
//  else if(zn=='`') pp=7*x;
//  else if(zn=='(') pp=8*x;
//  else if(zn==')') pp=9*x;
//  else if(zn=='*') pp=10*x;
//  else if(zn=='+') pp=11*x;
//  else if(zn==',') pp=12*x;
//  else if(zn=='-') pp=13*x;
//  else if(zn=='.') pp=14*x;
//  else if(zn=='/') pp=15*x;
//  else if(zn=='0') pp=16*x;
//  else if(zn=='1') pp=17*x;
//  else if(zn=='2') pp=18*x;
//  else if(zn=='3') pp=19*x;
//  else if(zn=='4') pp=20*x;
//  else if(zn=='5') pp=21*x;
//  else if(zn=='6') pp=22*x;
//  else if(zn=='7') pp=23*x;
//  else if(zn=='8') pp=24*x;
//  else if(zn=='9') pp=25*x;
//  else if(zn==':') pp=26*x;
//  else if(zn==';') pp=27*x;
//  else if(zn=='<') pp=28*x;
//  else if(zn=='=') pp=29*x;
//  else if(zn=='>') pp=30*x;
//  else if(zn=='?') pp=31*x;
//  else if(zn=='@') pp=32*x;
//  else if(zn=='A') pp=33*x;
//  else if(zn=='B') pp=34*x;
//  else if(zn=='C') pp=35*x;
//  else if(zn=='D') pp=36*x;
//  else if(zn=='E') pp=37*x;
//  else if(zn=='F') pp=38*x;
//  else if(zn=='G') pp=39*x;
//  else if(zn=='H') pp=40*x;
//  else if(zn=='I') pp=41*x;
//  else if(zn=='J') pp=42*x;
//  else if(zn=='K') pp=43*x;
//  else if(zn=='L') pp=44*x;
//  else if(zn=='M') pp=45*x;
//  else if(zn=='N') pp=46*x;
//  else if(zn=='O') pp=47*x;
//  else if(zn=='P') pp=48*x;
//  else if(zn=='Q') pp=49*x;
//  else if(zn=='R') pp=50*x;
//  else if(zn=='S') pp=51*x;
//  else if(zn=='T') pp=52*x;
//  else if(zn=='U') pp=53*x;
//  else if(zn=='V') pp=54*x;
//  else if(zn=='W') pp=55*x;
//  else if(zn=='X') pp=56*x;
//  else if(zn=='Y') pp=57*x;
//  else if(zn=='Z') pp=58*x;
//  else if(zn=='[') pp=59*x;
//  else if(zn=='\\') pp=60*x;
//  else if(zn==']') pp=61*x;
//  else if(zn=='^') pp=62*x;
//  else if(zn=='_') pp=63*x;
//  else if(zn=='`') pp=64*x;
//  else if(zn=='a') pp=65*x;
//  else if(zn=='b') pp=66*x;
//  else if(zn=='c') pp=67*x;
//  else if(zn=='d') pp=68*x;
//  else if(zn=='e') pp=69*x;
//  else if(zn=='f') pp=70*x;
//  else if(zn=='g') pp=71*x;
//  else if(zn=='h') pp=72*x;
//  else if(zn=='i') pp=73*x;
//  else if(zn=='j') pp=74*x;
//  else if(zn=='k') pp=75*x;
//  else if(zn=='l') pp=76*x;
//  else if(zn=='m') pp=77*x;
//  else if(zn=='n') pp=78*x;
//  else if(zn=='o') pp=79*x;
//  else if(zn=='p') pp=80*x;
//  else if(zn=='q') pp=81*x;
//  else if(zn=='r') pp=82*x;
//  else if(zn=='s') pp=83*x;
//  else if(zn=='t') pp=84*x;
//  else if(zn=='u') pp=85*x;
//  else if(zn=='v') pp=86*x;
//  else if(zn=='w') pp=87*x;
//  else if(zn=='x') pp=88*x;
//  else if(zn=='y') pp=89*x;
//  else if(zn=='z') pp=90*x;
//  else if(zn=='{') pp=91*x;
//  else if(zn=='|') pp=92*x;
//  else if(zn=='}') pp=93*x;
//  else if(zn=='~') pp=94*x;
//  else   pp=0*x;
//       return pp;
//}
//
//void F24(int x,int y,char zn,int color)
//{
//    int j,g,k,pp,m;   m=0;
//    pp=F_znakkonwert(zn,24);
//
//    for(g=0;g<24;g++){
//    	k = 3*((y+g)*800+x);
//     for(j=0;j<8;j++){
//       if(((Font24.table[pp+m]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//     m++;
//     for(j=0;j<8;j++){
//       if(((Font24.table[pp+m]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//     m++;
//     for(j=0;j<1;j++){
//       if(((Font24.table[pp+m]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//     m++;
//    }
//}
//void F20(int x,int y,char zn,int color)
//{
//    int j,g,k,pp,m;   m=0;
//    pp=F_znakkonwert(zn,20);
//
//    for(g=0;g<20;g++){
//    	k = 3*((y+g)*800+x);
//     for(j=0;j<8;j++){
//       if(((Font20.table[pp+m]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//     m++;
//     for(j=0;j<4;j++){
//       if(((Font20.table[pp+m]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//     m++;
//    }
//}
//void F16(int x,int y,char zn,int color)
//{
//    int j,g,k,pp,m;   m=0;
//    pp=F_znakkonwert(zn,16);
//
//    for(g=0;g<16;g++){
//    	k = 3*((y+g)*800+x);
//     for(j=0;j<8;j++){
//       if(((Font16.table[pp+m]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//     m++;
//     for(j=0;j<3;j++){
//       if(((Font16.table[pp+m]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//     m++;
//    }
//}
//void F12(int x,int y,char zn, int color)
//{
//    int j,g,k,pp;
//    pp=F_znakkonwert(zn,12);
//
//    for(g=0;g<12;g++){
//    	k = 3*((y+g)*800+x);
//     for(j=0;j<7;j++){
//       if(((Font12.table[pp+g]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//    }
//}
//void F8(int x,int y,char zn, int color)
//{
//    int j,g,k,pp;
//    pp=F_znakkonwert(zn,8);
//
//    for(g=0;g<8;g++){
//    	k = 3*((y+g)*800+x);
//     for(j=0;j<5;j++){
//       if(((Font8.table[pp+g]>>(7-j))&0x01)>0){  pLcd[k]=color>>0; k++;   pLcd[k]=color>>8; k++;    pLcd[k]=color>>16; k++; } else k+=3;
//     }
//    }
//}
//
//static void LCD_Txt_(int x,int y,char *txt,int size,int color)
//{
//    int i,j;
//    for(i=0;i<strlen(txt);i++)
//    {
//    	     if(size==12) F12(x+i*7,y,txt[i],color);
//    	else if(size==16) F16(x+i*11,y,txt[i],color);
//    	else if(size==20) F20(x+i*12,y,txt[i],color);
//    	else if(size==24) F24(x+i*16,y,txt[i],color);
//    	else if(size==8) F8(x+i*5,y,txt[i],color);
//    }
//}

//-------------------------------------------------------------------------

