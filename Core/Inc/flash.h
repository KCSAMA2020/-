#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"


HAL_StatusTypeDef Flash_Erase(uint32_t address);
HAL_StatusTypeDef Flash_Write(uint32_t address, uint16_t data);
uint16_t Flash_Read(uint32_t address);

HAL_StatusTypeDef Flash_WriteBuffer(uint32_t address, uint16_t *data, uint16_t length);
uint8_t Flash_Verify(uint32_t address, uint16_t *data, uint16_t length);

HAL_StatusTypeDef Flash_WriteBytes(
    uint32_t address,
    uint8_t *data,
    uint16_t length
);

uint8_t Flash_VerifyBytes(uint32_t address,
                          uint8_t *data,
                          uint16_t length);
						  
HAL_StatusTypeDef Flash_EraseApp(void);
uint32_t Flash_CRC32(uint32_t address, uint32_t length);
HAL_StatusTypeDef Flash_EraseAppFlag(void);


#endif
