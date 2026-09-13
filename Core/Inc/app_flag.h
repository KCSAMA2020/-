#ifndef __APP_FLAG_H
#define __APP_FLAG_H

#include "main.h"



#define FLAG_UPDATE       0x5555AAAA  //升级中断
#define FLAG_VALID        0xA55AA55A  //APP有效
#define FLAG_EMPTY        0xFFFFFFFF  //没有升级过

typedef struct
{
    uint32_t magic;       //状态标志

    uint32_t version;     //版本号

    uint32_t size;        //APP大小

    uint32_t crc;         //CRC


}AppFlag;

void App_ReadFlag(AppFlag *flag);
HAL_StatusTypeDef App_WriteFlag(AppFlag *flag);
uint8_t App_CRC_Check(AppFlag *flag);

#endif
