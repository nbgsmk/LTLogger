//
// Created by mcu on 29/05/2026.
//

#ifndef DS18B20_F411CEU_DS18B20_BITBANG_H
#define DS18B20_F411CEU_DS18B20_BITBANG_H



void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void delay_uS (uint16_t time);

int DS18B20_BB_Start (void);
void DS18B20_BB_Write (uint8_t data);
uint8_t DS18B20_BB_read (void);

#endif //DS18B20_F411CEU_DS18B20_BITBANG_H
