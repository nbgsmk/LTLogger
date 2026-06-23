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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <ctype.h>

#include "Led.hpp"
// #include "DataLogger.h"
#include "DLogger.hpp"
#include "W25Qxx.h"

#include "usbd_cdc_if.h"
#include "UsbSerial.hpp"
#include "ZRT.hpp"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_RAM_BUFFER_KB 64
#define RAMBLOCK_CNT_bkpreg    RTC_BKP_DR5

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

// Static RAM buffer for data logger, aligned to 4 bytes for Cortex-M4 (stm32f4)
static uint8_t ramBuffer[1024 * MAX_RAM_BUFFER_KB] __attribute__((aligned(4)));
Logging::DLogger dLogger(ramBuffer, MAX_RAM_BUFFER_KB);
Led boardLed(BOARD_LED0_GPIO_Port, BOARD_LED0_Pin, LedPolarity::ActLO);
// Led& get_board_led() {
// 	// This constructor runs safely EXACTLY when this function is called for the first time
// 	static Led boardLed(BOARD_LED0_GPIO_Port, BOARD_LED0_Pin, LedPolarity::ActLO);
// 	return boardLed;
// }
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_RTC_Init(void);

static void MX_USART1_UART_Init(void);

static void MX_CRC_Init(void);

static void MX_SPI1_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile bool flegLogDumpRequested = false;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	GPIO_PinState prikit = HAL_GPIO_ReadPin(BOARD_KEY0_WKUP_GPIO_Port, BOARD_KEY0_WKUP_Pin);
	if (prikit == BOARD_KEY0_polarity) {
		flegLogDumpRequested = true;
	}
}

void dampujAkoJePrikit() {
	extern USBD_HandleTypeDef hUsbDeviceFS;
	USBD_CDC_HandleTypeDef *hcdc = nullptr;
	if (hUsbDeviceFS.pClassData != nullptr) {
		hcdc = (USBD_CDC_HandleTypeDef *) hUsbDeviceFS.pClassData;
	}
	if (flegLogDumpRequested == true) {

		char buff[200] = {0};
		std::string linija;
		UsbSerial::transmitNR();
		for (uint32_t i = 0; i < dLogger.getNextPos(); ++i) {

			linija = dLogger.getRecordStr(i);
			if (linija.empty()) {
				break;
			}
			snprintf(buff, 200, "%s", linija.c_str());
			uint32_t abortDelay = 2000;
			uint8_t usbStat = USBD_BUSY;
			while (abortDelay > 0) {
				usbStat = CDC_Transmit_FS((uint8_t *) buff, strlen(buff));
				if (USBD_BUSY == usbStat) {
					abortDelay--;
					HAL_Delay(2); // sacekaj da USBD bude spreman
				} else {
					HAL_Delay(5); // throttle a little bit
					break;
				}
			}

			// CRITICAL SAFETY NET: If the request was successful, wait for the
			// physical USB hardware to finish broadcasting the bytes over the wire
			// BEFORE allowing the for-loop to run again and overwrite "buf".
			if (usbStat == USBD_OK && hcdc != nullptr) {
				uint32_t txSafetyTimeout = 2000;
				while (hcdc->TxState != 0) {
					HAL_Delay(1); // wait for physical USB hardware to finish broadcasting
					if (--txSafetyTimeout == 0) {
						break; // Stop waiting if the host computer froze or unplugged
					}
				}
			}

		}

		// novi red na kraju celog dumpa
		CDC_Transmit_FS((uint8_t *)"\n\r", strlen("\n\r"));
		HAL_Delay(200); // some time to finish
		flegLogDumpRequested = false;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
	/* USER CODE BEGIN 1 */
	extern void (*__init_array_start []) (void) __attribute__((weak));
	extern void (*__init_array_end []) (void) __attribute__((weak));

	int count = __init_array_end - __init_array_start;
	for (int i = 0; i < count; i++) {
		__init_array_start[i]();
	}

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
	MX_RTC_Init();
	MX_USART1_UART_Init();
	MX_USB_DEVICE_Init();
	MX_CRC_Init();
	MX_SPI1_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	boardLed.blink(50, 50);
	boardLed.blink(50, 50);

	// W25Q_Reset();
	// uint32_t manuf = W25Q_ManufacturerDeviceID();
	// uint32_t jedid = W25Q_ReadID();
	// uint32_t uniq = W25Q_UniqueID();

	constexpr uint32_t flashPagesPerRamBlock = ( (1024*MAX_RAM_BUFFER_KB) / W25Q_WRITE_PAGE_SIZE );
	constexpr uint32_t ramBlocksPerW25Qflash = ( W25Q_FLASH_SIZE / (1024*MAX_RAM_BUFFER_KB) );

	uint32_t currentRamBlk = 	ZRT::GetBKPreg(RAMBLOCK_CNT_bkpreg);
	if (currentRamBlk >= ramBlocksPerW25Qflash) {
		currentRamBlk = 0;
		ZRT::SetBKPreg(RAMBLOCK_CNT_bkpreg, 0);
	}

	uint16_t fakeSensors[3] = {0};
	uint16_t idxx = 0;
	while (1) {
		boardLed.blink(10);

		RTC_TimeTypeDef sT = {0};
		RTC_DateTypeDef sD = {0};
		ZRT::GetTimeDate(&sT, &sD);

		fakeSensors[0] = idxx++;
		fakeSensors[1] = idxx++;
		fakeSensors[2] = idxx++;
		Logging::SingleLogRecord_t logr;
		logr.rtcYear = sD.Year;
		logr.rtcMonth = sD.Month;
		logr.rtcDate = sD.Date;
		logr.rtcHour = sT.Hours;
		logr.rtcMinute = sT.Minutes;
		logr.rtcSecond = sT.Seconds;
		logr.rtcWeekday = sD.WeekDay;
		logr.sensors[0] = fakeSensors[0];
		logr.sensors[1] = fakeSensors[1];
		logr.sensors[2] = fakeSensors[2];
		dLogger.append(logr);


		if (dLogger.isFull()) {
			uint32_t flashPageAddr = currentRamBlk * flashPagesPerRamBlock;
			// W25Q_Write_AppendOnly(flashPageAddr, 0, sizeof(ramBuffer),  (uint8_t*)&ramBuffer);
			currentRamBlk++;
			ZRT::SetBKPreg(RAMBLOCK_CNT_bkpreg, currentRamBlk);
			dLogger.flush();
		}
		if (currentRamBlk >= ramBlocksPerW25Qflash) {
			currentRamBlk = 0;
			ZRT::SetBKPreg(RAMBLOCK_CNT_bkpreg, currentRamBlk);
			dLogger.reset();
		}


		HAL_Delay(3000);

		if (flegLogDumpRequested == true) {
			std::string ponovo = "press again to dump the buffer once more";
			std::string obrisi = "press and hold 5 seconds to erase flash";
			std::string tot = "total entries: ";
			for ( ; ; ) {
				boardLed.blink(50, 50);		// rapid blink -> waiting for user input
				if (flegLogDumpRequested == true) {
					dampujAkoJePrikit();
					UsbSerial::transmitLine(tot + std::to_string(dLogger.getNextPos()));
					UsbSerial::transmitLine(ponovo);
					UsbSerial::transmitLine(obrisi);
				}
			}

			// if (trigger_erase == true) {
			// 	W25Q_Reset();
			// 	W25Q_Chip_Erase();
			// 	DataLogger_Init();
			// 	ramBlockCnt = 0;
			// 	zRTC_Set_BKPreg(RAMBLOCK_CNT_bkpreg, ramBlockCnt);
			// 	trigger_erase = false;
			// }
		} else {
			char bfr[100] = {0}; // Buffer to store the formatted text string
			ZRT::GetTimeDateString(bfr, sizeof(bfr));
			std::string x = std::to_string(dLogger.getNextPos());
			UsbSerial::transmit(std::string(bfr) + " " + x + "\n\r");
		}


		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 192;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
								| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}

	/** Enables the Clock Security System
	*/
	HAL_RCC_EnableCSS();
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void) {
	/* USER CODE BEGIN CRC_Init 0 */

	/* USER CODE END CRC_Init 0 */

	/* USER CODE BEGIN CRC_Init 1 */

	/* USER CODE END CRC_Init 1 */
	hcrc.Instance = CRC;
	if (HAL_CRC_Init(&hcrc) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN CRC_Init 2 */

	/* USER CODE END CRC_Init 2 */
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void) {
	/* USER CODE BEGIN RTC_Init 0 */

	/* USER CODE END RTC_Init 0 */

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	/* USER CODE BEGIN RTC_Init 1 */

	/* USER CODE END RTC_Init 1 */

	/** Initialize RTC Only
	*/
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK) {
		Error_Handler();
	}

	/* USER CODE BEGIN Check_RTC_BKUP */

	uint16_t MAGIC_NUM = 0x1177;
#define MAGIC_REG RTC_BKP_DR2
	uint32_t prev = HAL_RTCEx_BKUPRead(&hrtc, MAGIC_REG); // ako nadjes magic number, RTC je vec konfigurisan
	GPIO_PinState btn = HAL_GPIO_ReadPin(BOARD_KEY0_WKUP_GPIO_Port, BOARD_KEY0_WKUP_Pin); // ako je pritisnuto dugme

	if (prev == MAGIC_NUM) {
		if (btn != BOARD_KEY0_polarity) {
			// "!= active_polarity" tj nije aktivno tj "nije pritisnuto"
			return; // vec postoji magic number a nije pritisnuto "override" dugme
		}
	}


	/* USER CODE END Check_RTC_BKUP */

	/** Initialize RTC and set the Time and Date
	*/
	sTime.Hours = 0x23;
	sTime.Minutes = 0x48;
	sTime.Seconds = 0x5;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler();
	}
	sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
	sDate.Month = RTC_MONTH_JUNE;
	sDate.Date = 0x16;
	sDate.Year = 0x26;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN RTC_Init 2 */

	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc, MAGIC_REG, MAGIC_NUM); // zabelezi da je konfigurisano pre nego sto zaboravis
	HAL_PWR_DisableBkUpAccess();

	/* USER CODE END RTC_Init 2 */
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void) {
	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void) {
	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(BOARD_LED0_GPIO_Port, BOARD_LED0_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(BOARD_SPI_FLASH_CS_GPIO_Port, BOARD_SPI_FLASH_CS_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin : BOARD_LED0_Pin */
	GPIO_InitStruct.Pin = BOARD_LED0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BOARD_LED0_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : BOARD_KEY0_WKUP_Pin */
	GPIO_InitStruct.Pin = BOARD_KEY0_WKUP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(BOARD_KEY0_WKUP_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : BOARD_SPI_FLASH_CS_Pin */
	GPIO_InitStruct.Pin = BOARD_SPI_FLASH_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(BOARD_SPI_FLASH_CS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : BOARD_BOOT1_10k_pull_down_Pin */
	GPIO_InitStruct.Pin = BOARD_BOOT1_10k_pull_down_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BOARD_BOOT1_10k_pull_down_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM11 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM11) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
void assert_failed(uint8_t *file, uint32_t line) {
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
