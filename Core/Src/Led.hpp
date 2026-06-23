//
// Created by mcu on 27/05/2026.
//

#ifndef CP_LED_H
#define CP_LED_H

#include "main.h" // Gives access to HAL_GPIO functions

enum class LedPolarity : bool {
	ActLO = GPIO_PIN_RESET,
	ActHI = GPIO_PIN_SET,
};


class Led {
private:
	GPIO_TypeDef* gpioPort;
	uint16_t gpioPin;
	LedPolarity activePolarity;

public:
	// Constructor to assign hardware pins
	Led(GPIO_TypeDef* port, uint16_t pin, LedPolarity polarity);


	// Class Methods
	void on() const;
	void off() const;
	void toggle() const;

	void blink(uint32_t ticksOn) const;
	void blink(uint32_t ticksOn, uint32_t ticksOff) const;

	void blink(uint32_t repeatCnt, uint32_t ticksOn, uint32_t ticksOff) const;

	void blur(uint32_t blurDurationTicks, uint32_t ticksOn, uint32_t ticksOff) const;

};

#endif //CP_LED_H
