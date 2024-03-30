/*
 * SDCard.h
 *
 *  Created on: 22.12.2018
 *      Author: Elektronika RM
 */

#ifndef INC_SDCARD_H_
#define INC_SDCARD_H_


#include "stm32f7xx_hal.h"
#include "fatfs.h"

#define MAX_MYFILE   5

FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile[MAX_MYFILE];     /* File object */
char SDPath[4]; /* SD card logical drive path */
uint32_t byteswritten,bytesread;

int SDCardFileOpen(uint8_t id, char *name, uint8_t mode);
int SDCardStart(void);
int SDCardFileRead(uint8_t id, char *txt, uint32_t size, uint32_t *readSize);
int SDCardFileWrite(uint8_t id, char *txt, uint32_t size);
int SDCardFileClose(uint8_t id);
int SDCardFileInfo(char *name, uint32_t *fileSize);
int SDCardFilePosition(uint8_t id, uint32_t wsk);
uint32_t SDCard_ReadFile(char *fileName, char *pReadBuff, int maxSize);

#endif /* INC_SDCARD_H_ */
