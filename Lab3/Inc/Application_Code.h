#ifndef APPLICATION_CODE_H
#define APPLICATION_CODE_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "Gyro_Driver.h"
#include "cmsis_os.h"

#define LAB3_USE_TASK 1

#define BUTTON_NOT_PRESSED 0
#define BUTTON_PRESSED 1

[[maybe_unused]] static uint8_t button_state; // 0 if not pressed, 1 if pressed
[[maybe_unused]] static uint8_t gyro_rotation; // 0-4 - defined in GyroRotationRates enum

typedef enum {
    GYRO_FAST_CW_ROTATION = 0, // Clock-wise fast rotation - negative gyro values
    GYRO_SLOW_CW_ROTATION,     // Clock-wise slow rotation - negative gyro values
    GYRO_ZERO_ROTATION,        // Approximately zero rotation
    GYRO_FAST_CCW_ROTATION,    // Counter clockwise fast rotation - positive gyro values
    GYRO_SLOW_CCW_ROTATION     // Counter clockwise slow rotation - positive gyro values
} GyroRotationRates;

// Timer variables
[[maybe_unused]] static osTimerId_t timer_id;
[[maybe_unused]] static StaticTimer_t timer_TCB;

[[maybe_unused]] static const osTimerAttr_t timer_attributes = {"MyCoolTimer", 0, &timer_TCB, sizeof(timer_TCB)};

// Task variables
[[maybe_unused]] static StaticTask_t task_TCB;
[[maybe_unused]] static uint32_t task_stack;
[[maybe_unused]] static osThreadId_t task_id;

//[[maybe_unused]] static const osThreatAttr_t task_attributes = {"MyCoolTask", 0, &task_TCB, sizeof(timer_TCB), &task_stack, sizeof(task_stack), osPriorityNormal};


void APPLICATION_init(void);

// Checks state of button - loads state into 'button_state' variable
void APPLICATION_sample_button(void);

// Checks speed of gyro rotation and stores in variable 'gyro_rotation'
void APPLICATION_get_gyro_rotation_rate(void); 

// Checks if gyro is rotating clockwise/counter-clockwise
bool APPLICATION_is_gyro_rotating_cw(void);
bool APPLICATION_is_gyro_rotating_ccw(void);


// Updates the state of the LEDs depending on the button and the gyro
void APPLICATION_update_led(void);

// Turn on/off LEDs
void APPLICATION_activate_green_led(void);
void APPLICATION_activate_red_led(void);
void APPLICATION_deactivate_green_led(void);
void APPLICATION_deactivate_red_led(void);

void os_timer_callback(void *arg);

void task_function(void *arg);

#endif
