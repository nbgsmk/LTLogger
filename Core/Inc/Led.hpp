#ifndef LED_HPP
#define LED_HPP

#include "main.h"

extern "C" {
#include "BoardLed.h" // provides LedActPol_t and bspDelay macro
}

class Led {
public:
    // Construct with GPIO port and pin to control.
    Led(GPIO_TypeDef* port, uint16_t pin) noexcept;

    // No default construction without pins
    Led() = delete;

    void setActivePolarity(LedActPol_t p) { activePol = p; }
    LedActPol_t getActivePolarity() const { return activePol; }

    void on();
    void off();
    void toggle();

    void blink(uint32_t ticksOn);
    void blinkPer(uint32_t ticksOn, uint32_t ticksOff);
    void blinkPerX(uint32_t repeat, uint32_t ticksOn, uint32_t ticksOff);
    void blur(uint32_t ticksDuration, uint32_t ticksOn, uint32_t ticksOff);

private:
    GPIO_TypeDef* port;
    uint16_t pin;
    LedActPol_t activePol = actLOW;
};

#endif // LED_HPP
