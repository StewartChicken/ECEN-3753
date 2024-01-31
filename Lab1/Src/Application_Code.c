#ifndef APPLICATION_H
#define APPLICATION_H

#include "Application_Code.h"

void APPLICATION_init(void)
{
	HAL_GPIO_WritePin(LED_PORT, GREEN_LED_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_PORT, RED_LED_PIN, GPIO_PIN_RESET);
}

void APPLICATION_blink_led(void)
{
	HAL_GPIO_TogglePin(LED_PORT, RED_LED_PIN);
	HAL_Delay(1000);
}

void APPLICATION_get_button_state(void)
{
	if(HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_SET)
	{
		HAL_GPIO_WritePin(LED_PORT, GREEN_LED_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(LED_PORT, GREEN_LED_PIN, GPIO_PIN_RESET);
	}
}

#endif
