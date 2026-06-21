//
// Created by mcu on 29/05/2026.
//

#include "main.h"

#include "DS18b20_bitbang.h"


////////////////////////////////////
/// KAKO SE KORISTI
/// ////////////////////////////////
///
///////////////////////////
/// DS18b20
/// https://controllerstech.com/ds18b20-and-stm32/#info_box
/// //////////////////////
/*

int presence = DS18B20_BB_Start ();
DS18B20_BB_Write (0xCC);  // skip ROM
DS18B20_BB_Write (0x44);  // convert t

presence = DS18B20_BB_Start ();
DS18B20_BB_Write (0xCC);  // skip ROM
DS18B20_BB_Write (0xBE);  // Read Scratch-pad

uint8_t TempLSB = DS18B20_BB_read();
uint8_t TempMSB = DS18B20_BB_read();
uint16_t TEMP = (TempMSB<<8) | TempLSB;
float Temperature = ( (float)TEMP ) / 16.0f;  // resolution is 0.0625

*/


/////////////////////////////////
/////////////////////////////////
/// BITBANG
/////////////////////////////////
/////////////////////////////////

extern TIM_HandleTypeDef Timer_uS;

void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;		// open drain is the best choice here
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;				// If external ds18b20 pullup is missing, this might help
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void delay_uS (uint16_t time) {
	uint16_t start = __HAL_TIM_GET_COUNTER(&Timer_uS);
	// subtraction cleanly handles 16-bit register overflows
	while ((uint16_t)(__HAL_TIM_GET_COUNTER(&Timer_uS) - start) < time) {
		__NOP(); // Prevents the compiler from optimizing out the loop
	}

}

int DS18B20_BB_Start (void) {
	int res = 0;
	Set_Pin_Output(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);						// set the pin as output
	HAL_GPIO_WritePin (DS18_bitbang_GPIO_Port, DS18_bitbang_Pin, 0);		// pull the pin low
	delay_uS(480);																// delay according to datasheet
	Set_Pin_Input(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);						// set the pin as input
	delay_uS (80);																// delay according to datasheet
	if ( (HAL_GPIO_ReadPin (DS18_bitbang_GPIO_Port, DS18_bitbang_Pin)) == 0 ) {
		res = 1;																	// if pin = low, the presence pulse is detected
	} else {
		res = -1;
	}
	delay_uS (400);															// 480 us delay totally.
	return res;
}

void DS18B20_BB_Write (uint8_t data) {
	Set_Pin_Output(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);							// set as output
	for (int i=0; i<8; i++) {
		if ( (data & (1<<i) ) != 0 )  {
			// if the bit is HIGH, master write 1 for 60..120uS
			Set_Pin_Output(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);					// set as output
			HAL_GPIO_WritePin (DS18_bitbang_GPIO_Port, DS18_bitbang_Pin, 0);	// pull the pin LOW
			delay_uS (1);															// wait for 1 us
			Set_Pin_Input(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);					// set as input
			delay_uS (60);															// wait for 60 us
		} else {
			// if bit is LOW, master writes 0 for 60..120uS
			Set_Pin_Output(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);
			HAL_GPIO_WritePin (DS18_bitbang_GPIO_Port, DS18_bitbang_Pin, 0);	// pull the pin LOW
			delay_uS (60);															// wait for 60 us
			Set_Pin_Input(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);
		}
	}
}


uint8_t DS18B20_BB_read (void) {
	uint8_t value=0;
	Set_Pin_Input(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);

	for (int i=0;i<8;i++) {
		Set_Pin_Output(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);					// set as output
		HAL_GPIO_WritePin (DS18_bitbang_GPIO_Port, DS18_bitbang_Pin, 0);	// pull the data pin LOW
		delay_uS (2);															// wait for >=1 us
		Set_Pin_Input(DS18_bitbang_GPIO_Port, DS18_bitbang_Pin);					// set as input
		if (HAL_GPIO_ReadPin (DS18_bitbang_GPIO_Port, DS18_bitbang_Pin)) {
			value |= 1<<i;															// if the pin is HIGH read = 1
		}
		delay_uS (60);															// wait for 60 us
	}
	return value;
}




