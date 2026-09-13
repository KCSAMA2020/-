/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include "flash.h"
#include "boot.h"
#include "app_flag.h"

uint32_t startTime;

uint32_t writeAddress;
typedef void (*pFunction)(void);
void Jump_To_App(void);

uint8_t rxByte;
uint8_t rxBuffer[70];
uint8_t rxIndex = 0;
uint8_t rxLength = 0;
uint8_t rxState = 0;
uint8_t rxChecksum = 0;
uint8_t rxCmd = 0;

uint8_t ack;

uint32_t receivedSize = 0;

uint32_t receivedCRC;
uint32_t flashCRC;

volatile uint8_t updateRequest = 0;

uint32_t packetIndex = 0;

uint8_t valid;
uint8_t marked;
uint8_t crcOK;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
	AppFlag flag;
  
	HAL_Delay(100);
	uint8_t ready = 0x55;
	HAL_UART_Transmit(&huart2,&ready,1,100);
    
	App_ReadFlag(&flag);
  
  writeAddress = APP_ADDRESS_START;
  
  HAL_UART_Receive_IT(&huart2, &rxByte, 1);
  
  startTime = HAL_GetTick();
  
  
  
	while((HAL_GetTick() - startTime) < 500U)
	{
		/* 等待升级请求 */

		if(updateRequest)
		{
			break;
		}
	}
  
	if (updateRequest){
		 /*
		 * 等待 DATA 和 END
		 *
		 * START时 updateRequest = 1
		 * END成功后 updateRequest = 0
		 */
		while (updateRequest){
			HAL_Delay(1);
		}
		
		 /*
		 * 能走到这里，说明升级流程已经结束
		 */
		
	}
	
	/* 没有升级请求，或者升级已经完成 */
	/*
	 * END 命令在 UART 中断里写回了 APP 标志页；这里不能继续使用
	 * 启动时（进入 500 ms 等待窗口前）读取的旧 flag，否则会以 size=0
	 * 计算 CRC。断点仅仅是改变了该竞态的时序，并不是修复。
	 */
	App_ReadFlag(&flag);
	
	valid = App_IsValid();
	marked = App_IsMarkedValid();
	crcOK = App_CRC_Check(&flag);
	if(valid && marked && crcOK) //向量表正常 + 升级流程成功结束 + CRC正常
	{
		Jump_To_App();
	}
	else
	{
		  /*
			 * APP不存在或者升级失败
			 * 留在Bootloader
		 */
		while (1){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
			HAL_Delay(200);
		}
	}
	
	
	
//             上电
//              ↓
//          Bootloader
//              ↓
//          等待 0.5 秒
//              ↓
//      ┌───────┴────────┐
//      │                │
//  收到START          没收到
//      │                │
//      ↓                ↓
//   升级模式       检查App有效性
//                       │
//                  ┌────┴────┐
//                  │         │
//                 有效       无效
//                  │         │
//                  ↓         ↓
//             Jump_To_App   Bootloader
//                  │
//                  ↓
//                 慢闪
	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
typedef void (*pFunction)(void);
void Jump_To_App(void){
	uint32_t appStack;
	uint32_t appResetHandler;
	pFunction JumpToApplication;
	
	appStack = *(__IO uint32_t *)APP_ADDRESS_START;
	appResetHandler = *(__IO uint32_t *)(APP_ADDRESS_START + 4U);
	__disable_irq();
	HAL_UART_AbortReceive(&huart2);
    HAL_UART_DeInit(&huart2);
	
	SysTick->CTRL=0;
	SysTick->LOAD=0;
	SysTick->VAL=0;
	SCB->VTOR=APP_ADDRESS_START;
	JumpToApplication = (pFunction)appResetHandler;
	__set_MSP(appStack);
	JumpToApplication();
	while(1){
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2)
    {
		
      
        switch(rxState)
        {
            case 0:

                if(rxByte == 0xA5)
                {
                    rxState = 1;
                }

                break;


            case 1:

                if(rxByte == 0x5A)
                {
                    rxState = 2;
                }
                else
                {
                    rxState = 0;
                }

                break;


            case 2:

                rxCmd = rxByte;

                rxState = 3;

                break;


            case 3:

                rxLength = rxByte;

                if(rxLength > 65)
                {
                    rxState = 0;
                    break;
                }

                rxIndex = 0;
                rxChecksum = 0;

				if (rxLength == 0){
					rxState = 5;
				}
				
				else {
					rxState = 4;
				}

                break;


            case 4:

                rxBuffer[rxIndex] = rxByte;

                rxChecksum += rxByte;

                rxIndex++;

                if(rxIndex >= rxLength)
                {
                    rxState = 5;
                }

                break;


            case 5:

                if(rxByte == rxChecksum)
                {
                    /*
                     * 一帧数据接收成功
                     */

                    if(rxCmd == CMD_START_UPDATE)
                    {
                        // 开始升级
						
						if(Flash_EraseAppFlag() == HAL_OK){
							AppFlag flag;
							
							//1. 标记：正在升级
						
							flag.magic = FLAG_UPDATE;
							flag.version = 0;
							flag.size = 0;
							flag.crc = 0;
							
							if(App_WriteFlag(&flag) == HAL_OK){
								ack = ACK;
							}
						}		
						
						//2. 擦除APP区域
						
						if(Flash_EraseApp() == HAL_OK)
						{
							writeAddress = APP_ADDRESS_START;

							receivedSize = 0;
							packetIndex = 0;

							updateRequest = 1;

							ack = ACK;
						}
						else
						{
							ack = NACK;
						}
						HAL_UART_Transmit(&huart2, &ack, 1, 100);
                    }

                    else if(rxCmd == CMD_DATA)
                    {
                        // 固件数据
						uint32_t writeLength;
						uint8_t seq;
						uint16_t dataLength;
						
						// 第0字节是包序号
						seq = rxBuffer[0];
						
						// 检查包序号
						if (seq != packetIndex){
							ack = NACK;
							HAL_UART_Transmit(&huart2, &ack, 1, 100);
							rxState = 0;
							HAL_UART_Receive_IT(&huart2,&rxByte,1);
							return ;
						}
						
						// 实际数据长度少1
						dataLength = rxLength - 1;
									
						/* 计算实际需要写入的 Flash 字节数
							奇数长度需要补 1 个 0xFF */
						writeLength = (dataLength + 1U) & ~1U;
						
						/* 检查是否超出 App 区域 */
						if (writeAddress + writeLength <= APP_ADDRESS_END + 1U){
							
							/* 写入 Flash */
							if (Flash_WriteBytes(writeAddress, &rxBuffer[1], dataLength) == HAL_OK){
								if (Flash_VerifyBytes(writeAddress, &rxBuffer[1], dataLength)){
									/* Flash 是半字编程，所以地址按 2 字节对齐增加 */
									writeAddress += writeLength;
									receivedSize += dataLength;
									
									packetIndex ++;
									
									ack = ACK;
									HAL_UART_Transmit(&huart2, &ack, 1, 100);
								}
								else{
									ack = NACK;
									HAL_UART_Transmit(&huart2, &ack, 1, 100);
								}
							}
							else {
								ack = NACK;
								HAL_UART_Transmit(&huart2, &ack, 1, 100);
							}
						}
						
                    }

					else if(rxCmd == CMD_END_UPDATE)
					{
						// 升级结束
				
						// Flash_EraseAppFlag();
						/*
						 * 旧代码在校验 END 包长度和 CRC 之前擦除标志页。
						 * 任意畸形/CRC 错误的 END 包都会把 0x0800FC00 变成 0xFFFFFFFF。
						 * 仅在整包校验成功、即将写入 FLAG_VALID 时才擦除该页。
						 */
						
						if(rxLength != 4){
							ack = NACK;
							HAL_UART_Transmit(&huart2, &ack, 1, 100);
							break;
						}
						
						receivedCRC = ((uint32_t)rxBuffer[0]) |
									  ((uint32_t)rxBuffer[1] << 8) |
									  ((uint32_t)rxBuffer[2] << 16) |
									  ((uint32_t)rxBuffer[3] << 24);
						
						flashCRC = Flash_CRC32(APP_ADDRESS_START, receivedSize);
						
						if(flashCRC == receivedCRC)
						{
							
							AppFlag flag;

							if (Flash_EraseAppFlag() == HAL_OK)
							{
								flag.magic = FLAG_VALID;
								flag.version = 0x00010000;
								flag.size = receivedSize;
								flag.crc = flashCRC;

								if (App_WriteFlag(&flag) == HAL_OK){
									ack = ACK;
									updateRequest = 0;
								}
								else {
									ack = NACK;
								}
							}
							else {
								ack = NACK;
							}
						}
						else
						{
							ack = NACK;
						}
						
						HAL_UART_Transmit(&huart2, &ack, 1, 100);
                    }
                }
				
				else {
					ack = NACK;
					HAL_UART_Transmit(&huart2, &ack, 1, 100);
				}

                rxState = 0;

                break;
        }

        HAL_UART_Receive_IT(&huart2, &rxByte, 1);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
