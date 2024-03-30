/*
 * common.h
 *
 *  Created on: 15.01.2021
 *      Author: RafalMar
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "sdram.h"

#define ABS(X)  ((X) > 0 ? (X) : -(X))

typedef struct
{
	char *pData;
	uint32_t len;
}DATA_TO_SEND;

typedef struct
{
	char a1[100];
}aaa;
SDRAM static aaa bbb[40];

#endif /* COMMON_H_ */
