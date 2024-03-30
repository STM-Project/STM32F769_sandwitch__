/*
 * common.h
 *
 *  Created on: 02.02.2021
 *      Author: Elektronika RM
 */

#ifndef LCD_SCREENS_SCREEN_COMMON_H_
#define LCD_SCREENS_SCREEN_COMMON_H_

#include <stdarg.h>
#include "freeRTOS.h"

#define TOUCH_OFFSET		10

typedef enum
{
	TXT_ID1, TXT_ID2, TXT_ID3, TXT_ID4, TXT_ID5, TXT_ID6, TXT_ID7, TXT_ID8, TXT_ID9, TXT_ID10,
	TXT_ID11, TXT_ID12, TXT_ID13, TXT_ID14, TXT_ID15, TXT_ID16, TXT_ID17, TXT_ID18, TXT_ID19, TXT_ID20,
	TXT_ID21, TXT_ID22, TXT_ID23, TXT_ID24, TXT_ID25, TXT_ID26, TXT_ID27, TXT_ID28, TXT_ID29, TXT_ID30,
	TXT_ID31, TXT_ID32, TXT_ID33, TXT_ID34, TXT_ID35, TXT_ID36, TXT_ID37, TXT_ID38, TXT_ID39, TXT_ID40
}LcdTxtVarID;

enum touchGeneral
{
  noTouch,   //ogolne touch
  TOUCH_mainMenu,  // i tu mozna wiecej ogolnych touch
  image_1=TOUCH_OFFSET,   //reszta do przeniesienia
  image_2,
  image_3,
  image_4,
  image_5,
  video,
  jpeg,
  bmp,
  slider0,
  slider0_left,
  slider0_right,
  slider1,
  slider1_left,
  slider1_right,
  slider2,
  slider2_up,
  slider2_down,
  touch_calibr,
  triangle_down1,
  triangle_down2,
  triangle_down3,
  triangle_up1,
  triangle_up2,
  triangle_up3,
  triangle_right,
  triangle_left,
  mainFrame_1
};

enum allScreens
{
  noParam,
  MainPanel,
  SCREEN_Wifi,
  SCREEN_Keyboard,
  SCREEN_Video,
  SC____

};

typedef enum{
	PAINT_StrenghtGsmSign,
	PAINT_StrenghtGsmSign2,
	PAINT_StrenghtGsmSign3
}LCD_Paints;

typedef struct
{
  int current;
  int param;
}LCD_Screen;

extern int touchPressed;

extern portTickType refreshScreenVar[];
extern LCD_Screen Screen;
extern char SelectTXT_MainMenu[];

void AllRefreshScreenClear(void);
void RefreshScreenClear(int nrRefresh);
int IsRefreshScreenTimeout(int nrRefresh, int timeout);

void SCREEN_Load(int nrScreen,  int param, int param2);
void SCREEN_Touch(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y);

void LCD_Obraz_image1(void);
void LCD_Obraz_image2(void);
void LCD_Obraz_image3(void);
void LCD_Obraz_imageSS(void);
void LCD_Obraz_image4(void);
void LCD_Obraz_image5(void);
void LCD_FontsTest(int nrTest, int fontStyle, uint32_t backgroundColor, uint32_t fontColor);
void FUNCTION_MainMenuSelect(int nr);
void LCD_LoadPaint(int paintID, ...);
void ClearAllPaints(void);

#endif /* LCD_SCREENS_SCREEN_COMMON_H_ */
