/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define UART_TRACE huart1
#define BOARD_LED0_polarity 0
#define BOARD_SPI_FLASH_8M hspi1
#define BOARD_SPI_FLASH_CSpolarity 0
#define BOARD_Xtal 25000000
#define BOARD_KEY0_polarity 0
#define BOARD_MEM_flash 512
#define BOARD_MEM_ram 128
#define BOARD_LED0_Pin GPIO_PIN_13
#define BOARD_LED0_GPIO_Port GPIOC
#define BOARD_KEY0_WKUP_Pin GPIO_PIN_0
#define BOARD_KEY0_WKUP_GPIO_Port GPIOA
#define BOARD_KEY0_WKUP_EXTI_IRQn EXTI0_IRQn
#define BOARD_SPI_FLASH_CS_Pin GPIO_PIN_4
#define BOARD_SPI_FLASH_CS_GPIO_Port GPIOA
#define BOARD_SPI_FLASH_SCK_Pin GPIO_PIN_5
#define BOARD_SPI_FLASH_SCK_GPIO_Port GPIOA
#define BOARD_SPI_FLASH_MISO_Pin GPIO_PIN_6
#define BOARD_SPI_FLASH_MISO_GPIO_Port GPIOA
#define BOARD_SPI_FLASH_MOSI_Pin GPIO_PIN_7
#define BOARD_SPI_FLASH_MOSI_GPIO_Port GPIOA
#define BOARD_BOOT1_10k_pull_down_Pin GPIO_PIN_2
#define BOARD_BOOT1_10k_pull_down_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
