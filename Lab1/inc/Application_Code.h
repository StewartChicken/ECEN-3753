#ifndef APPLICATION_CODE_H_
#define APPLICATION_CODE_H_

#include "stm32f4xx_hal.h"

// Button port and pin number
#define BUTTON_PORT GPIOA
#define BUTTON_PIN GPIO_PIN_0

// LED port and pin numbers
#define LED_PORT GPIOG
#define GREEN_LED_PIN GPIO_PIN_13
#define RED_LED_PIN GPIO_PIN_14

// Button states
#define BUTTON_NOT_PRESSED 0
#define BUTTON_PRESSED 1

void APPLICATION_init(void);
void APPLICATION_blink_led(void);
void APPLICATION_get_button_state(void);


#endif