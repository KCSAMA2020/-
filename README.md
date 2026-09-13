# STM32 Bootloader Firmware Upgrade

基于 STM32F103 的串口 Bootloader 固件升级系统。
~~~
        PC / Python升级工具
                │
             UART串口
                │
                ▼
┌──────────────────────────┐
│      Bootloader           │
│      0x08000000           │
│                          │
│  启动检测                 │
│  协议解析                 │
│  固件接收                 │
│  Flash擦除/写入           │
│  数据校验                 │
│  CRC32校验                │
│  升级状态管理             │
│  APP合法性检查            │
│  APP跳转                  │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│          APP             │
│      0x08008000          │
│                          │
│      正常应用程序         │
│      GPIO慢闪            │
└──────────────────────────┘

        Flag区域：
        0x0800FC00

~~~

## Features

- **Bootloader / APP Flash 分区**
    - Bootloader *[0x08000000 ~ 0x08007FFF]*
    - APP *[0x08008000 ~ 0x0800FBFF]*
      
- **自定义 UART 升级协议**
    - 帧头 A5 5A
    - 命令 CMD
    - 长度 LEN
    - 数据 SEQ + DATA
    - 校验码 CHECKSUM
    - =
    - A5 5A CMD LEN SEQ+DATA CHECKSUM
      
- **START / DATA / END 升级流程**
  1. Python发握手
  2. Bootloader收到后
  3. 擦除Flag、写入UPDATE状态、擦除APP、writeAddress=0x8008000、返回ACK
  4. ...
- **ACK / NACK 数据确认**
  - Bootloader是否正确收到和处理当前操作 Y/N
- **Flash 擦除、写入与回读校验**
  - 每个数据包写入 FLASH 后都会进行回读验证
- **CRC32 固件完整性校验**
  - 新写入Flash的数据和发过来的bin固件一致
- **APP 升级状态 Flag 管理**
  - 正在升级    FLAG_UPDATE
  - APP有效     FLAG_VALID
  - 断电、异常   FLAG_UPDATE 
- **APP 向量表合法性检查**
  - 检查 MSP + Reset_Handler
- **APP CRC 二次校验**
  - APP结构正确 + 升级状态正确 + 固件CRC正确 = 成功跳转APP
- **Bootloader 自动跳转 APP**
- **Python 上位机升级工具**

## Upgrade Flow

Python → UART → Bootloader → Flash → CRC32 → Flag → APP

## Platform

- MCU: STM32F103C8T6
- IDE: Keil
- Firmware: STM32 HAL
- PC Tool: Python
