/*
 * fonts.h
 *
 *  Created on: 16.07.2020
 *      Author: Elektronika RM
 */

#ifndef LCD_FONTS_H_
#define LCD_FONTS_H_

#include "stm32f7xx_hal.h"
#include <stdbool.h>
//+-.,[]{}<>'~*()=%∞|?/!1234567890
//a b c d e f g h i j k l m n o p q r s t u v w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z + - . , : ; [ ] { } < > ' ~ * ( ) & # ^ = _ $ % ∞ @ | ? ! • π ∆ Ê   Í £ ≥ — Ò ” Û å ú è ü Ø ø / 1 2 3 4 5 6 7 8 9 0
//gdy czcionki nachodza na siebie np dla italics//a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  +  -  .  ,  :  ;  [  ]  {  }  <  >  '  ~  *  (  )  &  #  ^  =  _  $  %  ∞  @  |  ?  !  •  π  ∆  Ê     Í  £  ≥  —  Ò  ”  Û  å  ú  è  ü  Ø  ø  /  1  2  3  4  5  6  7  8  9  0

#define NOCONST_WIDTH	0
#define CONST_WIDTH	1

enum FontsSize
{
	FONT_8,
	FONT_8_bold,
	FONT_8_italics,
	FONT_9,
	FONT_9_bold,
	FONT_9_italics,
	FONT_10,
	FONT_10_bold,
	FONT_10_italics,
	FONT_11,
	FONT_11_bold,
	FONT_11_italics,
	FONT_12,
	FONT_12_bold,
	FONT_12_italics,
	FONT_14,
	FONT_14_bold,
	FONT_14_italics,
	FONT_16,
	FONT_16_bold,
	FONT_16_italics,
	FONT_18,
	FONT_18_bold,
	FONT_18_italics,
	FONT_20,
	FONT_20_bold,
	FONT_20_italics,
	FONT_22,
	FONT_22_bold,
	FONT_22_italics,
	FONT_24,
	FONT_24_bold,
	FONT_24_italics,
	FONT_26,
	FONT_26_bold,
	FONT_26_italics,
	FONT_28,
	FONT_28_bold,
	FONT_28_italics,
	FONT_36,
	FONT_36_bold,
	FONT_36_italics,
	FONT_48,
	FONT_48_bold,
	FONT_48_italics,
	FONT_72,
	FONT_72_bold,
	FONT_72_italics,
	FONT_130,
	FONT_130_bold,
	FONT_130_italics,
	FONT_36_dings2
};

enum FontsStyle
{
	Arial_Narrow,
	Calibri_Light,
	Bodoni_MT_Condenset,
	Comic_Saens_MS,
	Dings_2,
	Arial
};

enum ID_Fonts
{
	fullHight=-3,
	halfHight=-2,
	fontNoChange=-1,
	fontID_1,
	fontID_2,
	fontID_3,
	fontID_4,
	fontID_5,
	fontID_6,
	fontID_7,
	fontID_8,
	fontID_9,
	fontID_10,
	fontID_11,
	fontID_12,
	fontID_13,
	fontID_14,
	fontID_15,
	fontID_16,
	fontID_17
};

void InfoForImagesFonts(void);
int LCD_LoadFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, uint32_t fontID, int digitsCorrect);
bool LCD_DeleteFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor);
void LCD_DeleteAllFontAndImages(void);
bool LCD_DrawTxt_Buff(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int X, int Y, char *txt, char *LcdBuffer, int OnlyDigits, int space);
bool LCD_DrawTxt_BufferWindow(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer,int OnlyDigits, int widthWindow);
bool LCD_DrawTxt_Buff_color(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer,int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int space);
bool LCD_DrawTxt_color_BufferWindow(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer,int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int widthWindow);
bool LCD_DrawTxtLine_Buff(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, int X, int Y, char *txt, char *LcdBuffer, int lineWidth, int nrLine);
void DisplayFontsStructState(void);
bool LCD_DrawImage_Buff(char *imageName,int X, int Y, char *LcdBuffer);
bool LCD_DrawImageIndirect(char *imageName,int X, int Y);
bool LCD_DrawTxtIndirect_Buff(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int X, int Y, char *txt, char *LcdBuffer, int OnlyDigits, int space);
bool LCD_DrawTxtIndirect_Buff_color(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor,int X, int Y, char *txt, char *LcdBuffer, int OnlyDigits, uint32_t bkGroundColor, uint32_t FontColor, int space);
int LCD_DrawAnimationIndirect(char *folderPathName,int animationSpeed, int Xpos, int Ypos);
int LCD_LoadAnimation(char *folderPathName, int imagesNumber, int everyImage);
bool LCD_DrawBmpBuff_(char *imageName, char *LcdBuffer);
int32_t LCD_GetWidthHeightBmp(char *imageName);
int ReturnWidthFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, char *txt);
int ReturnHeightFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor);
bool LCD_TxtID(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int OnlyDigits, int space);
bool LCD_TxtID_BufferWindow(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int OnlyDigits, int widthWindow);
bool LCD_TxtID_color(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int OnlyDigits, uint32_t bkGroundColor, uint32_t fontColor, int space);
bool LCD_TxtID_color_BufferWindow(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int OnlyDigits, uint32_t bkGroundColor, uint32_t fontColor, int widthWindow);
bool LCD_TxtLineID(int text_ID, int Xpos, int Ypos, char *txt, char *LcdBuffer, int lineWidth, int nrLine);
int LCD_GetFontID_size(int fontID);
int LCD_GetFontID_style(int fontID);
int LCD_GetFontID_bkColor(int fontID);
int LCD_GetFontID_color(int fontID);
int ReturnWidthFontID(int fontID, char *txt);
int ReturnHeightFontID(int fontID);
int ReturnHeightHalfFontID(int fontID);
int LCD_GetFontID(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor);

#endif /* LCD_FONTS_H_ */
