/*
 * sdram.h
 *
 *  Created on: 16.07.2020
 *      Author: Elektronika RM
 */

#ifndef SDRAM_H_
#define SDRAM_H_

#define SDRAM __attribute__ ((section(".sdram")))  //bez sekcji inicjalizacji zmiennych
#define SRAM __attribute__ ((section(".sram")))   //bez sekcji inicjalizacji zmiennych

void SDRAM_Init(void);
int SDRAM_Test(void);
int TestSDRAM(void);

#endif /* SDRAM_H_ */
