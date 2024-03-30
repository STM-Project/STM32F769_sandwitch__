/*
 * sdram.c
 *
 *  Created on: 16.07.2020
 *      Author: Elektronika RM
 */

#include "stm32f7xx_hal.h"
#include "sdram.h"
#include "dbg.h"

#define SDRAM_DEVICE_ADDR         ((uint32_t)0xC0000000)
#define SDRAM_DEVICE_SIZE         ((uint32_t)0x01000000)

#define SDRAM_BANK_ADDR                 ((uint32_t)0xC0E00000)  //16MB SDRAMu

SDRAM_HandleTypeDef hsdram1;
FMC_SDRAM_CommandTypeDef command;

#define SDRAM_TIMEOUT     ((uint32_t)0xFFFF)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)



static void SDRAM_Write(uint32_t *buf, uint32_t len, uint32_t addr )   // "addr" i "len" musi byc podzielne przez 4
{
	uint32_t i;

	for(i=0;i<len;i++)
	{
	     *(__IO uint32_t*) (SDRAM_BANK_ADDR + addr + 4*i) = buf[i];
	}
}

static void SDRAM_Read(uint32_t *buf, uint32_t len, uint32_t addr )   // "addr" i "len" musi byc podzielne przez 4
{
	uint32_t i;

	for(i=0;i<len;i++)
	{
		buf[i] = *(__IO uint32_t*) (SDRAM_BANK_ADDR + addr + 4*i);
	}
}

static void BSP_SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
	  __IO uint32_t tmpmrd =0;
	  /* Step 3:  Configure a clock configuration enable command */
	  Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
	  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	  Command->AutoRefreshNumber = 1;
	  Command->ModeRegisterDefinition = 0;

	  /* Send the command */
	  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

	  /* Step 4: Insert 100 us minimum delay */
	  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
	  HAL_Delay(1);

	  /* Step 5: Configure a PALL (precharge all) command */
	  Command->CommandMode = FMC_SDRAM_CMD_PALL;
	  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	  Command->AutoRefreshNumber = 1;
	  Command->ModeRegisterDefinition = 0;

	  /* Send the command */
	  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

	  /* Step 6 : Configure a Auto-Refresh command */
	  Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	  Command->AutoRefreshNumber = 8;
	  Command->ModeRegisterDefinition = 0;

	  /* Send the command */
	  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

	  /* Step 7: Program the external memory mode register */
	  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4          |
	                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
						 SDRAM_MODEREG_CAS_LATENCY_2           |
	                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
	                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	  Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	  Command->AutoRefreshNumber = 1;
	  Command->ModeRegisterDefinition = tmpmrd;

	  /* Send the command */
	  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

	  /* Step 8: Set the refresh rate counter */
	  /* (15.62 us x Freq) - 20 */
	  /* Set the device refresh counter */
	  hsdram->Instance->SDRTR |= ((uint32_t)((1292)<< 1));

}

void SDRAM_Init(void)
{
	BSP_SDRAM_Initialization_Sequence(&hsdram1, &command);
}

int SDRAM_Test(void)
{
		uint32_t i;

uint32_t buff[200];

		  for(i=0;i<200;i++) buff[i]=i;
		  SDRAM_Write(buff,200,0);
		  for(i=0;i<200;i++) buff[i]=0;
		  SDRAM_Read(buff,200,0);
		  for(i=0;i<200;i++)
		  {
		  	 if(buff[i]==i);
		  	 else
		  	 {
		  	    return 0;
		  	 }
		  }
		  return 1;
}

int TestSDRAM(void)
{
	uint32_t WriteReadAddr = 4;
	uint32_t aTxBuffer;
	uint32_t aRxBuffer;

	uint32_t uwWriteReadStatus = 0;

	uint32_t Sdram_addr = 0;
	long int i, iter;

	iter = SDRAM_DEVICE_SIZE / WriteReadAddr;

	//-----------------------------START TEST 1-----------------------------

	for (i = 0; i < iter; i++)
	{
		Sdram_addr = SDRAM_DEVICE_ADDR + WriteReadAddr * i;
		aTxBuffer = Sdram_addr;
		*(volatile uint32_t*) Sdram_addr = aTxBuffer;
	}

	for (i = 0; i < iter; i++)
	{
		aRxBuffer = 0x0;
		Sdram_addr = SDRAM_DEVICE_ADDR + WriteReadAddr * i;
		aRxBuffer = *(volatile uint32_t*) Sdram_addr;

		if (aRxBuffer != Sdram_addr)
			uwWriteReadStatus++;

		if (uwWriteReadStatus)
		{
			//BLUE_LED_OFF;
			//RED_LED_ON;
dbg("#");
			HAL_Delay(750);
			//RED_LED_OFF;
			break;
		}

	}
	if (!uwWriteReadStatus)
	{
		//BLUE_LED_ON;
		dbg("\r\nOK_1 SDRAM");
		//BLUE_LED_OFF;
	}
	else return 1;

	uwWriteReadStatus = 0;

	//------------------------------END TEST 1------------------------------

	//-----------------------------START TEST 2-----------------------------

	for (i = 0; i < iter; i++)
	{
		Sdram_addr = SDRAM_DEVICE_ADDR + WriteReadAddr * i;
		aTxBuffer = ~Sdram_addr;
		*(volatile uint32_t*) Sdram_addr = aTxBuffer;
	}

	for (i = 0; i < iter; i++)
	{
		aRxBuffer = 0x0;
		Sdram_addr = SDRAM_DEVICE_ADDR + WriteReadAddr * i;
		aRxBuffer = *(volatile uint32_t*) Sdram_addr;

		if (aRxBuffer != ~Sdram_addr)
			uwWriteReadStatus++;

		if (uwWriteReadStatus)
		{
			//BLUE_LED_OFF;
			//RED_LED_ON;
			dbg("#");
			HAL_Delay(750);
			//RED_LED_OFF;
			break;
		}

	}
	if (!uwWriteReadStatus)
	{
		//BLUE_LED_ON;
		dbg("\r\nOK_2 SDRAM");
		return 0;
		//BLUE_LED_OFF;
	}
	else return 2;

	uwWriteReadStatus = 0;
	//------------------------------END TEST 2------------------------------
}

