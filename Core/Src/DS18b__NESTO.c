

#include  "main.h"
///////////////////////////////
/// DS18b20
///////////////////////////////
int isRxed = 0;
uint8_t RxData[8];
uint8_t Temp_LSB = 0;
uint8_t Temp_MSB = 0;

void uart_Init(uint32_t baud) {
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_HalfDuplex_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
}

int DS18B20_Init(void) {
	uint8_t data = 0xf0;
	uart_Init(9600);
	HAL_UART_Transmit(&huart1, &data, 1, 100);
	if (HAL_UART_Receive(&huart1, &data, 1, 100) != HAL_OK) {
		return -1;
	}
	uart_Init(115200);
	if (data == 0xf0) {
		return -2;
	}
	return 1;
}

void DS18B20_Write(uint8_t data) {
	uint8_t buf[8];
	for (int i = 0; i < 8; i++) {
		if ((data & (1 << i)) != 0) {
			buf[i] = 0xff; // send 1 to sensor
		} else {
			buf[i] = 0x00; // send 0 to sensor
		}
	}
	HAL_UART_Transmit(&huart1, buf, 8, 1000);
}

uint8_t DS18B20_Read(void) {
	uint8_t buf[8];
	uint8_t val = 0;
	for (int i = 0; i < 8; ++i) {
		buf[i] = 0xff;
	}
	HAL_UART_Transmit_DMA(&huart1, buf, 8);
	HAL_UART_Receive_DMA(&huart1, RxData, 8);

	while (isRxed == 0) {
		// cekam
	}
	for (int i = 0; i < 8; ++i) {
		if (RxData[i] == 0xff) {
			// bit is 1
			val |= 1 << i; // write 1 to that position
			// } else {
			// 	val |= 0<<i;
		}
		isRxed = 0;
		return val;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	isRxed = 1;
}







/* USER CODE END 0 */
/* USER CODE BEGIN WHILE */

	uint16_t rambk[32000];
	uint32_t rambk_pos = 0;

	int ds18Presence = 0;

	char krecem[64] = "mmYelou!\n";
	char tuje[64] = "tu je!\n";
	char nijetu[64] = "nije\n";

	while (1) {
		HAL_GPIO_WritePin(BOARD_LED0_GPIO_Port, BOARD_LED0_Pin, GPIO_PIN_RESET);
		HAL_Delay(5);
		HAL_GPIO_WritePin(BOARD_LED0_GPIO_Port, BOARD_LED0_Pin, GPIO_PIN_SET);
		HAL_Delay(95);

		HAL_UART_Transmit(&UART_TRACE, (uint8_t *) krecem, sizeof(krecem), 200);
		HAL_Delay(100);

		// CHECK FOR DS18 PRESENCE
		ds18Presence = DS18B20_Init();
		if (ds18Presence > 0) {
			HAL_UART_Transmit(&UART_TRACE, (uint8_t *) tuje, sizeof(tuje), 200);

			HAL_Delay(1);
			DS18B20_Write(0xCC); // skip ROM, sta god da je
			DS18B20_Write(0x44); // convert T

			ds18Presence = DS18B20_Init();
			DS18B20_Write(0xCC); // skip ROM, sta god da je
			DS18B20_Write(0xBE); // read scratchpad

			Temp_LSB = DS18B20_Read();
			Temp_MSB = DS18B20_Read();
		} else {
			// SENSOR NOT PRESENT, SIGNAL ERROR
			HAL_UART_Transmit(&UART_TRACE, (uint8_t *) nijetu, sizeof(nijetu), 200);
			// TODO halt, panic
		}


		// else goto sleep, stop, standby

		// waking up:
		// read DS18 sensor
		// write DS18 data to SRAM next position
		// increase last written position
		// if filled SRAM, flush to WQ
		// increase WQ position
		// loop to sleep


		if (ds18Presence > 0) {
			HAL_UART_Transmit(&UART_TRACE, (uint8_t *) tuje, sizeof(tuje), 200);

			HAL_Delay(1);
			DS18B20_Write(0xCC); // skip ROM, sta god da je
			DS18B20_Write(0x44); // convert T

			ds18Presence = DS18B20_Init();
			DS18B20_Write(0xCC); // skip ROM, sta god da je
			DS18B20_Write(0xBE); // read scratchpad

			Temp_LSB = DS18B20_Read();
			Temp_MSB = DS18B20_Read();
		} else {
			// signal error
			HAL_UART_Transmit(&UART_TRACE, (uint8_t *) nijetu, sizeof(nijetu), 200);
		}
		RTC_CalendarShow();
		HAL_Delay(5000);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}
