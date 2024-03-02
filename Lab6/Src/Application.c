#include "Application.h"

void APPLICATION_init(void)
{
    APPLICATION_activate_green_led();
    APPLICATION_activate_red_led();
    led_state = 1; // Update state

    APPLICATION_enable_button_interrupts();

    button_pressed_event = osEventFlagsNew(&button_pressed_event_attributes);

    // Check for successful creation
    if(button_pressed_event == NULL)
        while(1);
  
    button_input_task = osThreadNew(button_input_task_function, (void *)0, &button_input_task_attributes);

    // Check for successful creation
    if(button_input_task == NULL)
        while(1);
}


void APPLICATION_activate_green_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
}
void APPLICATION_activate_red_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
}

void APPLICATION_deactivate_green_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
}

void APPLICATION_deactivate_red_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}

void APPLICATION_enable_button_interrupts(void)
{
	HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);
}

void APPLICATION_toggle_both_leds(void)
{
    if(led_state == 1)
    {
        led_state = 0;
        APPLICATION_deactivate_red_led();
        APPLICATION_deactivate_green_led();
    } else {
        led_state = 1;
        APPLICATION_activate_red_led();
        APPLICATION_activate_green_led();
    }
}

void button_input_task_function(void *arg)
{
    (void) &arg; // Remove warnings

    [[maybe_unused]] uint32_t event_flags;

    while(1)
    {
       event_flags = osEventFlagsWait(button_pressed_event, BUTTON_PRESSED_EVENT_MASK, osFlagsWaitAny, osWaitForever);
       APPLICATION_toggle_both_leds();
    }
}

void EXTI0_IRQHandler()
{
	HAL_NVIC_DisableIRQ(BUTTON_IRQ_NUMBER);

    osEventFlagsSet(button_pressed_event, BUTTON_PRESSED_EVENT_MASK);

	// Clear button's pending interrupt
    EXTI_HandleTypeDef button;
    button.Line = 0;

    HAL_EXTI_ClearPending(&button, EXTI_TRIGGER_RISING_FALLING);

	HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);

}


