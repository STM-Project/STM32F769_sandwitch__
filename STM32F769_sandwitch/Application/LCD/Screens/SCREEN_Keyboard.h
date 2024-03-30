/*
 * SCREEN_Keyboard.h
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_SCREENS_SCREEN_KEYBOARD_H_
#define LCD_SCREENS_SCREEN_KEYBOARD_H_

#include <LCD_Common.h>
#include "stm32f7xx_hal.h"
#include "variables.h"

#define MAX_TXT_IN_KEY	 5

typedef struct
{
	uint8_t pressCapitalLetter;
	uint8_t pressAnotherSigns;
	char txt[SIZE_TXT+5];
	uint16_t lenTxt;
	uint8_t nextLine;
	uint16_t pixelBetweenLines;
	uint16_t cursorPixelPosX;
	uint16_t cursorPixelPosY;
	uint16_t cursorPixelWidth;
	uint16_t cursorTxtPos;
}LCD_keyboard;  //biale pole so wypelnienia

typedef struct
{
	uint16_t id;
	uint16_t xBox;
	uint16_t yBox;
	uint16_t wBox;
	uint16_t hBox;
	uint32_t bkColor;
	uint32_t frameColor;
	uint32_t fillColor;
	frameShape pShape;
	uint16_t fontID;
	uint16_t pressFontID;
	uint32_t pressFrameColor;
	uint32_t pressFillColor;
	uint32_t alternatFrameColor;
	uint32_t alternatFillColor;
	uint16_t xFont;
	uint16_t yFont;
	char txtBuff[MAX_TXT_IN_KEY];
}LCD_KEYBOARDStruct;   // sma klawiatura

extern LCD_keyboard KeyBoard;

void KEYBOARD_CursorToggle(void);
void ClearKeyBoardStruct(void);
void ClearAllKey(void);
int SearchFirstFreeKey(void);
int GetKeyNumber(int touchID);

void LCD_CursorIndirect(int x, int y, int width, int color);
void LCD_DrawUpArrow(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor);
void LCD_SignX(int x, int y, int width, int color);
void LCD_SignBackspace(int x, int y, int width, int height, int color);
void LCD_SignX_Buff(int posBuff,int areaWidth, int x, int y, int width, int color);
void LCD_SignBackspace_Buff(int posBuff,int areaWidth, int x, int y, int width, int height, int color);
void KeyBlock(uint16_t x,uint16_t y,uint16_t width,uint16_t height, int fontID,int pressFontID, uint32_t bkColor, frameShape pframeShape,char *txt,int touch);
void LCD_Keyboard(int keyBoardID, frameShape pframeShape);
void KEYBOARD_DisplayTxt(void);
int KEYBOARD_GetEndPosX(int k);  //ustawic jako static !!!!!
int KEYBOARD_GetStartPosX(int k);  //ustawic jako static !!!!!
int KEYBOARD_GetStartPosY(int k);  //ustawic jako static !!!!!
int KEYBOARD_GetPixelTxtPosX(int k, int *bufLineStart,int *TxtLenInLine);  //ustawic jako static !!!!!
void SCREEN_KeyboardSet(int screenID, int fontSize,int resumeScreen);
void TOUCH_KeyID(int KeyID, int touchID);
int KEYBOARD_GetTxtLines(int k);
int KEYBOARD_SetChar(int touchID);
void TOUCH_KeyboardSet(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y);

#endif /* LCD_SCREENS_SCREEN_KEYBOARD_H_ */
