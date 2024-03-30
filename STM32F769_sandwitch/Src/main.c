/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <LCD_BasicGaphics.h>
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "libjpeg.h"
#include "mbedtls.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>

#include "api.h"
#include "netif.h"
#include "ethernetif.h"
#include "tcpip.h"

#include "ssl_client.h"
#include "http.h"
#include "jpeg_utils.h"
#include "png_dec.h"
#include "touch.h"
#include "stmpe811.h"
#include "SDCard.h"
#include "StartUp.h"
#include "ssl_server.h"
#include "variables.h"
#include "mini_printf.h"
#include "sdram.h"
#include "fonts_images.h"

#include "dbg.h"
#include "smtp.h"
#include "esp32wroom.h"
#include "lang.h"
#include "SCREEN_Common.h"
#include "sdram.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((section(".sdram")));

char wybor_bitowy;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c1;

JPEG_HandleTypeDef hjpeg;
DMA_HandleTypeDef hdma_jpeg_in;
DMA_HandleTypeDef hdma_jpeg_out;

LTDC_HandleTypeDef hltdc;

RNG_HandleTypeDef hrng;

SD_HandleTypeDef hsd1;
DMA_HandleTypeDef hdma_sdmmc1_rx;
DMA_HandleTypeDef hdma_sdmmc1_tx;

UART_HandleTypeDef huart7;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_uart7_rx;
DMA_HandleTypeDef hdma_uart7_tx;
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;

SDRAM_HandleTypeDef hsdram1;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
osThreadId defaultTaskHandle3,defaultTaskHandle4;
int moj_wsk;   int prescVal=13;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RNG_Init(void);
static void MX_CRC_Init(void);
static void MX_UART7_Init(void);
static void MX_USART6_Init(void);
static void MX_FMC_Init(void);
static void MX_LTDC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_JPEG_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */

void MPU_Config_My(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


int semafora=0;


xTaskHandle vtaskBSPHandle;


#define _JumpToProgr           0x8080000
typedef  void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t JumpAddress, APPLICATION_ADDRESS;

void JumpToUserProgram()
{
	 APPLICATION_ADDRESS=_JumpToProgr;
	// Test if user code is programmed starting from address "APPLICATION_ADDRESS"
	 dbg("\r\nSKOK...");  HAL_Delay(100);

	// sprintf(buf_p,"\r\nAAA: %x  ", (*(__IO uint32_t*)APPLICATION_ADDRESS));  dbg(buf_p);
  	                if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FE00000 ) == 0x20000000)
  	    		    {

  	                	//LCD_Text(385,200-3,"OK    ",0x202020,0xFFFFFF,&Font20);     osDelay(4000);

  	                	uint32_t appStack;
  	                	pFunction appEntry;

  	                	// get the application stack pointer (1st entry in the app vector table)
  	                	appStack = (uint32_t)*((__IO uint32_t*)_JumpToProgr);

  	                	// Get the app entry point (2nd entry in the app vector table
  	                	appEntry = (pFunction)*(__IO uint32_t*)(_JumpToProgr + 4);

  	                	//boot_basicClockConfig();
  	                	HAL_RCC_DeInit();
  	                	HAL_DeInit();

  	                	SysTick->CTRL = 0;
  	                	SysTick->LOAD = 0;
  	                	SysTick->VAL  = 0;

  	                	__set_MSP(appStack); // Set app stack pointer
  	                	appEntry(); // Start the app

  	                	while (1); // never reached
  	    		    }
  	                else
  	                 {
  	                	dbg("\r\nNIE !!!!...");
  	                	//LCD_Text(385,200-3,"ERROR ",0x202020,0xFF0000,&Font20);
  	                 }

}
void JumpToUserProgram__()
{
	 APPLICATION_ADDRESS=_JumpToProgr;
	// Test if user code is programmed starting from address "APPLICATION_ADDRESS"
	 dbg("\r\nSKOK...");  HAL_Delay(100);

	// sprintf(buf_p,"\r\nAAA: %x  ", (*(__IO uint32_t*)APPLICATION_ADDRESS));  dbg(buf_p);
  	                if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FE00000 ) == 0x20000000)
  	    		    {

  	                	uint32_t appStack;
  	                	pFunction appEntry;

  	                	// get the application stack pointer (1st entry in the app vector table)
  	                	appStack = (uint32_t)*((__IO uint32_t*)_JumpToProgr);

  	                	// Get the app entry point (2nd entry in the app vector table
  	                	appEntry = (pFunction)*(__IO uint32_t*)(_JumpToProgr + 4);

  	                	//boot_basicClockConfig();
  	                	HAL_RCC_DeInit();
  	                	HAL_DeInit();

  	                	SysTick->CTRL = 0;
  	                	SysTick->LOAD = 0;
  	                	SysTick->VAL  = 0;

  	                	__set_MSP(appStack); // Set app stack pointer
  	                	appEntry(); // Start the app

  	                	while (1); // never reached
  	    		    }
  	                else
  	                {
  	                	dbg("\r\nNIE !!!!...");
  	                }

}

//################################# JPEG  ############################################################
//################################# JPEG  ############################################################
//################################# JPEG  ############################################################
SRAM char buffff[4096];
void DynWebPage(void)
{
  portCHAR PAGE_BODY[512];
  portCHAR pagehits[10] = {0};

  memset(PAGE_BODY, 0,512);

  /* Update the hit count */
  strcat(PAGE_BODY, pagehits);
  strcat((char *)PAGE_BODY, "\r\nName          State  Priority  Stack   Num" );
  strcat((char *)PAGE_BODY, "\r\n---------------------------------------------\r\n");

  /* The list of tasks and their status */
  osThreadList((unsigned char *)(PAGE_BODY + strlen(PAGE_BODY)));
  strcat((char *)PAGE_BODY, "\r\n---------------------------------------------");
  strcat((char *)PAGE_BODY, "\r\nB : Blocked, R : Ready, D : Deleted, S : Suspended\r\n");

  /* Send the dynamically generated page */
  dbg(PAGE_BODY);
}



//#######################################################################################################################
//#######################################################################################################################
//#######################################################################################################################
//
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)   // !!!!!!!!! SD CARD nie ma wyrownania i moze trzeba ALIGN_32BYTES(buff_for_sd_read);
{
  /* USER CODE BEGIN 1 */

	//UWAGI @@@@@@@@@@@@@@@@@

//	!!!!! stosuj tux przed wyslaniem do DMA:      SCB_CleanDCache_by_Addr((uint32_t*)alignedAddr, count*BLOCKSIZE + ((uint32_t)buff - alignedAddr));
	//!!!! stosuje zaraz po otrzymaniu od DMA:      SCB_InvalidateDCache_by_Addr((uint32_t*)scratch, BLOCKSIZE);
  //np w wifi zasosowac przed dma recv !!!



//	SCB_DisableICache();
//	SCB_DisableDCache();

	MPU_Config_My();
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
 // MPU_Config();

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

// wykonywanie watek nie rownoczesnie tylko po kolei za pomoc¹ eventGroupBits !!!!!!

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RNG_Init();
  MX_CRC_Init();
  MX_USART6_Init();
  MX_UART7_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  MX_DMA2D_Init();
  MX_SDMMC1_SD_Init();
  MX_JPEG_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */



  HAL_Delay(3000);

  SDRAM_Init();  //poniewaz sterte mamy w SDRAM to przed funkcjami systemowymi jimkolwiek trzeba dac SDRAM_Init(); !!!!!

  LCD_Init();

  HAL_Delay(2000);

  dbg("\r\nKURWA "); //  EspTx("\r\nEEESSSPPP !!!!");

  if(0==SDRAM_Test())
  {
	dbg("\r\nERROR SDRAM !!!");
	osDelay(1000);
	MX_FMC_Init();
	SDRAM_Init();
  }
 // DbgSprintf(50,"\r\nTest SDRAM: %d ",TestSDRAM());

  STARTUP_Init();
  MX_FATFS_Init();
  SDCardStart();

  Init_DBG();


  BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());



  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 3*512);  //Watek z odswiez ekranu wyzszy prio od pozostalych jesli nie to szarpie obrazem MAMY tu taskENTER_CRITICAL();
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
 // CreateWifiTask();
 // vtaskWifi___();
  //Create_ETH_DHCP_HTTP_Thread();    //osPriorityLow  dla SSL   //osPriorityNormal  dla HTTP

 // Create_ETH_DHCP_HTTPS_Thread();



  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
 osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /**Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_USART6|RCC_PERIPHCLK_UART7
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_SDMMC1
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 3; //2
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  PeriphClkInitStruct.Usart6ClockSelection = RCC_USART6CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Uart7ClockSelection = RCC_UART7CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_OUTPUT_RGB888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20404768;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief JPEG Initialization Function
  * @param None
  * @retval None
  */
static void MX_JPEG_Init(void)
{

  /* USER CODE BEGIN JPEG_Init 0 */
	JPEG_InitColorTables();
  /* USER CODE END JPEG_Init 0 */

  /* USER CODE BEGIN JPEG_Init 1 */

  /* USER CODE END JPEG_Init 1 */
  hjpeg.Instance = JPEG;
  if (HAL_JPEG_Init(&hjpeg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN JPEG_Init 2 */

  /* USER CODE END JPEG_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 39;
  hltdc.Init.VerticalSync = 3;
  hltdc.Init.AccumulatedHBP = 46;
  hltdc.Init.AccumulatedVBP = 5;
  hltdc.Init.AccumulatedActiveW = 846;
  hltdc.Init.AccumulatedActiveH = 485;
  hltdc.Init.TotalWidth = 852;
  hltdc.Init.TotalHeigh = 487;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 800;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 480;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;//LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 0;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0xC0E89000;
  pLayerCfg.ImageWidth = 800;
  pLayerCfg.ImageHeight = 480;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief UART7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART7_Init(void)
{

  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 2000000;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */

  /* USER CODE END UART7_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 15, 0);  //JPEG
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 15, 0);  //JPEG
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 7, 0);   //SD
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 7, 0);  //SD
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 8, 0);   //hdma_usart6_rx  wifi
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 8, 0);   //hdma_usart6_tx  wifi
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}
/* FMC initialization function */
static void MX_FMC_Init(void)
{
  FMC_SDRAM_TimingTypeDef SdramTiming;

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;  //check in datasheet -> column address
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;      //check in datasheet -> row address
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_2;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 2;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);    //ESP_EN

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOK, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PK7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


  GPIO_InitStruct.Pin = GPIO_PIN_9;  //ESP_EN
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)  //To umiscic w src uart.c!!!!!
{
	if(huart->Instance==UART7)
	{
		__HAL_UART_CLEAR_FEFLAG(huart);
		__HAL_UART_CLEAR_PEFLAG(huart);
		__HAL_UART_CLEAR_OREFLAG(huart);
		__HAL_UART_FLUSH_DRREGISTER(huart);
	}
	else if(huart->Instance==USART6)
	{
		__HAL_UART_CLEAR_FEFLAG(huart);
		__HAL_UART_CLEAR_PEFLAG(huart);
		__HAL_UART_CLEAR_OREFLAG(huart);
		__HAL_UART_FLUSH_DRREGISTER(huart);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==UART7)
	{
		DBG_RxCallbackService();
		dbg("\r\n -----  USART7  HAL_UART_RxCpltCallback -------  ");
	}
	else if(huart->Instance==USART6)
	{
		WIFI_RxCallbackService();
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)  //W PRZERWANIACH TYLKO FLAGI !!!!!!
{
	if(huart->Instance==UART7)
	{
		dbg("\r\nERROR UART7 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% ");
	}
	else if(huart->Instance==USART6)
	{
		WIFI_UartErrorService();
	}
}

void MPU_Config_My(void)
{
	  MPU_Region_InitTypeDef MPU_InitStruct = {0};

	  /* Disables the MPU */
	  HAL_MPU_Disable();
	  /**Initializes and configures the Region and the memory to be protected
	  */
	  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	  MPU_InitStruct.BaseAddress = 0x20000000;
	  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
	  MPU_InitStruct.SubRegionDisable = 0x0;
	  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
	  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

	  HAL_MPU_ConfigRegion(&MPU_InitStruct);


	  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	  MPU_InitStruct.BaseAddress = 0xC0000000;
	  MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
	  MPU_InitStruct.SubRegionDisable = 0x0;
	  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

	  HAL_MPU_ConfigRegion(&MPU_InitStruct);


	  /* Enables the MPU */
	  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void DBG_Service(void)
{
	if(isAnything_Recv())
	{
	  if(isDBG_Recv("set"))
	  {
		  STARTUP_SetBits(0x0002);   dbg(" Set ");
	  }
	  if(isDBG_Recv("ko1"))
	  {
		  dbg("\r\nKoniec HTTPS ");
		  Koniec_SSL_Server_Watek();
	  }
	  if(isDBG_Recv("ko2"))
	  {
		  dbg("\r\nKoniec SMTPS ");
		  Koniec_SSL_client_Watek();
	  }
	  if(isDBG_Recv("sta"))
	  {
		  dbg("\r\nStart HTTPS ");
		  Create_ETH_DHCP_HTTPS_Thread();
	  }
	  if(isDBG_Recv("aa"))
	  {
		  dbg("\r\nHURA22 !!!!");
			osThreadDef(Clientt, SSL_Client, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 10);
			defaultTaskHandle3=osThreadCreate (osThread(Clientt), NULL);
	  }

	  if(isDBG_Recv("qq"))
	  {
		  dbg("\r\nHURA !!!!");
		  moj_wsk=1;
	  }
	  if(isDBG_Recv("jj"))
	  {
		  dbg("\r\nJUMP....."); HAL_Delay(100);
		  JumpToUserProgram__();
	  }
	  if(isDBG_Recv("dd"))
	  {
		  DynWebPage();
	  }
	  if(isDBG_Recv("vv"))
	  {
		  dbg("\r\nvv");

	  }
	  if(isDBG_Recv("gg"))
	  {
		  dbg("\r\ngg");


	  }
	  if(isDBG_Recv("cc"))
	  {
		  dbg("\r\ncc");
		  RestartWifiTask();

	  }
	  if(isDBG_Recv("rr"))
	  {
		  dbg("\r\nrr");
		  vRestartWifiServer();

	  }
	  if(isDBG_Recv("tt"))
	  {
		  dbg("\r\ntt");

	  }
	  if(isDBG_Recv("test"))
	  {
		  dbg("\r\ntest");
		  vTestATcommand();

	  }



	  if(isDBG_Recv("restore"))
	  {
		  dbg("\r\nrestore");
		  vRestoreESP();

	  }
	  if(isDBG_Recv("dns1"))
	  {
		  dbg("\r\ndns");
		  vDNSdomain();

	  }
	  if(isDBG_Recv("ee1"))
	  {
		  dbg("\r\nee1");
		  SendEmail(0, 1<<5|1<<1, EMAIL_TEST);
	  }
	  if(isDBG_Recv("ee2"))
	  {
		  dbg("\r\nee2");
		  SendEmail(1, 1<<5|1<<1, EMAIL_TEST);
	  }
	  if(isDBG_Recv("ss"))
	  {
		  dbg("\r\nss");

		  LCD_Obraz_imageSS();

	  }
	  if(isDBG_Recv("pp"))
	  {
		  Touchscreen_Calibration();
		  dbg("\r\npp");
	  }
	  if(isDBG_Recv("pn"))
	  {
		 // png_dec_test();
		  dbg("\r\npp");
	  }
	  if(isDBG_Recv("ff"))
	  {
		  dbg("\r\nff");
		  for(int i=0;i<3;i++){ dbg("W"); LCD_FontsTest(i,Arial,BROWN,WHITE);  osDelay(7000);  }
		  for(int i=0;i<3;i++){dbg("R");  LCD_FontsTest(i,Arial,WHITE,BLACK);   osDelay(7000); }

//		  for(int i=0;i<3;i++){ dbg("W"); LCD_FontsTest(i,Arial,DARKGRAY,WHITE);  osDelay(7000);  }
//		  for(int i=0;i<3;i++){dbg("R");  LCD_FontsTest(i,Arial,DARKGRAY,RED);   osDelay(7000); }
//		  for(int i=0;i<3;i++){ dbg("G"); LCD_FontsTest(i,Arial,DARKGRAY,GREEN); osDelay(7000);   }
//		  for(int i=0;i<3;i++){ dbg("B");  LCD_FontsTest(i,Arial,DARKGRAY,BLUE);  osDelay(7000);  }
		  dbg("\r\nKoniec ");
	  }
	  if(isDBG_Recv("a1"))
	  {
		  SCB_EnableDCache();
		  dbg("\r\na1");
	  }
	  if(isDBG_Recv("a2"))
	  {
		  SCB_DisableDCache();
		  dbg("\r\na2");
	  }
	  if(isDBG_Recv("11")){ if(prescVal<16) prescVal++;  SCREEN_Load(MainPanel,prescVal,0);   }
	  if(isDBG_Recv("22")){ if(prescVal) prescVal--;     SCREEN_Load(MainPanel,prescVal,0);   }
	  if(isDBG_Recv("88")){ while(1){ if(prescVal<16) prescVal++; else break;  SCREEN_Load(MainPanel,prescVal,0); }   }
	}
}

int WSTRZYMUJ=0;
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  //MX_FATFS_Init();

  /* MX_LWIP_Init() is generated within mbedtls_net_init() function in net_cockets.c file */
  /* Up to user to call mbedtls_net_init() function in MBEDTLS initialization step */

  /* Up to user define the empty MX_MBEDTLS_Init() function located in mbedtls.c file */
  MX_MBEDTLS_Init();

  /* init code for LIBJPEG */
  MX_LIBJPEG_Init();

  /* USER CODE BEGIN 5 */

  STARTUP_SetBits(0x0002|0x0004|0x0001);

  STARTUP_SetBits(0x0010|0x0011);

  portTickType xLastExecutionTime;


  	xLastExecutionTime = xTaskGetTickCount();
HAL_Delay(1);
  	DbgSprintf(250,"\r\nTime1: %d ",xTaskGetTickCount()-xLastExecutionTime);

  	xLastExecutionTime = xTaskGetTickCount();
HAL_Delay(143);
  	DbgSprintf(250,"\r\nTime143: %d ",xTaskGetTickCount()-xLastExecutionTime);

  //	vTaskDelayUntil(&xLastExecutionTime, 300);


   //LCD_Obraz_1();
 // ChoiceMainPanel(0);

 int naRaz=0;

 SetLang(2,Polish);







xLastExecutionTime = xTaskGetTickCount();

LCD_DrawBoldRoundRectangle3_TEST(0,0, 800, 480, 0xff0ff00, 0xff0ff00,0xff0ff00);

DbgSprintf(250,"\r\nAQAQAQAQAQA: %d ",xTaskGetTickCount()-xLastExecutionTime);










  /* Infinite loop */
  for(;;)
  {
//	  if..
//	  else
//	    continue;

//if (0x01 & xEventGroupGetBits(xBoardResetEvent))


//	  1- main
//
//	  2- lan  ssl
//
//	  4- sslclient

	   if (STARTUP_WaitForBits(0x0002))  //zlikwidowac to
		   { if (STARTUP_WaitForBits(0x0004)){

				  STARTUP_ClaerBits(0x0001);

				  if(naRaz==0)
				  {
					  naRaz=1;
					  SCREEN_Load(MainPanel,prescVal,0);
					  InfoForImagesFonts();
					 // DisplayFontsStructState();

					  CreateWifiTask();
				  }

				  DBG_Service();


				 TouchService();   if(WSTRZYMUJ==0) RefreshData();

STARTUP_SetBits(0x0001);
}}

	//LCD_DrawAnimation("Animation/butterfly1",100, 60, 350);
//	LCD_DrawAnimation("Animation/butterfly2",200, 120, 350);
//	LCD_DrawAnimation("Animation/butterfly3",300, 230, 350);
//	LCD_DrawAnimation("Animation/butterfly4",400, 340, 350);
//	LCD_DrawAnimation("Animation/butterfly5",500, 450, 350);
//	LCD_DrawAnimation("Animation/butterfly6",600, 560, 350);

	if(touchPressed==0) osDelay(50); // LCD_Obraz_1();
	//EspTx("i");



  }
  /* USER CODE END 5 */ 
}

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
  /**Initializes and configures the Region and the memory to be protected 
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}
/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
dbg("\r\n !!! -- Error_Handler -- !!! ");
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
