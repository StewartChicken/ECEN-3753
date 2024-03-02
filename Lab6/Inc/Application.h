#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "gyro.h"
#include "cmsis_os.h"

// Current issue - when the button_pressed task function is called, system hardfaults. 

#define BUTTON_IRQ_NUMBER 6

// Event flag mask for a button press
#define BUTTON_PRESSED_EVENT_MASK 0x00000001U

[[maybe_unused]] static uint8_t led_state; // 0 if off, 1 if on

// Button input task variables
[[maybe_unused]] static StaticTask_t button_input_task_TCB;
[[maybe_unused]] static uint32_t button_input_task_stack;
[[maybe_unused]] static osThreadId_t button_input_task;

// Button input task attributes
[[maybe_unused]] static const osThreadAttr_t button_input_task_attributes = {
    .name = "button_input_task",
    .cb_mem = &button_input_task_TCB,
    .cb_size = sizeof(button_input_task_TCB),
    .stack_mem = &button_input_task_stack,
    .stack_size = sizeof(button_input_task_stack)
};

// Event to indicate that the button was pressed
[[maybe_unused]] static osEventFlagsId_t button_pressed_event;

// Button pressed event attributes
[[maybe_unused]] static const osEventFlagsAttr_t button_pressed_event_attributes = {
    .name = "button_pressed_event",
};

void APPLICATION_init(void);

// Turn on/off LEDs
void APPLICATION_activate_green_led(void);
void APPLICATION_activate_red_led(void);
void APPLICATION_deactivate_green_led(void);
void APPLICATION_deactivate_red_led(void);
void APPLICATION_toggle_both_leds(void);

void APPLICATION_enable_button_interrupts(void);

void button_input_task_function(void *arg);

#endif
