/*
 * LCD_CheckboxID.h
 *
 *  Created on: 05.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_ELEMENTS_LCD_CHECKBOXID_H_
#define LCD_ELEMENTS_LCD_CHECKBOXID_H_

#include "stm32f7xx_hal.h"

enum actionsCheckbox
{
  No_Checked,
  Checked
};

enum themeCheckbox
{
  Rect,
  Rect_b,
  Rect_r,
  Bold_Rect,
  Bold_Rect_b,
  Bold_Rect_r,
  Round_Rect_1,
  Round_Rect_1b,
  Round_Rect_1r,
  Bold_Round_Rect_1,
  Bold_Round_Rect_1b,
  Bold_Round_Rect_1r,
  Round_Rect_2,
  Round_Rect_2b,
  Round_Rect_2r,
  Bold_Round_Rect_2,
  Bold_Round_Rect_2b,
  Bold_Round_Rect_2r,
  Round_Rect_3,
  Round_Rect_3b,
  Round_Rect_3r,
  Bold_Round_Rect_3,
  Bold_Round_Rect_3b,
  Bold_Round_Rect_3r
};

typedef struct
{
	uint8_t id;
	uint8_t type;
	uint16_t xBox;
	uint16_t yBox;
	uint16_t xCheck;
	uint16_t yCheck;
	uint8_t size;
	uint32_t bkColor;
	uint32_t frameColor;
	uint32_t fillColor;
	uint32_t checkColor;
	uint8_t valName;
}LCD_CHECKBOXStruct;

void ClearAllCheckbox(void);
int SearchFirstFreeCheckbox(void);
int GetCheckboxNumber(int touchID);
void LCD_Checkbox(int checkboxID, int valName, int x, int y, int size, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch);
void LCD_DescrCheckboxLeft(int checkboxID, int valName, int x,int xBox, int yBox, int size,int descrFontID,char *descrTxt, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch);
void LCD_DescrCheckboxRight(int checkboxID, int valName, int xBox,int x, int yBox, int size,int descrFontID,char *descrTxt, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch);
void LCD_DescrCheckboxUp(int checkboxID, int valName, int xBox,int yBox,int ySpace, int size,int descrFontID,char *descrTxt, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch);
void LCD_DescrCheckboxDown(int checkboxID, int valName, int xBox,int yBox,int ySpace, int size,int descrFontID,char *descrTxt, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor, uint32_t checkColor, int touch);
int TOUCH_CheckboxID(int CheckboxID, int touchID, int k);
void TOUCH_CheckboxService(int ID_touchDetect, int pressRelease);

#endif /* LCD_ELEMENTS_LCD_CHECKBOXID_H_ */
