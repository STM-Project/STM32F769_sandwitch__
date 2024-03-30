/*
 * SDCard.c
 *
 *  Created on: 22.12.2018
 *      Author: Elektronika RM
 */


#include "stm32f7xx_hal.h"

#include "SDCard.h"
#include "dbg.h"

int SDCardStart(void)
{
	FRESULT result=f_mount(&SDFatFs, (TCHAR const*)SDPath, 0);
	if(FR_OK!=result)
		dbg("\r\nERROR SD_card -> f_mount ");
	return result;
}

int SDCardFileOpen(uint8_t id, char *name, uint8_t mode)
{
	FRESULT result=f_open(&MyFile[id], name, mode);
	if(FR_OK!=result)
		dbg("\r\nERROR SD_card -> f_open ");
	return result;


	//portTickType xLastExecutionTime;
	//xLastExecutionTime = xTaskGetTickCount();
	//vTaskDelayUntil(&xLastExecutionTime, 100);

}

int SDCardFileInfo(char *name, uint32_t *fileSize)
{
	FILINFO plikInfo;
	FRESULT result=f_stat(name, &plikInfo);   //f_size(&MyFile[id]); tylko gdy plik jest otwarty funkcj¹ f_open()
	if(FR_OK!=result)
		dbg("\r\nERROR SD_card -> f_stat ");
	else
		*fileSize=plikInfo.fsize;
	return result;
}

int SDCardFileWrite(uint8_t id, char *txt, uint32_t size)
{
	FRESULT result=f_write(&MyFile[id], txt, size, (void *)&byteswritten);
	//if(FR_OK!=result)
	//	dbg("\r\nERROR SD_card -> f_write ");
	return result;
}

int SDCardFileRead(uint8_t id, char *txt, uint32_t size, uint32_t *readSize)
{
	FRESULT result=f_read(&MyFile[id], txt, size, (void *)&bytesread);
	if(FR_OK!=result)
		dbg("\r\nERROR SD_card -> f_read ");
	else
		*readSize=bytesread;
	return result;
}

int SDCardFileClose(uint8_t id)
{
	FRESULT result=f_close(&MyFile[id]);
	if(FR_OK!=result)
		dbg("\r\nERROR SD_card -> f_open ");
	return result;
}

int SDCardFilePosition(uint8_t id, uint32_t wsk)  // Warunek: wsk >= 4 !!!
{
//	while((wsk%4)!=0)  //wyrównanie do 4bajtów dla odczytu SDRAM
//		wsk--;
	FRESULT result=f_lseek(&MyFile[id], wsk);
	if(FR_OK!=result)
		dbg("\r\nERROR SD_card -> f_lseek ");
	return result;
}

uint32_t SDCard_ReadFile(char *fileName, char *pReadBuff, int maxSize)
{
	uint32_t len;
	int result;

	result=SDCardFileOpen(0,fileName,FA_READ);
	if(FR_OK!=result)
		return -1;

	result=SDCardFileRead(0, pReadBuff, maxSize, &len);
	if(FR_OK!=result)
		return -1;

	result=SDCardFileClose(0);
	if(FR_OK!=result)
		return -1;

   return len;
}





//##################### -- Testy Wykonane i dzialajace -- ################################################################################################
/*
void SDTest()
{
	uint32_t  i1,i2,i3;


  SDCardFileOpen(0,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1.txt",FA_CREATE_ALWAYS | FA_WRITE);
  SDCardFileOpen(1,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2.txt",FA_CREATE_ALWAYS | FA_WRITE);
  SDCardFileOpen(2,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa3.txt",FA_CREATE_ALWAYS | FA_WRITE);
  SDCardFileWrite(0,"111",3);
  SDCardFileWrite(1,"222",3);
  SDCardFileWrite(2,"333",3);
  SDCardFileClose(0);
  SDCardFileClose(1);
  SDCardFileClose(2);



  i1=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1.txt");  SDCardFileOpen(0,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1.txt", FA_WRITE);
  i2=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2.txt");  SDCardFileOpen(1,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2.txt", FA_WRITE);
  i3=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa3.txt");  SDCardFileOpen(2,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa3.txt", FA_WRITE);
  SDCardFilePosition(0,i1);  SDCardFileWrite(0,"aaa",3);
  SDCardFilePosition(1,i2);  SDCardFileWrite(1,"bbb",3);
  SDCardFilePosition(2,i3);  SDCardFileWrite(2,"ccc",3);
  SDCardFileClose(0);
  SDCardFileClose(1);
  SDCardFileClose(2);


  i1=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1.txt");  SDCardFileOpen(0,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1.txt", FA_WRITE);
  i2=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2.txt");  SDCardFileOpen(1,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2.txt", FA_WRITE);
  i3=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa3.txt");  SDCardFileOpen(2,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa3.txt", FA_WRITE);
  SDCardFilePosition(0,i1);  SDCardFileWrite(0,"111",3);
  SDCardFilePosition(1,i2);  SDCardFileWrite(1,"222",3);
  SDCardFilePosition(2,i3);  SDCardFileWrite(2,"333",3);
  SDCardFileClose(0);
  SDCardFileClose(1);
  SDCardFileClose(2);


  i1=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1.txt");  SDCardFileOpen(0,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1.txt", FA_WRITE);
  i2=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2.txt");  SDCardFileOpen(1,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2.txt", FA_WRITE);
  i3=SDCardFileSize("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa3.txt");  SDCardFileOpen(2,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa3.txt", FA_WRITE);
  SDCardFilePosition(0,i1);  SDCardFileWrite(0,"aaa",3);
  SDCardFilePosition(1,i2);  SDCardFileWrite(1,"bbb",3);
  SDCardFilePosition(2,i3);  SDCardFileWrite(2,"ccc",3);
  SDCardFileClose(0);
  SDCardFileClose(1);
  SDCardFileClose(2);
}

*/
