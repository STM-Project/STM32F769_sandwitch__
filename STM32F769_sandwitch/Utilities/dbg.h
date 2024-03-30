/*
 * dbg.h
 *
 *  Created on: 17.07.2020
 *      Author: Elektronika RM
 */

#ifndef DBG_H_
#define DBG_H_

#include <stdbool.h>

void dbg(char *txt);
void dbg_DMA(char *txt);
void dbg_roz(char *txt);
void Init_DBG(void);
bool isDBG_Recv(char *txt);
bool isAnything_Recv(void);
void DbgSprintf(unsigned int buffer_len, const char *fmt, ...);
void DBG_RxCallbackService(void);

void Dbg(int on, char *txt);
void DbgStr(int on, char *startTxt, char *txt, char *endTxt);
void DbgVar(int on, unsigned int buffer_len, const char *fmt, ...);

#endif /* DBG_H_ */
