/*
 * SCREEN_WifiSet.c
 *
 *  Created on: 05.02.2021
 *      Author: RafalMar
 */
#include "SCREEN_WifiSet.h"
#include "LCD_WhiteFieldID.h"
#include "LCD_ButtonID.h"
#include "LCD_CheckboxID.h"
#include "LCD_SelectID.h"
#include "LCD_ButtonID.h"
#include "fonts_images.h"
#include "LCD_BasicGaphics.h"
#include "LCD_Common.h"
#include "SCREEN_Common.h"
#include "float.h"
#include "variables.h"
#include "LCD_SliderID.h"
#include "dbg.h"
#include "cmsis_os.h"   //ZROBIC jakie zmienne kozystam Variable i tu w strukturze wpisuje a nie gdzies w dol kodu !!!!!!

enum DEFINED_FONT_NAME
{
	FontSelMainMenuTitle				=fontID_1,
	FontSelMainMenuTitlePress		=fontID_2,
	FontSelMainMenuSubtitle	    	=fontID_3,
	FontDescription 					=fontID_4,
	FontWhiteField						=fontID_5,
	FontSelOtherTitle					=fontID_6,
	FontSelOtherTitlePress			=fontID_7,
	FontSelOtherSubtitle	   		=fontID_8,
	FontButton			   			=fontID_9,
	FontButtonPress	   			=fontID_10,
	FontRED			   				=fontID_11,
	FontBLUE	   						=fontID_12,
	FontNormal		   				=fontID_13
};

enum DEFINED_TOUCH_NAME
{
	TOUCH_dhcp = TOUCH_OFFSET,
	TOUCH_act_STA,
	TOUCH_act_AP,
	TOUCH_button_111,
	TOUCH_button_222,
	TOUCH_button_333,
	TOUCH_macAddr,
	TOUCH_ipAddr,
	TOUCH_maskAddr,
	TOUCH_gateAddr,
	TOUCH_portNmb,
	TOUCH_checkbox_1,
	TOUCH_checkbox_2,
	TOUCH_checkbox_3,
	TOUCH_checkbox_4,
	TOUCH_checkbox_5,
	TOUCH_SliderV,
	TOUCH_SliderV_up,
	TOUCH_SliderV_down,
	TOUCH_SliderH,
	TOUCH_SliderH_left,
	TOUCH_SliderH_right
};

char WhiteFieldsTXT[]="\
MAC,MAC,\
IP,IP,\
Mask,Mask,\
Gate,Gate,\
Port,Port,\
";

static const char ButtonsTXT[]="\
naciœnij mni,press me now,\
Rafa³,press me now,\
@|?!¥¹ÆæÊê£³ÑñÓóŒœŸ¯¿,press me now,\
";

static const char CheckboxesTXT[]="\
67,Server,\
.,2,\
A,2,\
Rp,2,\
Œ,2,\
";

static const char SelectTXT_name1[]="\
NIE,NO,\
TAK,YES,\
TAK,YES,\
Markielowski,YES,\
TAK,YES,\
TAK,YES,\
TAK,YES,\
TAKX,YES,\
";
static const char SelectTXT_name2[]="\
NIE1,NO,\
NIE2,YES,\
NIE3,YES,\
NIE4,YES,\
NIE5,YES,\
NIE6,YES,\
NIE7,YES,\
NIE8,YES,\
NIE9,YES,\
NIE10,YES,\
NIE11,YES,\
NIE12,YES,\
NIE13,YES,\
NIE14,YES,\
NIE15,YES,\
NIE16,YES,\
NIE17,YES,\
NIE18,YES,\
NIE19,YES,\
NIE20,YES,\
NIE21,YES,\
NIE22,YES,\
NIE23,YES,\
NIE24,YES,\
NIE25,YES,\
12,YES,\
";
static const char SelectTXT_name3[]="\
NIE,NO,\
abc,YES,\
def,YES,\
";

extern int touchPressed;

extern int prescVal;

static int GetFontSizeFactor(int fontSize, int krotn, int bold)
{
	int getFont= fontSize;
	if(fontSize%3==0)  //fontSize =>  no bold
	{
		if(bold) getFont += 3*krotn+1;
		else     getFont += 3*krotn;
	}
	else //fontSize => bold
	{
		if(bold) getFont += 3*krotn;
		else     getFont += 3*krotn-1;
	}
	if((getFont<FONT_8)||(getFont>FONT_130_bold))
		return fontSize;
	else
		return getFont;
}

void SCREEN_WifiSet(int screenID, int fontSize, int resumeScreen)  //wifiset
{
	int FontSize,   FontStyle,   FontBkColor,	 FontColor;
	int widthLcd = LCD_GetXSize();
	int heightLcd = LCD_GetYSize();


	switch(screenID)
	{
	case 0:
		//FontSize=ValuefontSize;	 		FontStyle=Arial;		FontBkColor=DARKGRAY;	 FontColor=WHITE;

		break;

	default:

		break;
	}

	DeleteAllTouch();

	if(resumeScreen==0)
	{
		LCD_DeleteAllFontAndImages();

		LCD_LoadFont(fontSize,Arial,BROWN,WHITE, FontSelMainMenuTitle,1);
		LCD_LoadFont(fontSize,Arial,DARKGRAY,BLUE, FontSelMainMenuTitlePress,1);
		LCD_LoadFont(fontSize,Arial,BROWN,WHITE, FontSelMainMenuSubtitle,1);

		LCD_LoadFont(FONT_10, Arial, DARKGRAY, GREEN, FontDescription,1);
		LCD_LoadFont(GetFontSizeFactor(fontSize,2,0),Arial,WHITE,BLACK, FontWhiteField,1);

		LCD_LoadFont(fontSize,Arial,DARKGRAY,WHITE, FontSelOtherTitle,CONST_WIDTH);
		LCD_LoadFont(fontSize,Arial,DARKGRAY,BLUE, FontSelOtherTitlePress,CONST_WIDTH);
		LCD_LoadFont(fontSize,Arial,DARKGRAY,WHITE, FontSelOtherSubtitle,CONST_WIDTH);

		LCD_LoadFont(fontSize,Arial,DARKGRAY,WHITE, FontButton,1);
		LCD_LoadFont(fontSize,Arial,DARKGRAY,WHITE, FontButtonPress,1);

		LCD_LoadFont(fontSize,Arial,DARKGRAY,WHITE, FontNormal,1);
		LCD_LoadFont(fontSize, Arial, DARKGRAY, RED, FontRED,1);
		LCD_LoadFont(fontSize, Arial, DARKGRAY, BLUE, FontBLUE,1);

		LCD_LoadImage("LCD_images/slider_vlow.bmp");
		LCD_LoadImage("LCD_images/slider_low.bmp");
		LCD_LoadImage("LCD_images/slider_med.bmp");
		LCD_LoadImage("LCD_images/slider_high.bmp");

		LCD_LoadImage("LCD_images/term_vlow.bmp");
		LCD_LoadImage("LCD_images/term_low.bmp");
		LCD_LoadImage("LCD_images/term_med.bmp");
		LCD_LoadImage("LCD_images/term_high.bmp");

		LCD_LoadImage("LCD_images/wilg_vlow.bmp");
		LCD_LoadImage("LCD_images/wilg_low.bmp");
		LCD_LoadImage("LCD_images/wilg_med.bmp");
		LCD_LoadImage("LCD_images/wilg_high.bmp");


		SetVal(Const_Wifi_dhcp,0);
		SetVal(Const_Wifi_sta,0);
		SetVal(Const_Wifi_ap,0);

		SetVal(Const_Checkbox_test_1,No_Checked);
		SetVal(Const_Checkbox_test_2,No_Checked);
		SetVal(Const_Checkbox_test_3,Checked);
		SetVal(Const_Checkbox_test_4,Checked);
		SetVal(Const_Checkbox_test_5,Checked);
	}

	DisplayFontsStructState();
	LCD_Clear(0x000000);

	LCD_DrawThemeFrame(3,10, MYLIGHTGRAY,DARKGRAY);

	//int w[10];  w[0]=80; w[1]=180; w[2]=400; w[3]=53;
	//int w[10]={80,180,400,53};
	//LCD_DrawWidthFrame((int*){80,180,2,53},4,10, MYLIGHTGRAY,DARKGRAY);


	SetVal64(Const_Wifi_mac,NO_TAB,0xabcdef0123);
	SetVal(Const_Wifi_ip,IPDOT(192,168,1,46));
	SetVal(Const_Wifi_mask,IPStr2Int("2.255.69.113"));
	SetVal(Const_Wifi_gate,IPDOT(192,168,1,1));
	SetVal(Const_Wifi_port,80);



	ClearAllWhiteField();
	LCD_DescrWhiteField(WHITEFIELD_Type1,10,75,10,0,param_auto,FontDescription,GetSelTxt(0,WhiteFieldsTXT,0) ,FontWhiteField,Int64ToHexStr(GetVal64(Const_Wifi_mac,NO_TAB)), TOUCH_macAddr,1,TXT_ID1);
	LCD_DescrWhiteField(WHITEFIELD_Type1,10,param_auto,90,0,param_auto,FontDescription,GetSelTxt(0,WhiteFieldsTXT,1) ,FontWhiteField,IP2Str(GetVal(Const_Wifi_ip)), TOUCH_ipAddr,1,TXT_ID2);
	LCD_DescrWhiteField(WHITEFIELD_Type1,10,10,170,5,param_auto,FontDescription,GetSelTxt(0,WhiteFieldsTXT,2),FontWhiteField,IP2Str(GetVal(Const_Wifi_mask)),TOUCH_maskAddr,1,TXT_ID3);
	LCD_DescrWhiteField(WHITEFIELD_Type1,20,10,250,10,param_auto,FontDescription,GetSelTxt(0,WhiteFieldsTXT,3),FontWhiteField,IP2Str(GetVal(Const_Wifi_gate)),  TOUCH_gateAddr,1,TXT_ID4);
	LCD_DescrWhiteField(WHITEFIELD_Type1,10,75,330,0,param_auto,FontDescription,GetSelTxt(0,WhiteFieldsTXT,4),FontWhiteField,Int2DecStr(GetVal(Const_Wifi_port)), TOUCH_portNmb,1,TXT_ID5);


	ClearAllSelect();
	LCD_Select(SELECT_TYPE_Other,10,430,FontSelOtherTitle,FontSelOtherTitlePress,FontSelOtherSubtitle,SelectTXT_name1,Const_Wifi_dhcp,TOUCH_dhcp,TXT_ID6);
	LCD_Select(SELECT_TYPE_Other,560,300,FontSelOtherTitle,FontSelOtherTitlePress,FontSelOtherSubtitle,SelectTXT_name2,Const_Wifi_sta,TOUCH_act_STA,TXT_ID7);
	LCD_Select(SELECT_TYPE_Other,500,430,FontSelOtherTitle,FontSelOtherTitlePress,FontSelOtherSubtitle,SelectTXT_name3,Const_Wifi_ap,TOUCH_act_AP,TXT_ID8);
	LCD_Select(SELECT_TYPE_MainMenu,widthLcd,0,FontSelMainMenuTitle,FontSelMainMenuTitlePress,FontSelMainMenuSubtitle,SelectTXT_MainMenu,0,TOUCH_mainMenu,TXT_ID9);

	ClearAllButton();
	LCD_Button(0,FontButton,FontButtonPress,400,200,param_auto,GetSelTxt(0,ButtonsTXT,0),MYLIGHTGRAY,RED,DARKGRAY,TOUCH_button_111,TXT_ID10);
	LCD_Button(1,FontButton,FontButtonPress,400,300,param_auto,GetSelTxt(0,ButtonsTXT,1),MYLIGHTGRAY,GREEN,DARKGRAY,TOUCH_button_222,TXT_ID11);
	LCD_Button(2,FontButton,FontButtonPress,400,400,param_auto,GetSelTxt(0,ButtonsTXT,2),MYLIGHTGRAY,BLUE,DARKGRAY,TOUCH_button_333,TXT_ID12);


	ClearAllCheckbox();
	LCD_DescrCheckboxUp(Rect_b,Const_Checkbox_test_1,300,100,2,25,FontDescription,GetSelTxt(0,CheckboxesTXT,0), WHITE,DARKGRAY,DARKGRAY,WHITE,TOUCH_checkbox_1);
	LCD_DescrCheckboxUp(Round_Rect_3r,Const_Checkbox_test_2,400,100,10,25,FontDescription,GetSelTxt(0,CheckboxesTXT,1), WHITE,DARKGRAY,DARKGRAY,ORANGE,TOUCH_checkbox_2);
	LCD_DescrCheckboxUp(Round_Rect_3r,Const_Checkbox_test_3,450,100,10,28,FontDescription,GetSelTxt(0,CheckboxesTXT,2), WHITE,DARKGRAY,DARKGRAY,RED,TOUCH_checkbox_3);
	LCD_DescrCheckboxUp(Rect_b,Const_Checkbox_test_4,600,100,10,36,FontDescription,GetSelTxt(0,CheckboxesTXT,3), WHITE,DARKGRAY,DARKGRAY,GREEN,TOUCH_checkbox_4);
	LCD_DescrCheckboxUp(Round_Rect_3r,Const_Checkbox_test_5,700,100,10,60,FontDescription,GetSelTxt(0,CheckboxesTXT,4), WHITE,DARKGRAY,DARKGRAY,MAGENTA,TOUCH_checkbox_5);


	LCD_LoadImage("pex.bmp");

	ClearAllSlider();
	SetVal(Var_slider0,75);
	LCD_DrawSliderWithButtons(SLIDER_ID1,VERTICAL, 210,170, 200,22, Var_slider0, TOUCH_SliderV, TOUCH_SliderV_up, TOUCH_SliderV_down);
	LCD_TxtIDSlider(SLIDER_ID1,FontNormal, 170,260,TXT_ID13);
	//TxtID_Var(TXT_ID13,FontNormal, 170,260, int2perctStr(GetVal(Var_slider0),' '),halfHight,0);

	SetVal(Var_slider1,25);
	LCD_DrawSliderWithButtons(SLIDER_ID2,HORIZONTAL, 270,30, 150,22, Var_slider1, TOUCH_SliderH, TOUCH_SliderH_left, TOUCH_SliderH_right);
	//LCD_TxtIDSlider(SLIDER_ID2,FontNormal, 350,70,TXT_ID14);
	TxtID_Var(TXT_ID14,FontNormal, 325,70, int2perctStr(GetVal(Var_slider1),' '),halfHight,0);



	LCD_DrawImage("pex.bmp",760,100);



	LCD_DisplayBuff(0,(uint32_t)0,(uint32_t)0,(uint32_t)widthLcd,(uint32_t)heightLcd);

  //przetestuj wyswietlanie obrazków .bmp i animaccji

	//LCD_LoadAnimation("Animation/butterfly1",22,1);
	LCD_LoadAnimation("Animation/butterfly2",22,1);

	LCD_DrawImageIndirect("pex.bmp",760,150);

	BSP_LCD_DrawCircle(100,100,50);


}


void TOUCH_WifiSet(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y)
{
	void _Slider(void)
	{
		TOUCH_SliderService(ID_touchDetect,pressRelease,x,y);
		if(PRESSED==pressRelease){
		if(IsTouchSlider(SLIDER_ID2,ID_touchDetect))
		{
			if((GetVal(Var_slider1)>40)&&(GetVal(Var_slider1)<60))  //tu jezeli dotyk odpowiedni to realizuj to a nie tak!!!!!!!
				LCD_Txt(TXT_ID14, int2perctStr(GetVal(Var_slider1),' '), fontNoChange, halfHight,0);
			else if(GetVal(Var_slider1)>=60)
				LCD_Txt(TXT_ID14, int2perctStr(GetVal(Var_slider1),' '), FontRED, halfHight,0);
			else
				LCD_Txt(TXT_ID14, int2perctStr(GetVal(Var_slider1),' '), FontBLUE, halfHight,0);
		}}
	}

	void _Select(void){
		vTOUCH_SelectService(ID_touchDetect,pressRelease);
	}

	void _WhiteField(void)
	{
		switch(vTOUCH_WhiteFieldService(ID_touchDetect,pressRelease))
		{
		case TOUCH_macAddr:
		case TOUCH_ipAddr:
		case TOUCH_maskAddr:
		case TOUCH_gateAddr:
			break;
		case TOUCH_portNmb:
			SCREEN_Load(SCREEN_Keyboard,FONT_28,0);
			break;
		}
	}

	void _Checkbox(void){
		TOUCH_CheckboxService(ID_touchDetect,pressRelease);
	}

	void _Button(void)
	{
		switch(TOUCH_ButtonService(ID_touchDetect,pressRelease))
		{
		case TOUCH_button_111:
		case TOUCH_button_222:
		case TOUCH_button_333:
			osDelay(300);
			SCREEN_Load(Screen.current,Screen.param,1);
			break;
		}
	}

	_Slider();
	_Select();
	_WhiteField();
	_Checkbox();
	_Button();

	switch (pressRelease)
	{
	case PRESSED:
		break;
	case RELEASED:
		touchPressed=0;
		break;
  }
}
