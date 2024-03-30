/*
 * lang.h
 *
 *  Created on: 01.02.2021
 *      Author: RafalMar
 */

#ifndef LANG_H_
#define LANG_H_

typedef enum{
	Polish,
	English
}lang;

typedef struct
{
  int howMuch;
  int what;
}Language;

extern Language Lang;

void SetLang(int howMuch, int what);

#endif /* LANG_H_ */
