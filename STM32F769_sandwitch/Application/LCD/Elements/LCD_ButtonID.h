/*
 * LCD_ButtonID.h
 *
 *  Created on: 03.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_ELEMENTS_LCD_BUTTONID_H_
#define LCD_ELEMENTS_LCD_BUTTONID_H_

#include "variables.h"

typedef struct
{
	int id;
	int x;
	int y;
	int width;
	int height;
	int bkColor;
	int frameColor;
	int fillColor;
	char txtTab[SIZE_TXT];
	int txtVar;
	int font;
	int fontPress;
	uint8_t buttonID;
}LCD_BUTTONStruct;

void ClearAllButton(void);
int SearchFirstFreeButton(void);
int GetButtonNumber(int touchID);
void LCD_Button(int buttonID, int fontID,int fontPress, int x, int y, int width, char *txt, int frameColor, int fillColor, int bkColor, uint16_t touch, int txtVar);
int TOUCH_ButtonID(int buttonID, int touchID);
int TOUCH_ButtonService(int ID_touchDetect, int pressRelease);

#endif /* LCD_ELEMENTS_LCD_BUTTONID_H_ */
