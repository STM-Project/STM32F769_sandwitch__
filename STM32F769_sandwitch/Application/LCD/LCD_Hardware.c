/*
 * lcd.c
 *
 *  Created on: 02.02.2021
 *      Author: RafalMar
 */
#include <LCD_Hardware.h>
#include "stm32f7xx_hal.h"
#include "touch.h"
#include "cmsis_os.h"
#include "sdram.h"

#define LCD_FB_START_ADDRESS       ((uint32_t)0xC0E89000) 	//  0xC1000000 - 800*480*4  // CKROC to bo teraz jest *3 anie *4 !!!!!!!!!!!!!!!!!!!

extern LTDC_HandleTypeDef hltdc;
extern DMA2D_HandleTypeDef hdma2d;

static uint32_t  ActiveLayer = 0;

static void LL_ConvertLineToARGB8888(void *pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode)
{
  /* Configure the DMA2D Mode, Color Mode and output offset */
  hdma2d.Init.Mode         = DMA2D_M2M;
  hdma2d.Init.ColorMode    = DMA2D_OUTPUT_RGB888;
  hdma2d.Init.OutputOffset = 0;

  /* Foreground Configuration */
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0xFF;
  hdma2d.LayerCfg[1].InputColorMode = ColorMode;
  hdma2d.LayerCfg[1].InputOffset = 0;

  hdma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
    {
      if (HAL_DMA2D_Start(&hdma2d, (uint32_t)pSrc, (uint32_t)pDst, xSize, 1) == HAL_OK)
      {
        /* Polling For DMA transfer */
        HAL_DMA2D_PollForTransfer(&hdma2d, 10);
      }
    }
  }
}

static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
  /* Register to memory mode with ARGB8888 as color Mode */
  hdma2d.Init.Mode         = DMA2D_R2M;
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
	  hdma2d.Init.ColorMode    = DMA2D_RGB565;
  }
  else
  { /* ARGB8888 format */
	  hdma2d.Init.ColorMode    = DMA2D_RGB888;
  }
  hdma2d.Init.OutputOffset = OffLine;

  hdma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d, LayerIndex) == HAL_OK)
    {
      if (HAL_DMA2D_Start(&hdma2d, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        /* Polling For DMA transfer */
        HAL_DMA2D_PollForTransfer(&hdma2d, 50);
      }
    }
  }
}

void BSP_LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, width = 0, height = 0, bit_pixel = 0;
  uint32_t address;
  uint32_t input_color_mode = 0;

  /* Get bitmap data address offset */
  index = pbmp[10] + (pbmp[11] << 8) + (pbmp[12] << 16)  + (pbmp[13] << 24);

  /* Read bitmap width */
  width = pbmp[18] + (pbmp[19] << 8) + (pbmp[20] << 16)  + (pbmp[21] << 24);

  /* Read bitmap height */
  height = pbmp[22] + (pbmp[23] << 8) + (pbmp[24] << 16)  + (pbmp[25] << 24);

  /* Read bit/pixel */
  bit_pixel = pbmp[28] + (pbmp[29] << 8);

	touchTemp.x_Start= Xpos;
	touchTemp.y_Start= Ypos;

  /* Set the address */
  address = hltdc.LayerCfg[ActiveLayer].FBStartAdress + (((LCD_GetXSize()*Ypos) + Xpos)*(3));

  /* Get the layer pixel format */
  if ((bit_pixel/8) == 4)
  {
    input_color_mode = CM_ARGB8888;
  }
  else if ((bit_pixel/8) == 2)
  {
    input_color_mode = CM_RGB565;
  }
  else
  {
    input_color_mode = CM_RGB888;
  }

  /* Bypass the bitmap header */
  pbmp += (index + (width * (height - 1) * (bit_pixel/8)));

  /* Convert picture to ARGB8888 pixel format */
  for(index=0; index < height; index++)
  {
    /* Pixel format conversion */
    LL_ConvertLineToARGB8888((uint32_t *)pbmp, (uint32_t *)address, width, input_color_mode);

    /* Increment the source and destination buffers */
    address+=  (LCD_GetXSize()*3);
    pbmp -= width*(bit_pixel/8);
  }
  touchTemp.x_End= touchTemp.x_Start+width;
  touchTemp.y_End= touchTemp.y_Start+height;
}

void DMA2D_CopyBuffer(uint32_t *pSrc, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize, uint32_t width_offset)
{

  uint32_t destination = hltdc.LayerCfg[ActiveLayer].FBStartAdress + (y * LCD_GetXSize() + x) * 3;
  uint32_t source      = (uint32_t)pSrc;

  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
  hdma2d.Init.Mode          = DMA2D_M2M_PFC;
  hdma2d.Init.ColorMode     = DMA2D_OUTPUT_RGB888;
  hdma2d.Init.OutputOffset  = LCD_GetXSize() - xsize;
  hdma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;  /* No Output Alpha Inversion*/
  hdma2d.Init.RedBlueSwap   = DMA2D_RB_REGULAR;     /* No Output Red & Blue swap */

  /*##-2- DMA2D Callbacks Configuration ######################################*/
  hdma2d.XferCpltCallback  = NULL;

  /*##-3- Foreground Configuration ###########################################*/
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0xFF;

//#if (JPEG_RGB_FORMAT == JPEG_ARGB8888)
//  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
//
//#elif (JPEG_RGB_FORMAT == JPEG_RGB888)
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB888;
//
//#elif (JPEG_RGB_FORMAT == JPEG_RGB565)
//  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
//
//#endif /* JPEG_RGB_FORMAT * */


  hdma2d.LayerCfg[1].InputOffset = width_offset;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

  hdma2d.Instance          = DMA2D;

  /* DMA2D Initialization */

//
//  taskENTER_CRITICAL();
//  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)  //zlikwiduj to - tylko raz wywoluj w main()
//  {
//    if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
//    {
//      if (HAL_DMA2D_Start(&hdma2d, source, destination, xsize, ysize) == HAL_OK)
//      {
//        /* Polling For DMA transfer */
//        HAL_DMA2D_PollForTransfer(&hdma2d, 50);
//      }
//    }
//  }
//  taskEXIT_CRITICAL();




  //taskENTER_CRITICAL();
  while (DMA2D->CR & DMA2D_CR_START);
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
    {

   	 HAL_DMA2D_Start_IT(&hdma2d, source, destination, xsize, ysize);

    }
  }
 // while (DMA2D->CR & DMA2D_CR_START);
  //taskEXIT_CRITICAL();

}

void BSP_LCD_Display(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint32_t *pbmp)
{
  uint32_t index = 0, bit_pixel = 24;

  uint32_t address;
  uint32_t input_color_mode = 0;

  /* Read bit/pixel */
  bit_pixel/=8;

  /* Set the address */
  address = hltdc.LayerCfg[ActiveLayer].FBStartAdress + (((LCD_GetXSize()*Ypos) + Xpos));
  input_color_mode = CM_ARGB8888;



    /* Configure the DMA2D Mode, Color Mode and output offset */
    hdma2d.Init.Mode         = DMA2D_M2M_PFC;
    hdma2d.Init.ColorMode    = DMA2D_RGB888;
    hdma2d.Init.OutputOffset = LCD_GetXSize() - width;

    /* Foreground Configuration */
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;
    hdma2d.LayerCfg[1].InputColorMode = input_color_mode;
    hdma2d.LayerCfg[1].InputOffset = 0;

    hdma2d.Instance = DMA2D;
    hdma2d.XferCpltCallback  = NULL;


    //SCB_InvalidateDCache_by_Addr((uint32_t *) pbmp, width*height*3);
    //taskENTER_CRITICAL();   //Critical trzeba dac jesli mamy opt
    /* DMA2D Initialization */
    while (DMA2D->CR & DMA2D_CR_START);
    if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
    {
      if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
      {
      	HAL_DMA2D_Start_IT(&hdma2d, (uint32_t)pbmp, address, width, height);
      }
    }
    while (DMA2D->CR & DMA2D_CR_START);
    //taskEXIT_CRITICAL();



}

void BSP_LCD_DrawXXX(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint8_t *pbmp)
{
  uint32_t index = 0, bit_pixel = 24;

  uint32_t address;
  uint32_t input_color_mode = 0;

  /* Read bit/pixel */
  bit_pixel/=8;

  /* Set the address */
  address = hltdc.LayerCfg[ActiveLayer].FBStartAdress + (((LCD_GetXSize()*Ypos) + Xpos)*(3));  //4
  input_color_mode = CM_RGB888;






//  taskENTER_CRITICAL();
//  /* Convert picture to ARGB8888 pixel format */
//  for(index=0; index < height; index++)
//  {
//    /* Pixel format conversion */
//    LL_ConvertLineToARGB8888((uint32_t *)pbmp_p, (uint32_t *)address, width, input_color_mode);
//
//    /* Increment the source and destination buffers */
//    address+=  (LCD_GetXSize()*3);
//    pbmp_p += width*bit_pixel;
//  }
//  taskEXIT_CRITICAL();







    /* Configure the DMA2D Mode, Color Mode and output offset */
    hdma2d.Init.Mode         = DMA2D_M2M;
    hdma2d.Init.ColorMode    = DMA2D_RGB888;
    hdma2d.Init.OutputOffset = LCD_GetXSize() - width;

    /* Foreground Configuration */
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;
    hdma2d.LayerCfg[1].InputColorMode = input_color_mode;
    hdma2d.LayerCfg[1].InputOffset = 0;

    hdma2d.Instance = DMA2D;
    hdma2d.XferCpltCallback  = NULL;

   //taskENTER_CRITICAL();   //Critical trzeba dac jesli mamy opt
    /* DMA2D Initialization */
    while (DMA2D->CR & DMA2D_CR_START);
    if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
    {
      if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
      {
      	HAL_DMA2D_Start_IT(&hdma2d, (uint32_t)pbmp, address, width, height);
      }
    }
    while (DMA2D->CR & DMA2D_CR_START);
    //taskEXIT_CRITICAL();







//  /* Configure the DMA2D Mode, Color Mode and output offset */
//  hdma2d.Init.Mode         = DMA2D_M2M;
//  hdma2d.Init.ColorMode    = DMA2D_RGB888;
//  hdma2d.Init.OutputOffset = LCD_GetXSize() - width;
//
//  /* Foreground Configuration */
//  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
//  hdma2d.LayerCfg[1].InputAlpha = 0xFF;
//  hdma2d.LayerCfg[1].InputColorMode = input_color_mode;
//  hdma2d.LayerCfg[1].InputOffset = 0;
//
//  hdma2d.Instance = DMA2D;
//  hdma2d.XferCpltCallback  = NULL;
//
//  taskENTER_CRITICAL();
//  /* DMA2D Initialization */
//  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
//  {
//    if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
//    {
//      if (HAL_DMA2D_Start(&hdma2d, (uint32_t)pbmp, address, width, 1) == HAL_OK)
//      {
//        /* Polling For DMA transfer */
//        HAL_DMA2D_PollForTransfer(&hdma2d, 10);
//      }
//      if (HAL_DMA2D_Start(&hdma2d, (uint32_t)pbmp, address, width, height) == HAL_OK)
//      {
//        /* Polling For DMA transfer */
//        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
//      }
//
//
//    }
//  }
//  taskEXIT_CRITICAL();



}

uint32_t LCD_GetXSize(void)
{
  return hltdc.LayerCfg[ActiveLayer].ImageWidth;
}

uint32_t LCD_GetYSize(void)
{
  return hltdc.LayerCfg[ActiveLayer].ImageHeight;
}

static void BSP_LCD_LayerDefaultInit(uint16_t LayerIndex, uint32_t FB_Address)
{
	LTDC_LayerCfgTypeDef  layer_cfg;

  /* Layer Init */
  layer_cfg.WindowX0 = 0;
  layer_cfg.WindowX1 = LCD_GetXSize();
  layer_cfg.WindowY0 = 0;
  layer_cfg.WindowY1 = LCD_GetYSize();
  layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;//LTDC_PIXEL_FORMAT_ARGB8888;
  layer_cfg.FBStartAdress = FB_Address;
  layer_cfg.Alpha = 255;
  layer_cfg.Alpha0 = 0;
  layer_cfg.Backcolor.Blue = 0;
  layer_cfg.Backcolor.Green = 0;
  layer_cfg.Backcolor.Red = 0;
  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  layer_cfg.ImageWidth = LCD_GetXSize();
  layer_cfg.ImageHeight = LCD_GetYSize();

  HAL_LTDC_ConfigLayer(&hltdc, &layer_cfg, LayerIndex);
}

void LCD_Init(void)
{
	BSP_LCD_LayerDefaultInit(ActiveLayer, LCD_FB_START_ADDRESS);
	BSP_LCD_Clear(0xFF000000);
}

void BSP_LCD_Clear(uint32_t Color)
{
  /* Clear the LCD */
  LL_FillBuffer(ActiveLayer, (uint32_t *)(hltdc.LayerCfg[ActiveLayer].FBStartAdress), LCD_GetXSize(), LCD_GetYSize(), 0, Color);
}

void BSP_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;  int color=0xFFFF0000;

  /* Get the line address */
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);
  }
  else
  { /* ARGB8888 format */
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }

  /* Write line */
  LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, Length, 1, 0, color);
}

void BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{
  /* Write data value to all SDRAM memory */
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
    *(__IO uint16_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;
  }
  else
  { /* ARGB8888 format */
    *(__IO uint32_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))) = RGB_Code;
  }
}

