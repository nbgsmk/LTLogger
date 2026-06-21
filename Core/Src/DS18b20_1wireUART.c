//
// Created by mcu on 29/05/2026.
//

#include "main.h"
#include "DS18b20_1wireUART.h"

#include <stdio.h>
#include <string.h>


/////////////////////////////////
/////////////////////////////////
/// 1-wire UART
/////////////////////////////////
/////////////////////////////////


UART_HandleTypeDef ds18Uart;

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

float Humidity = 0;

uint8_t ADC_int_flag = 0;
uint16_t AD_RES[2];
float V_Sense, V_Ref;
char TxBuffer[200];


// int Presence = 0;
int isRxed = 0;
uint8_t RxData[8];
int16_t Temp;

void uart_Init(UART_HandleTypeDef ds18Uart, uint32_t baud) {
	ds18Uart.Instance = USART2;
	ds18Uart.Init.BaudRate = baud;
	ds18Uart.Init.WordLength = UART_WORDLENGTH_8B;
	ds18Uart.Init.StopBits = UART_STOPBITS_1;
	ds18Uart.Init.Parity = UART_PARITY_NONE;
	ds18Uart.Init.Mode = UART_MODE_TX_RX;
	ds18Uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	ds18Uart.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_HalfDuplex_Init(&ds18Uart) != HAL_OK) {
		Error_Handler();
	}
}

int DS18B20_UART_Start(void) {
	uint8_t data = 0xF0;
	uart_Init(ds18Uart,9600);
	HAL_UART_Transmit(&ds18Uart, &data, 1, 100);
	if (HAL_UART_Receive(&ds18Uart, &data, 1, 1000) != HAL_OK) return -1; // failed.. check connection
	uart_Init(ds18Uart, 115200);
	if (data == 0xf0) return -2; // error no device detected
	return 1;
}

void DS18B20_UART_Write(uint8_t data) {
	uint8_t buffer[8];
	for (int i = 0; i < 8; i++) {
		if ((data & (1 << i)) != 0) {
			buffer[i] = 0xFF; // if the bit is HIGH, send 1
		} else {
			buffer[i] = 0x00; // else send 0
		}
	}
	HAL_UART_Transmit(&ds18Uart, buffer, 8, 1000);
}

uint8_t DS18B20_UART_Read(void) {
	uint8_t buffer[8];
	uint8_t value = 0;
	for (int i = 0; i < 8; i++) {
		buffer[i] = 0xFF;
	}
	HAL_UART_Transmit_DMA(&ds18Uart, buffer, 8);
	HAL_UART_Receive_DMA(&ds18Uart, RxData, 8);

	while (isRxed == 0);

	for (int i = 0; i < 8; i++) {
		if (RxData[i] == 0xFF) {
			// if bit is 1
			value |= 1 << i; // write a 1 to that position
		}
	}
	isRxed = 0;
	return value;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	isRxed = 1;
}




void calcAndSend(uint32_t adc_value) {
	// STM32f411ceu
	float const AVG_SLOPE = 2.5f;
	float const V_AT_25C = 0.76f;
	float const V_REF_INT = 1.21f;
	// Calculate The Temperature
	V_Ref = (float) ((V_REF_INT * 4095.0) / AD_RES[0]);
	V_Sense = (float) (AD_RES[1] * V_Ref) / 4095.0;
	float Temperature = (((V_AT_25C - V_Sense) * 1000.0) / AVG_SLOPE) + 25.0;

}








