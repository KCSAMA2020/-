#include <flash.h>

HAL_StatusTypeDef Flash_Erase(uint32_t address){
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError = 0;
	HAL_StatusTypeDef status;
	
	HAL_FLASH_Unlock();
	
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = address;
	EraseInitStruct.NbPages = 1;
	
	status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	
	HAL_FLASH_Lock();
	
	return status;
}
//==============================
HAL_StatusTypeDef Flash_Write(uint32_t address, uint16_t data){
	HAL_StatusTypeDef status;
	
	HAL_FLASH_Unlock();
	
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data);
	
	HAL_FLASH_Lock();
	
	return status;
}
//==============================
uint16_t Flash_Read(uint32_t address){
	return *(__IO uint16_t *)address;
}
//==============================
HAL_StatusTypeDef Flash_WriteBuffer(uint32_t address, uint16_t *data, uint16_t length){
	HAL_StatusTypeDef status;

	HAL_FLASH_Unlock();
	
	for (uint16_t i = 0; i < length; i ++){
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + i * 2, data[i]);
		if (status != HAL_OK){
			return status;
		}
	}
	
	HAL_FLASH_Lock();
	
	return HAL_OK;
}
//==============================
uint8_t Flash_Verify(uint32_t address, uint16_t *data, uint16_t length){
	for (uint16_t i = 0; i < length; i ++){
		if (Flash_Read(address) != data[i]){
			return 0;
		}
		address += 2;
	}
	return 1;
}
//==============================
uint16_t Make_HalfWord(uint8_t low, uint8_t high){
	return ((uint16_t)high << 8) | low;
}
//==============================
HAL_StatusTypeDef Flash_WriteBytes(
    uint32_t address,
    uint8_t *data,
    uint16_t length
)
{
	HAL_StatusTypeDef status;
	uint16_t halfWord;
	
	HAL_FLASH_Unlock();
	
	for (uint16_t i = 0; i < length; i += 2){
		
		if (i + 1 < length){
			halfWord = ((uint16_t)data[i + 1] << 8) | data[i];
		}
		else {
			halfWord = 0xFF00 | data[i];
		}
		
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, halfWord);
		
		if (status != HAL_OK){
			HAL_FLASH_Lock();
			return status;
		}
		address += 2;
	}
	HAL_FLASH_Lock();
	
	return HAL_OK;
}
//==============================
uint8_t Flash_VerifyBytes(uint32_t address,
                          uint8_t *data,
                          uint16_t length)
{
    for(uint16_t i = 0; i < length; i++)
    {
        if(*(__IO uint8_t *)address != data[i])
        {
            return 0;
        }

        address += 1;
    }

    return 1;
}
//==============================
HAL_StatusTypeDef Flash_EraseApp(void){
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError = 0;
	HAL_StatusTypeDef status;
	
	HAL_FLASH_Unlock();
	
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;;
	EraseInitStruct.PageAddress = APP_ADDRESS_START;
	// EraseInitStruct.NbPages = 31;
	/* 0x08008000..0x0800FBFF 恰好为 31 个 1 KiB 页；不擦除 0x0800FC00 标志页。 */
	EraseInitStruct.NbPages = (APP_ADDRESS_END - APP_ADDRESS_START + 1U) / FLASH_PAGE_SIZE;
	
	status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	
	HAL_FLASH_Lock();
	
	return status;
}
//==============================
uint32_t Flash_CRC32(uint32_t address, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFFU;
    uint8_t data;

    for(uint32_t i = 0; i < length; i++)
    {
        data = *(__IO uint8_t *)address;

        crc ^= data;

        for(uint8_t j = 0; j < 8; j++)
        {
            if(crc & 1U)
            {
                crc = (crc >> 1) ^ 0xEDB88320U;
            }
            else
            {
                crc >>= 1;
            }
        }

        address++;
    }

    return ~crc;
}
//==============================

HAL_StatusTypeDef Flash_EraseAppFlag(void)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    HAL_StatusTypeDef status;

    HAL_FLASH_Unlock();

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = APP_FLAG_ADDRESS;
    EraseInitStruct.NbPages = 1;

    status = HAL_FLASHEx_Erase(
        &EraseInitStruct,
        &PageError
    );

    HAL_FLASH_Lock();

    return status;
}

