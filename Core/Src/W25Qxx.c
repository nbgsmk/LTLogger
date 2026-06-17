/*
 * W25Qxx.c
 *
 *  Created on: Jul 15, 2023
 *      Author: controllerstech
 */


#include "main.h"
#include "W25Qxx.h"

extern SPI_HandleTypeDef hspi1;
#define W25Q_SPI hspi1

#define numBLOCK 32  // number of total blocks for 16Mb flash

void W25Q_Delay(uint32_t time) {
	HAL_Delay(time);
}

void csLOW(void) {
	HAL_GPIO_WritePin(BOARD_FLASH_CS_GPIO_Port, BOARD_FLASH_CS_Pin, GPIO_PIN_RESET);
}

void csHIGH(void) {
	HAL_GPIO_WritePin(BOARD_FLASH_CS_GPIO_Port, BOARD_FLASH_CS_Pin, GPIO_PIN_SET);
}

void SPI_Write(uint8_t *data, uint8_t len) {
	HAL_SPI_Transmit(&W25Q_SPI, data, len, 2000);
}

void SPI_Read(uint8_t *data, uint8_t len) {
	HAL_SPI_Receive(&W25Q_SPI, data, len, 5000);
}

/**************************************************************************************************/

void W25Q_Reset(void) {
	uint8_t tData[2];
	tData[0] = 0x66; // enable Reset
	tData[1] = 0x99; // Reset
	csLOW();
	SPI_Write(tData, 2);
	csHIGH();
	W25Q_Delay(100);
}

uint32_t W25Q_ReadID(void) {
	uint8_t tData = 0x9F; // Read JEDEC ID
	uint8_t rData[3];
	csLOW();
	SPI_Write(&tData, 1);
	SPI_Read(rData, 3);
	csHIGH();
	return ((rData[0] << 16) | (rData[1] << 8) | rData[2]);
}
