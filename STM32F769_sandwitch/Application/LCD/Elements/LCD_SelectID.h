/*
 * select.h
 *
 *  Created on: 01.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_ELEMENTS_LCD_SELECTID_H_
#define LCD_ELEMENTS_LCD_SELECTID_H_

#define SELECT_TOUCH	50000
#define MAX_SELECT_TOUCH	50200
#define MAX_LCD_SELECT	 40

typedef enum
{
  SELECT_TYPE_MainMenu,
  SELECT_TYPE_Other,
  SELECT_ACTION_Unroll,
  SELECT_ACTION_PressMain,
  SELECT_ACTION_PressOther
}SELECT_TYPES_AND_ACTIONS;

typedef struct //co nie konieczne to z int na uint16_t i jest osczednosc pamieci !!!!
{
	int id;  //daj uint16 zamiast int
	int xTriangl;
	int yTriangl;
	int xScroll;
	int yScroll;
	int yScrollUp;
	int width;
	int bkColor;
	int frameColorMain;
	int fillColorMain;
	int frameColorUnroll;
	int fillColorUnroll;
	int frameColorPress;
	int fillColorPress;
	const char *pTxt;
	int nmbTitleVar;
	int fontTitle;
	int fontTitlePress;
	int fontScroll;
	int selType;
	int valName;
	int xOffset;
}LCD_SelectMenu;

typedef struct
{
	int nrLcdSelect;
	int wsk;
	LCD_SelectMenu Param[MAX_LCD_SELECT];
}LCD_SelectStruct;

extern LCD_SelectStruct Sel;//WYMAZ!!!!!

void ClearAllSelect(void);
char* GetSelTxt(int nrBuff, const char* txt, int line);
void LCD_Select(int selectID, int x,int y, int fontTitle,int fontTitlePress,int fontScroll, const char *txt,int line, int touch, int txtVar);

void LCD_SelectUnroll(int touchID);
int LCD_SelectRoll(int touchID);
int LCD_SelectPress(int ID_touchDetect);
int GetSelectLineNr(int ID_touchDetect);
int IsSelectRoll(void);
void vTOUCH_SelectService(int ID_touchDetect, int pressRelease);

#endif /* LCD_ELEMENTS_LCD_SELECTID_H_ */
