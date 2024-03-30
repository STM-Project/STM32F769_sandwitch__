/*
 * lang.c
 *
 *  Created on: 01.02.2021
 *      Author: RafalMar
 */
#include "lang.h"
#include "sdram.h"

SRAM Language Lang;

void SetLang(int howMuch, int what)
{
	Lang.howMuch=howMuch;
	Lang.what=what;
}
