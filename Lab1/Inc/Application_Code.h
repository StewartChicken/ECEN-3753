/*
 * Application_Code.h
 *
 *  Created on: Jan 24, 2024
 *      Author: stewartchicken
 */

#ifndef INC_APPLICATION_CODE_H_
#define INC_APPLICATION_CODE_H_

#include "stm32f4xx_hal.h"

// Button and LED peripheral address maps
#define BUTTON_PORT GPIOA
#define LED_PORT GPIOG

#define BUTTON_PIN GPIO_PIN_0
#define GREEN_LED_PIN GPIO_PIN_13
#define RED_LED_PIN GPIO_PIN_14

void APPLICATION_init(void);
void APPLICATION_blink_led(void);
void APPLICATION_get_button_state(void);

#endif /* INC_APPLICATION_CODE_H_ */
