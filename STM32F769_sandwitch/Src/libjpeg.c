/**
 ******************************************************************************
 * File Name          : libjpeg.c
 * Description        : This file provides code for the configuration
 *                      of the libjpeg instances.
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2020 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <LCD_Hardware.h>
#include "libjpeg.h"

/* USER CODE BEGIN 0 */
#include "jpeg_utils.h"
#include "dbg.h"
#include "sdram.h"
#include "SDCard.h"
#include "Settings.h"
#include "StartUp.h"
/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/* Global variables ---------------------------------------------------------*/

/* USER CODE BEGIN 2 */
//
///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
//#define _TERMINAL	1

#define PATTERN_SEARCH_BUFFERSIZE 512

#define CHUNK_SIZE_IN  ((uint32_t)(4096))
#define CHUNK_SIZE_OUT ((uint32_t)(768 * 4))

#define JPEG_BUFFER_EMPTY 0
#define JPEG_BUFFER_FULL  1

#define NB_OUTPUT_DATA_BUFFERS      2
#define NB_INPUT_DATA_BUFFERS       2

#define JPEG_SOI_MARKER (0xFFD8) /* JPEG Start Of Image marker*/
#define JPEG_SOI_MARKER_BYTE0 (JPEG_SOI_MARKER & 0xFF)
#define JPEG_SOI_MARKER_BYTE1 ((JPEG_SOI_MARKER >> 8) & 0xFF)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

typedef struct
{
	uint8_t State;
	uint8_t *DataBuffer;
	uint32_t DataBufferSize;

} JPEG_Data_BufferTypeDef;

static JPEG_YCbCrToRGB_Convert_Function pConvert_Function;

SRAM static uint8_t MCU_Data_OutBuffer0[CHUNK_SIZE_OUT];
SRAM static uint8_t MCU_Data_OutBuffer1[CHUNK_SIZE_OUT];

SRAM static uint8_t JPEG_Data_InBuffer0[CHUNK_SIZE_IN];
SRAM static uint8_t JPEG_Data_InBuffer1[CHUNK_SIZE_IN];

static JPEG_Data_BufferTypeDef Jpeg_OUT_BufferTab[NB_OUTPUT_DATA_BUFFERS]=
{
{ JPEG_BUFFER_EMPTY, MCU_Data_OutBuffer0, 0 },
{ JPEG_BUFFER_EMPTY, MCU_Data_OutBuffer1, 0 } };

static JPEG_Data_BufferTypeDef Jpeg_IN_BufferTab[NB_INPUT_DATA_BUFFERS]=
{
{ JPEG_BUFFER_EMPTY, JPEG_Data_InBuffer0, 0 },
{ JPEG_BUFFER_EMPTY, JPEG_Data_InBuffer1, 0 } };

static uint32_t MCU_TotalNb=0;
static uint32_t MCU_BlockIndex=0;
static uint32_t Jpeg_HWDecodingEnd=0;

static uint32_t JPEG_OUT_Read_BufferIndex=0;
static uint32_t JPEG_OUT_Write_BufferIndex=0;
static uint32_t Output_Is_Paused=0;

static uint32_t JPEG_IN_Read_BufferIndex=0;
static uint32_t JPEG_IN_Write_BufferIndex=0;
static uint32_t Input_Is_Paused=0;

static uint32_t FrameBufferAddress;
static uint32_t JpegProcessing_End=0;

static uint32_t FrameSize;
SRAM static uint8_t PatternSearchBuffer[PATTERN_SEARCH_BUFFERSIZE];
static uint32_t Previous_FrameOffset;
static uint32_t FrameRate;
static uint32_t FrameIndex;

extern JPEG_HandleTypeDef hjpeg;
extern JPEG_HandleTypeDef hjpeg;
extern DMA2D_HandleTypeDef hdma2d;
static JPEG_ConfTypeDef JPEG_Info;
static SRAM FIL JPEG_File, *pFile;

SDRAM static char jpegOutputData[800*480*3];

/* USER CODE END 2 */

/* LIBJPEG init function */
void MX_LIBJPEG_Init(void)
{
	/**
	 */

	/* USER CODE BEGIN 3 */
	/* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void OnError_Handler(void)
{
	dbg("\r\nERROR !!! ");
	while (1)
	{
		;
	} /* Blocking on error */
}

/**
 * @brief  Find Next JPEG frame offset in the video file.
 * @param  offset: Current offset in the video file.
 * @param  file: Pointer to the video file object.
 * @retval None
 */
static uint32_t JPEG_FindFrameOffset(uint32_t offset, FIL *file)
{
	uint32_t index=offset, readSize=0, i;
	//memset(PatternSearchBuffer,0,PATTERN_SEARCH_BUFFERSIZE);
	do
	{
		if (f_size(file)<=(index+1))
		{
			/* end of file reached*/
			dbg("\r\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
			return 0;
		}

		defregtgdsfvds:
		f_lseek(file, index);  //ZBADAC PROBLEM Z f_lseek !!!!
		//SDCardFilePosition(1, index);
		f_read(file, PatternSearchBuffer, PATTERN_SEARCH_BUFFERSIZE, (UINT*) (&readSize));

		if((readSize==0)&&(index<f_size(file)))
		{
			dbg("\r\nSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS");
			index+=FrameSize;
			if(index>=f_size(file))
				return 0;
			goto defregtgdsfvds;
		}

		if (readSize!=0)
		{
			for (i=0; i<(readSize-1); i++)
			{
				if ((PatternSearchBuffer[i]==0)&&(PatternSearchBuffer[i+1]==JPEG_SOI_MARKER_BYTE1))
				{
					if ((PatternSearchBuffer[i+2]==JPEG_SOI_MARKER_BYTE0)&&(PatternSearchBuffer[i+3]==0xFF))
					{
						if (i>3)
						{
							FrameSize=PatternSearchBuffer[i-1]*65536+PatternSearchBuffer[i-2]*256+PatternSearchBuffer[i-3];
						}
						return (index+i+1);
					}
				}
			}

			index+=(readSize-1);
		}
	} while (readSize!=0);

	dbg("\r\nWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW");
	return 0;
}

/**
 * @brief  Decode_DMA
 * @param hjpeg: JPEG handle pointer
 * @param  FileName    : jpg file path for decode.
 * @param  DestAddress : ARGB destination Frame Buffer Address.
 * @retval None
 */
static uint32_t JPEG_Decode_DMA(JPEG_HandleTypeDef *hjpeg, FIL *file, uint32_t DestAddress)
{
	uint32_t i;
	pFile=file;
	FrameBufferAddress=DestAddress;

	/* Read from JPG file and fill input buffers */
	for (i=0; i<NB_INPUT_DATA_BUFFERS; i++)  //to ze nie mozna odczytac za jednym razem f_red odpowiedzialny jest f_lseek !!!!!
	{
		if (f_read(pFile, Jpeg_IN_BufferTab[i].DataBuffer+512*0, 512, (UINT*) (&Jpeg_IN_BufferTab[i].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[i].DataBuffer+512*1, 512, (UINT*) (&Jpeg_IN_BufferTab[i].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[i].DataBuffer+512*2, 512, (UINT*) (&Jpeg_IN_BufferTab[i].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[i].DataBuffer+512*3, 512, (UINT*) (&Jpeg_IN_BufferTab[i].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[i].DataBuffer+512*4, 512, (UINT*) (&Jpeg_IN_BufferTab[i].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[i].DataBuffer+512*5, 512, (UINT*) (&Jpeg_IN_BufferTab[i].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[i].DataBuffer+512*6, 512, (UINT*) (&Jpeg_IN_BufferTab[i].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[i].DataBuffer+512*7, 512, (UINT*) (&Jpeg_IN_BufferTab[i].DataBufferSize))!=FR_OK)
			OnError_Handler();

		Jpeg_IN_BufferTab[i].DataBufferSize=CHUNK_SIZE_IN;
		Jpeg_IN_BufferTab[i].State= JPEG_BUFFER_FULL;
	}
	/* Start JPEG decoding with DMA method */
	HAL_JPEG_Decode_DMA(hjpeg, Jpeg_IN_BufferTab[0].DataBuffer, Jpeg_IN_BufferTab[0].DataBufferSize, Jpeg_OUT_BufferTab[0].DataBuffer, CHUNK_SIZE_OUT);
	return 0;
}

/**
 * @brief  JPEG Ouput Data BackGround Postprocessing .
 * @param hjpeg: JPEG handle pointer
 * @retval 1 : if JPEG processing has finiched, 0 : if JPEG processing still ongoing
 */
static uint32_t JPEG_OutputHandler(JPEG_HandleTypeDef *hjpeg)
{
	uint32_t ConvertedDataCount;

	if (Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].State==JPEG_BUFFER_FULL)
	{
		MCU_BlockIndex+=pConvert_Function(Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].DataBuffer, (uint8_t *) FrameBufferAddress, MCU_BlockIndex, Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].DataBufferSize, &ConvertedDataCount);

		Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].State= JPEG_BUFFER_EMPTY;
		Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].DataBufferSize=0;

		JPEG_OUT_Read_BufferIndex++;
		if (JPEG_OUT_Read_BufferIndex>=NB_OUTPUT_DATA_BUFFERS)
		{
			JPEG_OUT_Read_BufferIndex=0;
		}

		if (MCU_BlockIndex==MCU_TotalNb)
		{
			return 1;
		}
	}
	else if ((Output_Is_Paused==1)&&(JPEG_OUT_Write_BufferIndex==JPEG_OUT_Read_BufferIndex)&&(Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].State==JPEG_BUFFER_EMPTY))
	{
		Output_Is_Paused=0;
		HAL_JPEG_Resume(hjpeg, JPEG_PAUSE_RESUME_OUTPUT);
	}

	return 0;
}

/**
 * @brief  JPEG Input Data BackGround processing .
 * @param hjpeg: JPEG handle pointer
 * @retval None
 */
static void JPEG_InputHandler(JPEG_HandleTypeDef *hjpeg)
{
	if (Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].State==JPEG_BUFFER_EMPTY)
	{
		if (f_read(pFile, Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer+512*0, 512, (UINT*) (&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer+512*1, 512, (UINT*) (&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer+512*2, 512, (UINT*) (&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer+512*3, 512, (UINT*) (&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer+512*4, 512, (UINT*) (&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer+512*5, 512, (UINT*) (&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer+512*6, 512, (UINT*) (&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize))!=FR_OK)
			OnError_Handler();
		if (f_read(pFile, Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer+512*7, 512, (UINT*) (&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize))!=FR_OK)
			OnError_Handler();

		Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBufferSize=CHUNK_SIZE_IN;
		Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].State= JPEG_BUFFER_FULL;

		if ((Input_Is_Paused==1)&&(JPEG_IN_Write_BufferIndex==JPEG_IN_Read_BufferIndex))
		{
			Input_Is_Paused=0;
			HAL_JPEG_ConfigInputBuffer(hjpeg, Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBuffer, Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize);

			HAL_JPEG_Resume(hjpeg, JPEG_PAUSE_RESUME_INPUT);
		}

		JPEG_IN_Write_BufferIndex++;
		if (JPEG_IN_Write_BufferIndex>=NB_INPUT_DATA_BUFFERS)
		{
			JPEG_IN_Write_BufferIndex=0;
		}
	}
}

void JPEG_test(void)
{
	static uint32_t FrameOffset=0, FrameOffset_p=0;
	uint32_t xPos=0, yPos=0;

	Jpeg_OUT_BufferTab[0].State= JPEG_BUFFER_EMPTY;
	Jpeg_OUT_BufferTab[0].DataBuffer=MCU_Data_OutBuffer0;
	Jpeg_OUT_BufferTab[0].DataBufferSize=0;
	Jpeg_OUT_BufferTab[1].State= JPEG_BUFFER_EMPTY;
	Jpeg_OUT_BufferTab[1].DataBuffer=MCU_Data_OutBuffer1;
	Jpeg_OUT_BufferTab[1].DataBufferSize=0;

	Jpeg_IN_BufferTab[0].State= JPEG_BUFFER_EMPTY;
	Jpeg_IN_BufferTab[0].DataBuffer=JPEG_Data_InBuffer0;
	Jpeg_IN_BufferTab[0].DataBufferSize=0;
	Jpeg_IN_BufferTab[1].State= JPEG_BUFFER_EMPTY;
	Jpeg_IN_BufferTab[1].DataBuffer=JPEG_Data_InBuffer1;
	Jpeg_IN_BufferTab[1].DataBufferSize=0;

	MCU_TotalNb=0;
	MCU_BlockIndex=0;
	Jpeg_HWDecodingEnd=0;

	JPEG_OUT_Read_BufferIndex=0;
	JPEG_OUT_Write_BufferIndex=0;
	Output_Is_Paused=0;

	JPEG_IN_Read_BufferIndex=0;
	JPEG_IN_Write_BufferIndex=0;
	Input_Is_Paused=0;

	if (f_open(&JPEG_File, "image.jpg", FA_READ)==FR_OK)
	{
		/*##-6- JPEG decoding with DMA (Not Blocking ) Method ################*/
		JPEG_Decode_DMA(&hjpeg, &JPEG_File, (uint32_t) jpegOutputData);

		/*##-7- Wait till end of JPEG decoding and perfom Input/Output Processing in BackGround  #*/
		do
		{
			JPEG_InputHandler(&hjpeg);
			JpegProcessing_End=JPEG_OutputHandler(&hjpeg);
			dbg(".");

		} while (JpegProcessing_End==0);

		/*##-8- Get JPEG Info  ###############################################*/
		HAL_JPEG_GetInfo(&hjpeg, &JPEG_Info);

		/*##-9- Copy RGB decoded Data to the display FrameBuffer  ############*/
		xPos=(800-JPEG_Info.ImageWidth)/2;
		yPos=(480-JPEG_Info.ImageHeight)/2;

		DMA2D_CopyBuffer((uint32_t *) jpegOutputData, xPos, yPos, JPEG_Info.ImageWidth, JPEG_Info.ImageHeight, 0);

		/*##-10- Close the JPG file ##########################################*/
		f_close(&JPEG_File);
	}
}
#include "LCD_Common.h"

extern int WSTRZYMUJ;

void TOUCH_Video(int ID_touchDetect, int pressRelease, uint16_t x, uint16_t y)
{
	switch (pressRelease)
	{
	case PRESSED:

		switch (ID_touchDetect)
		{
		case 999:
			WSTRZYMUJ=2;
			break;
		}
		break;

	case RELEASED:
		break;
	}
}

void AVI_test(void *argument)
{
	static uint32_t FrameOffset=0, FrameOffset_p=0;
	uint32_t xPos=0, yPos=0, currentFrameRate, startTime, endTime, timeStop;
	uint32_t isfirstFrame;

	WSTRZYMUJ=1;

	isfirstFrame=1;
	Previous_FrameOffset=0;

	WaitForAccess(ACCESS_TO_SDCARD);
	if (f_open(&JPEG_File, "Video/BehindTheDemo.avi", FA_READ)==FR_OK)  //Video/video2_800_480.avi
	{
		ReleaseAccess(ACCESS_TO_SDCARD);
		dbg("\r\nSTART----------------------------------------------------");
		FrameIndex=0;
		FrameRate=0;
		do
		{
			REPEAT_JPEG_FindFrameOffset:
			startTime=xTaskGetTickCount();

			WaitForAccess(ACCESS_TO_SDCARD);
			FrameOffset=JPEG_FindFrameOffset(Previous_FrameOffset, &JPEG_File); //nie raz przy odswiezaniu HTTP wyrzuca ty chyba return 0 !!!!
			ReleaseAccess(ACCESS_TO_SDCARD);

			if (FrameOffset==FrameOffset_p)
			{
				Previous_FrameOffset+=FrameSize;
				//dbg("\r\nREPEAT_JPEG_FindFrameOffset");
				goto REPEAT_JPEG_FindFrameOffset;
			}
			FrameOffset_p=FrameOffset;

			if (FrameOffset!=0)
			{
				WaitForAccess(ACCESS_TO_SDCARD);
				if (f_lseek(&JPEG_File, FrameOffset)==FR_OK)
				{
					ReleaseAccess(ACCESS_TO_SDCARD);

					MCU_TotalNb=0;
					MCU_BlockIndex=0;
					Jpeg_HWDecodingEnd=0;

					JPEG_OUT_Read_BufferIndex=0;
					JPEG_OUT_Write_BufferIndex=0;
					Output_Is_Paused=0;

					JPEG_IN_Read_BufferIndex=0;
					JPEG_IN_Write_BufferIndex=0;
					Input_Is_Paused=0;

					Jpeg_OUT_BufferTab[0].State= JPEG_BUFFER_EMPTY;
					Jpeg_OUT_BufferTab[0].DataBuffer=MCU_Data_OutBuffer0;
					Jpeg_OUT_BufferTab[0].DataBufferSize=0;
					Jpeg_OUT_BufferTab[1].State= JPEG_BUFFER_EMPTY;
					Jpeg_OUT_BufferTab[1].DataBuffer=MCU_Data_OutBuffer1;
					Jpeg_OUT_BufferTab[1].DataBufferSize=0;

					Jpeg_IN_BufferTab[0].State= JPEG_BUFFER_EMPTY;
					Jpeg_IN_BufferTab[0].DataBuffer=JPEG_Data_InBuffer0;
					Jpeg_IN_BufferTab[0].DataBufferSize=0;
					Jpeg_IN_BufferTab[1].State= JPEG_BUFFER_EMPTY;
					Jpeg_IN_BufferTab[1].DataBuffer=JPEG_Data_InBuffer1;
					Jpeg_IN_BufferTab[1].DataBufferSize=0;

					/*##-6- JPEG decoding with DMA (Not Blocking ) Method ################*/

					WaitForAccess(ACCESS_TO_SDCARD);
					JPEG_Decode_DMA(&hjpeg, &JPEG_File, (uint32_t) jpegOutputData);
					ReleaseAccess(ACCESS_TO_SDCARD);

					/*##-7- Wait till end of JPEG decoding and perfom Input/Output Processing in BackGround  #*/

					timeStop=xTaskGetTickCount();
					do  //Jesli tu za dlugo w tej petli to wyjdz !!!!! by nie zawiesic na zawsze!!!!!
					{
						WaitForAccess(ACCESS_TO_SDCARD); //to nie lubi byc w funkcjach tylko w glownej petli watku!!! bo wyskakuje z avi
						JPEG_InputHandler(&hjpeg);
						ReleaseAccess(ACCESS_TO_SDCARD);

						JpegProcessing_End=JPEG_OutputHandler(&hjpeg);

						if (xTaskGetTickCount()-timeStop>2000)
						{
							dbg("\r\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
							Previous_FrameOffset=f_size(&JPEG_File);
							HAL_JPEG_Abort(&hjpeg);
//							ReleaseAccess(ACCESS_TO_SDCARD);
//							aaaaaaaaa();
//							WSTRZYMUJ=0;
//							vTaskDelete(NULL);
							break;
						}

						RefreshData();
						if (WSTRZYMUJ==2)
						{
							Previous_FrameOffset=f_size(&JPEG_File);
							dbg("\r\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
						}

					} while (JpegProcessing_End==0);

					if (isfirstFrame==1)
					{
						isfirstFrame=0;
						/*##-8- Get JPEG Info  ###############################################*/
						HAL_JPEG_GetInfo(&hjpeg, &JPEG_Info);

						/*##-9- Copy RGB decoded Data to the display FrameBuffer  ############*/
						xPos=(800-JPEG_Info.ImageWidth)/2;
						yPos=(480-JPEG_Info.ImageHeight)/2-50;

					}
					//SCB_CleanDCache_by_Addr((uint32_t *)jpegOutputData,800*480*3);
					DMA2D_CopyBuffer((uint32_t *) jpegOutputData, xPos, yPos, JPEG_Info.ImageWidth, JPEG_Info.ImageHeight, 0);
					FrameIndex++;
				}

				ReleaseAccess(ACCESS_TO_SDCARD);

				/*##-12- Calc the current decode frame rate #*/
				endTime=xTaskGetTickCount();
				currentFrameRate=1000/(endTime-startTime);
				//DbgSprintf(250,"\r\ncurrentFrameRate: %d ",currentFrameRate);
#ifdef _TERMINAL
				DbgSprintf(250,"\r\ncurrentFrameRate: %d ",currentFrameRate);
#endif
				FrameRate+=currentFrameRate;
			}

			if (WSTRZYMUJ==2)
				break;

		} while (FrameOffset!=0);

		f_close(&JPEG_File);
		if (FrameIndex>0)
		{
			/*##-12- Calc the average decode frame rate #*/
			FrameRate/=FrameIndex;
			DbgSprintf(250, "\r\nFrameIndex:%d  FrameRate:%d  ", FrameIndex, FrameRate);
		}
	}
	ReleaseAccess(ACCESS_TO_SDCARD);
	SCREEN_LoadFromAVI();
	WSTRZYMUJ=0;
	vTaskDelete(NULL);
}

static xTaskHandle vtaskAviHandle;
void CreateAviTask(void)
{
	xTaskCreate(AVI_test, "avi_test", AVI_TASK_STACK_DEPTH, NULL, (unsigned portBASE_TYPE ) AVI_TASK_PRIORITY, &vtaskAviHandle);
}

/**
 * @brief  JPEG Info ready callback
 * @param hjpeg: JPEG handle pointer
 * @param pInfo: JPEG Info Struct pointer
 * @retval None
 */
void HAL_JPEG_InfoReadyCallback(JPEG_HandleTypeDef *hjpeg, JPEG_ConfTypeDef *pInfo)
{
	if (JPEG_GetDecodeColorConvertFunc(pInfo, &pConvert_Function, &MCU_TotalNb)!=HAL_OK)
	{
		OnError_Handler();
	}
#ifdef _TERMINAL
	dbg("\r\nHAL_JPEG_InfoReadyCallback ");
#endif
}

/**
 * @brief  JPEG Get Data callback
 * @param hjpeg: JPEG handle pointer
 * @param NbDecodedData: Number of decoded (consummed) bytes from input buffer
 * @retval None
 */
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbDecodedData)
{
	if (NbDecodedData==Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize)
	{
		Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].State= JPEG_BUFFER_EMPTY;
		Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize=0;

		JPEG_IN_Read_BufferIndex++;
		if (JPEG_IN_Read_BufferIndex>=NB_INPUT_DATA_BUFFERS)
		{
			JPEG_IN_Read_BufferIndex=0;
		}

		if (Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].State==JPEG_BUFFER_EMPTY)
		{
			HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_INPUT);
			Input_Is_Paused=1;
		}
		else
		{
			HAL_JPEG_ConfigInputBuffer(hjpeg, Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBuffer, Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize);
		}
	}
	else
	{
		HAL_JPEG_ConfigInputBuffer(hjpeg, Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBuffer+NbDecodedData, Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize-NbDecodedData);
	}
#ifdef _TERMINAL
	dbg("\r\nnHAL_JPEG_GetDataCallback ");
#endif
	Previous_FrameOffset+=NbDecodedData;
}

/**
 * @brief  JPEG Data Ready callback
 * @param hjpeg: JPEG handle pointer
 * @param pDataOut: pointer to the output data buffer
 * @param OutDataLength: length of output buffer in bytes
 * @retval None
 */
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
	Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].State= JPEG_BUFFER_FULL;
	Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].DataBufferSize=OutDataLength;

	JPEG_OUT_Write_BufferIndex++;
	if (JPEG_OUT_Write_BufferIndex>=NB_OUTPUT_DATA_BUFFERS)
	{
		JPEG_OUT_Write_BufferIndex=0;
	}

	if (Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].State!=JPEG_BUFFER_EMPTY)
	{
		HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_OUTPUT);
		Output_Is_Paused=1;
	}
	HAL_JPEG_ConfigOutputBuffer(hjpeg, Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].DataBuffer, CHUNK_SIZE_OUT);
#ifdef _TERMINAL
	dbg("\r\nHAL_JPEG_DataReadyCallback ");
#endif
}

/**
 * @brief  JPEG Error callback
 * @param hjpeg: JPEG handle pointer
 * @retval None
 */
void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg)
{
	OnError_Handler();
}

/**
 * @brief  JPEG Decode complete callback
 * @param hjpeg: JPEG handle pointer
 * @retval None
 */
void HAL_JPEG_DecodeCpltCallback(JPEG_HandleTypeDef *hjpeg)
{
	Jpeg_HWDecodingEnd=1;
#ifdef _TERMINAL
	dbg("\r\nHAL_JPEG_DecodeCpltCallback ");
#endif
}

/* USER CODE END 4 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
