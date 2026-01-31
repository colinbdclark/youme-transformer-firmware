#include "pico/stdlib.h"

class LED {
public:
    uint8_t gpioPin;
    bool isOn;

    void init(uint8_t gpioPin) {
        this->gpioPin = gpioPin;
        this->isOn = false;

        gpio_init(gpioPin);
        gpio_set_dir(gpioPin, GPIO_OUT);
        gpio_put(gpioPin, 0);
    }

    void on() {
        if (isOn == false) {
            isOn = true;
            gpio_put(gpioPin, 1);
        }
    }

    void off() {
        if (isOn == true) {
            isOn = false;
            gpio_put(gpioPin, 0);
        }
    }

    void toggle() {
        isOn = !isOn;
        gpio_put(gpioPin, (int) isOn);
    }
};
