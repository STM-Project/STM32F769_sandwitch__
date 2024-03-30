/*
 * layer.h
 *
 *  Created on: 01.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_LAYER_H_
#define LCD_LAYER_H_

#include "stm32f7xx_hal.h"
#include <stdbool.h>

#define BLUE          ((uint32_t)0xFF0000FF)
#define GREEN         ((uint32_t)0xFF00FF00)
#define RED           ((uint32_t)0xFFFF0000)
#define CYAN          ((uint32_t)0xFF00FFFF)
#define MAGENTA       ((uint32_t)0xFFFF00FF)
#define YELLOW        ((uint32_t)0xFFFFFF00)
#define LIGHTBLUE     ((uint32_t)0xFF8080FF)
#define LIGHTGREEN    ((uint32_t)0xFF80FF80)
#define LIGHTRED      ((uint32_t)0xFFFF8080)
#define LIGHTCYAN     ((uint32_t)0xFF80FFFF)
#define LIGHTMAGENTA  ((uint32_t)0xFFFF80FF)
#define LIGHTYELLOW   ((uint32_t)0xFFFFFF80)
#define DARKBLUE      ((uint32_t)0xFF000080)
#define DARKGREEN     ((uint32_t)0xFF008000)
#define DARKRED       ((uint32_t)0xFF800000)
#define DARKCYAN      ((uint32_t)0xFF008080)
#define DARKMAGENTA   ((uint32_t)0xFF800080)
#define DARKYELLOW    ((uint32_t)0xFF808000)
#define WHITE         ((uint32_t)0xFFFFFFFF)
#define LIGHTGRAY     ((uint32_t)0xFFD0D0D0)
#define GRAY          ((uint32_t)0xFF808080)
#define DARKGRAY      ((uint32_t)0xFF404040)
#define BLACK         ((uint32_t)0xFF000000)
#define BROWN         ((uint32_t)0xFFA52A2A)
#define ORANGE        ((uint32_t)0xFFFFA500)
#define TRANSPARENT   ((uint32_t)0xFF000000)
#define GRAY1    	  ((uint32_t)0xFF404040)
#define MYBLUE    	  ((uint32_t)0xFF50C8FF)
#define MYLIGHTGRAY     ((uint32_t)0xFFB0B0B0)

typedef struct
{
	uint16_t X;
	uint16_t Y;
}PolygonPoint;

extern PolygonPoint Point[];
void LCD_DrawRoundRectangle3_TEST(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut);
void LCD_DrawBoldRectangle(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t bkColor);
void LCD_DrawRoundRectangle1(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut);
void LCD_DrawBoldRoundRectangle1(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut);
void LCD_DrawRoundRectangle2(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut);
void LCD_DrawBoldRoundRectangle2(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut);
void LCD_DrawRoundRectangle3(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut);
void LCD_DrawBoldRoundRectangle3(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t ColorInOut);
void LCD_DrawDownTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawRightUpTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawLeftTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawRightTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawRightDownTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawRightDownTriangle_xPosFill(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int xPosFill);
void LCD_DrawLeftDownTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawBoldDownTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawUpTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawBoldUpTriangle(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
bool TxtID_color(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, uint32_t bkGroundColor, uint32_t fontColor, int space);
bool TxtID_Var_color(int nrTxt, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, uint32_t bkGroundColor, uint32_t fontColor, int space);
bool LCD_DrawTxt_Var(int nrTxt,int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int Xpos, int Ypos, char *txt, int OnlyDigits);
bool TxtID_Var(int nrTxt, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space);
void LCD_SetTxtYPos(int nrTxt, int posY);
void LCD_SetTxtXPos(int nrTxt, int posX);
void LCD_Txt_AddY(int nrTxt, int Ypos);
int LCD_GetTxtXPos(int nrTxt);
int LCD_GetTxtYPos(int nrTxt);
void LCD_Txt_SubY(int nrTxt, int Ypos);
bool LCD_DrawTxtIndirect_color(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int Xpos, int Ypos, char *txt, int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int space);
bool LCD_DrawTxtIndirect(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int Xpos, int Ypos, char *txt, int OnlyDigits, int space);
void LCD_Txt(int nrTxt, char *txt, int otherFont, int OnlyDigits, int space);
void LCD_Txt_color(int nrTxt, char *txt, int otherFont, int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int space);

void BSP_LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void BSP_LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void BSP_LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius);

void LCD_DrawRectangleBuff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawRoundRectangle1Buff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawRoundRectangle3Buff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawBoldRoundRectangle3Buff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawUpTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawDownTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawBoldUpTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawBoldDownTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawRightTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawLeftTriangleBuff(int posBuff, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_ImageComposition(int posDstImg, int posSrcImg, int posX, int posY);
int LCD_LoadImage(char *pathName);
void LCD_DrawRightDownTriangle_xPosFillBuff(char *pImg, int width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int xPosFill);
void LCD_DrawBoldRectangleBuff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawBoldRoundRectangle1Buff(int posBuff, int width, int height, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_DrawCheckedBuff(int posBuff, int width, uint32_t fillColor,uint32_t bkColor);
void LCD_DrawBoldCheckedBuff(int posBuff, int width, uint32_t fillColor,uint32_t bkColor);
void BSP_LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius);
bool TxtID_color_BufferWindow(int posBuff,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int widthWindow, uint32_t bkGroundColor, uint32_t fontColor);
void LCD_Clear(uint32_t KolorTla );
void LCD_ClearScreen(uint32_t KolorTla);
bool LCD_DrawImage(char *imageName,int Xpos, int Ypos);
bool TxtID(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space);
bool TxtLineID(int fontID, int Xpos, int Ypos, char *txt, int lineWidth, int nrLine);
void LCD_StrenghtGsmSign(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int gsmStrenght);
void LCD_StrenghtGsmSignIndirect(uint32_t x,uint32_t y, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor, int gsmStrenght);
void LCD_DrawRectangle(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel, uint32_t bkColor);
void LCD_DownTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_BoldDownTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_BoldUpTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_UpTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_RightTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);
void LCD_LeftTriangleIndirect(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t frameColor, uint32_t fillColor, uint32_t BkColor);

void SetPolygonPoint(uint16_t x, uint16_t y, uint16_t PointCount);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void LCD_DrawPolygon(uint16_t PointCount, uint32_t color);
void LCD_DrawLineBuff(int posBuff, int areaWidth, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void LCD_DrawPolygonBuff(int posBuff,int width, uint16_t PointCount, uint32_t color);
void LCD_DrawRectangleIndirect(uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t ColorObram, uint32_t ColorWypel);
void LCD_DrawThemeFrame(int howMuchFrame, int marinesBetweenFrame, uint32_t frameColor, uint32_t fillColor);

void LCD_DrawChecked(uint32_t x,uint32_t y, uint32_t width, uint32_t color);
void LCD_DrawBoldChecked(uint32_t x,uint32_t y, uint32_t width, uint32_t color);

void LCD_DrawCircle(int x0, int y0, int r);
void LCD_DrawFillCircle(int x0, int y0, int r);
void LCD_Display(int posBuff, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height);
void LCD_ImageFromSDandDisplay(char *fileName);
void LCD_DrawWidthFrame(int *width, int howMuchFrame, int marinesBetweenFrame, uint32_t frameColor, uint32_t fillColor);
bool LCD_DrawTxt(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int Xpos, int Ypos, char *txt);
bool TxtID_BufferWindow(char *pBuff, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int widthWindow);
int LCD_DrawAnimation(char *folderPathName,int animationSpeed, int Xpos, int Ypos);

void LCD_DisplayBuff(int posBuff, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height);
bool LCD_DrawBmpBuff(int posBuff,char *imageName);

#endif /* LCD_LAYER_H_ */
