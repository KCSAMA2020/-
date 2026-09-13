#include "app_flag.h"
#include "flash.h"

void App_ReadFlag(AppFlag *flag){
	
	// uint32_t *addr;
	__IO uint32_t *addr;
	
	addr = (uint32_t *)APP_FLAG_ADDRESS;
	
	flag -> magic = addr[0];
	flag -> version = addr[1];
	flag -> size = addr[2];
	flag -> crc = addr[3];
}

HAL_StatusTypeDef App_WriteFlag(AppFlag *flag)
{
    HAL_FLASH_Unlock();
	
	uint32_t *data;
	
	data = (uint32_t *)flag;
	
	for(int i = 0; i < 4; i ++)
    {

		uint32_t address = APP_FLAG_ADDRESS + i*4;
		
		uint16_t low = data[i] & 0xFFFF;
		
		uint16_t high = data[i] >> 16;
		
        if(HAL_FLASH_Program(
            FLASH_TYPEPROGRAM_HALFWORD,
            address,
            low
        ) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return HAL_ERROR;
        }

		if(HAL_FLASH_Program(
            FLASH_TYPEPROGRAM_HALFWORD,
            address+2,
            high
        ) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return HAL_ERROR;
        }
		
    }
	
	HAL_FLASH_Lock();
	
	return HAL_OK;
}

uint8_t App_CRC_Check(AppFlag *flag){
	uint32_t crc;
	
	/* 不允许空/越界标志参与 CRC，避免读出 App 区和标志页之外的数据。 */
	if ((flag->magic != FLAG_VALID) ||
		(flag->size == 0U) ||
		(flag->size > (APP_ADDRESS_END - APP_ADDRESS_START + 1U)))
	{
		return 0;
	}

	// crc = Flash_CRC32(APP_ADDRESS_START, flag -> size);
	crc = Flash_CRC32(APP_ADDRESS_START, flag->size);
	
	if (crc == flag -> crc){
		return 1;
	}
	
	return 0;
}
