#include "Led.hpp"

#include "BoardLed.h"
#include "main.h"

Led::Led(GPIO_TypeDef *port, uint16_t pin, LedPolarity polarity) {
	gpioPort = port;
	gpioPin = pin;
	activePolarity = polarity;
}


void Led::on() const {
	(activePolarity == LedPolarity::ActHI) ? HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_SET) : HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET);
}
void Led::off() const {
	(activePolarity == LedPolarity::ActHI) ? HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET) : HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_SET);
}
void Led::toggle() const {
	HAL_GPIO_TogglePin(gpioPort, gpioPin);
}

void Led::blink(uint32_t ticksOn) const {
	on();
	bspDelay(ticksOn);
	off();
}

void Led::blink(uint32_t ticksOn, uint32_t ticksOff) const {
	on();
	bspDelay(ticksOn);
	off();
	bspDelay(ticksOff);
}

void Led::blink(uint32_t repeatCnt, uint32_t ticksOn, uint32_t ticksOff) const {
	if (repeatCnt == 0) repeatCnt = 1;
	for (uint32_t i = 0; i < repeatCnt; ++i) {
		blink(ticksOn, ticksOff);
	}
}

void Led::blur(uint32_t blurDurationTicks, uint32_t ticksOn, uint32_t ticksOff) const {
	uint32_t count = blurDurationTicks / (ticksOn + ticksOff);
	if (count == 0) count = 1;
	for (uint32_t i = 0; i < count; ++i) {
		on();
		bspDelay(ticksOn);
		off();
		bspDelay(ticksOff);
	}
}
