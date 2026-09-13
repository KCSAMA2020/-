#include "boot.h"
#include "app_flag.h"
#include "flash.h"

uint8_t App_IsValid(void)
{
    uint32_t appStack;
    uint32_t appResetHandler;

    appStack = *(__IO uint32_t *)APP_ADDRESS_START;

    appResetHandler =
        *(__IO uint32_t *)(APP_ADDRESS_START + 4U);

    appResetHandler &= ~1U;

    if((appStack >= 0x20000000U) &&
       (appStack <= 0x20004FFFU) &&
       (appResetHandler >= APP_ADDRESS_START) &&
       (appResetHandler <= APP_ADDRESS_END))
    {
        return 1;
    }

    return 0;
}

uint8_t App_IsMarkedValid(void)
{
    AppFlag flag;

	App_ReadFlag(&flag);
	
    if(flag.magic == FLAG_VALID)
    {
        return 1;
    }

    return 0;
}

