/*
 * W25Qxx.h
 *
 *  Created on: Jul 15, 2023
 *      Author: controllerstech
 */

#ifndef INC_W25QXX_H_
#define INC_W25QXX_H_

#include "main.h"

#define W25Q_FLASH_SIZE					0x800000	/* 64Mbit => 8Mbyte */
#define W25Q_ERASE_BLOCK_SIZE			0x10000		/* 1 block = 64KBytes */
#define W25Q_ERASE_SECTOR_SIZE			0x1000		/* 1 sector = 4kBytes */
#define W25Q_WRITE_PAGE_SIZE			256			/* 1 page = 256 bytes */

#define W25Q_WRITE_ENABLE				0x06
#define W25Q_VOLATILE_SR_WRITE_ENABLE	0x06
#define W25Q_WRITE_DISABLE				0x04

#define W25Q_POWERUP_ID					0xAB
#define W25Q_MANUFACTURER_DEVICE_ID		0x90
#define W25Q_JEDEC_ID					0x9F
#define W25Q_UNIQUE_ID					0x4B

#define W25Q_READ_DATA					0x03
#define W25Q_FAST_READ					0x0B
#define W25Q_PAGE_PROGRAM				0x02

#define W25Q_4KB_SECTOR_ERASE 			0x20
#define W25Q_32KB_BLOCK_ERASE 			0x52
#define W25Q_64KB_BLOCK_ERASE 			0xD8
#define W25Q_CHIP_ERASE					0xC7

#define W25Q_READ_SR1					0x05
#define W25Q_WRITE_SR1					0x01
#define W25Q_READ_SR2					0x35
#define W25Q_WRITE_SR2					0x31
#define W25Q_READ_SR3					0x15
#define W25Q_WRITE_SR3					0x11

#define W25Q_GLOBAL_BLOCK_LOCK			0x7E
#define W25Q_GLOBAL_BLOCK_UNLOCK		0x98
#define W25Q_READ_BLOCK_LOCK			0x3D
#define W25Q_INDIVIDUAL_BLOCK_LOCK		0x36
#define W25Q_INDIVIDUAL_BLOCK_UNLOCK	0x39
#define W25Q_ERASE_PROGRAM_SUSPEND		0x75
#define W25Q_ERASE_PROGRAM_RESUME		0x7A
#define W25Q_POWERDOWN					0xB9
#define W25Q_ENABLE_RESET				0x66	//sequence is 0x66 + 0x99 + 30us delay
#define W25Q_RESET						0x99	//sequence is 0x66 + 0x99 + 30us delay
#define W25Q_EXIT_QPI_MODE				0xFF

#define W25Q_READ_DATA_4B				0x13
#define W25Q_FAST_READ_4B				0x0C
#define W25Q_PAGE_PROGRAM_4B			0x12
#define W25Q_SECTOR_ERASE_4B			0x21
#define W25Q_64KB_BLOCK_ERASE_4B		0xDC


void W25Q_Reset(void);
void W25Q_Chip_Erase(void);

uint32_t W25Q_ManufacturerDeviceID(void);
uint32_t W25Q_ReadID(void);
uint32_t W25Q_UniqueID(void);

void W25Q_Read(uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);
void W25Q_FastRead(uint32_t startPage, uint8_t offset, uint32_t size, uint8_t *rData);

void W25Q_Erase_Sector(uint16_t numsector);

void W25Q_Write_AppendOnly(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data);
void W25Q_Write_Clean(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data);
void W25Q_Write(uint32_t page, uint16_t offset, uint32_t size, uint8_t *data);
void W25Q_Write_Byte(uint32_t Addr, uint8_t data);

uint8_t W25Q_Read_Byte(uint32_t Addr);
float W25Q_Read_NUM(uint32_t page, uint16_t offset);
void W25Q_Write_NUM(uint32_t page, uint16_t offset, float data);

void W25Q_Read_32B(uint32_t page, uint16_t offset, uint32_t size, uint32_t *data);
void W25Q_Write_32B(uint32_t page, uint16_t offset, uint32_t size, uint32_t *data);

void flash_WriteMemory(uint8_t *buffer, uint32_t address, uint32_t buffer_size);
void flash_ReadMemory(uint32_t Addr, uint32_t Size, uint8_t *buffer);

void flash_SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
void flash_ChipErase(void);
void flash_Reset(void);


#endif /* INC_W25QXX_H_ */
