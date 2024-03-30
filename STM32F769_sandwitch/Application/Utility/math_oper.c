/*
 * operations.c
 *
 *  Created on: 29.01.2021
 *      Author: RafalMar
 */

#include "math_oper.h"

int IsValueInRangeLowHigh(int val, int low, int high)
{
	if((val>low)&&(val<high))
		return 1;
	else
		return 0;
}
