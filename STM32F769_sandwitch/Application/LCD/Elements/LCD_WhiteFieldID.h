/*
 * LCD_whiteFieldID.h
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_ELEMENTS_LCD_WHITEFIELDID_H_
#define LCD_ELEMENTS_LCD_WHITEFIELDID_H_

#include "variables.h"

typedef enum{
	WHITEFIELD_Type1,
	WHITEFIELD_Type2,
	WHITEFIELD_Type3,
	WHITEFIELD_Type4,
	WHITEFIELD_Type5
}WhiteFieldTypes;

typedef struct{
	uint16_t id;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	int bkColor;
	int frameColor;
	int fillColor;
	uint16_t txtVar;
	char txtTab[SIZE_TXT];
	int font;
	uint8_t space;
}LCD_WhiteFieldStruct;

extern LCD_WhiteFieldStruct Field[];  //sprawdzic czy musi byc koniecznie

void ClearAllWhiteField(void);
int SearchFirstFreeWhiteField(void);
int GetWhiteFieldNumber(int touchID);
void DeleteWhiteField(int touchID);
int LCD_WhiteField(int fieldID, int fontID, int x, int y, int width, char *txt, uint16_t touch, int space, int txtVar);
void LCD_DescrWhiteField(int fieldID,int x,int xField,int yField,int ySpace,int widthField, int descrFontID,char *descrTxt, int fieldFontID,char *fieldTxt, int touch, int space, int txtVar);
void TOUCH_WhiteFieldID(int whietFieldID, int touchID);
int vTOUCH_WhiteFieldService(int ID_touchDetect, int pressRelease);

#endif /* LCD_ELEMENTS_LCD_WHITEFIELDID_H_ */
