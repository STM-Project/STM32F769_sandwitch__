/*
 * SCREEN_WifiSet.h
 *
 *  Created on: 05.02.2021
 *      Author: RafalMar
 */

#ifndef LCD_SCREENS_SCREEN_WIFISET_H_
#define LCD_SCREENS_SCREEN_WIFISET_H_

#include "fonts_images.h"

extern char WhiteFieldsTXT[];

void SCREEN_WifiSet(int screenID, int fontSize, int resumeScreen);
void TOUCH_WifiSet(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y);

#endif /* LCD_SCREENS_SCREEN_WIFISET_H_ */
