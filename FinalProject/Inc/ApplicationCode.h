/*
 * ApplicationCode.h
 *
 *  Created on: Nov 14, 2023
 *      Author: xcowa
 */

#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#include "LCD_Driver.h"
#include "Gyro_Driver.h"
#include "cmsis_os.h"
 
// Possible button states
#define BUTTON_NOT_PRESSED 0
#define BUTTON_PRESSED 1

// Button interrupt information
#define BUTTON_IRQ_NUMBER 6
#define EXTI0_DEFAULT_PRIORITY 13

#define INITIAL_BOARD_ANGLE 180 // Board is initially assumed to be at 180 degrees or flat

#define DISRUPTOR_ENERGY_DEPLETION_RATE 1 			// How much the disruptor's energy level decrements by
#define DISRUPTOR_ENERGY_DEPLETION_INTERVAL 30 	// How often energy depletes while button is being pressed
#define DISRUPTOR_ENERGY_RECHARGE_RATE 1 			// How much the disruptor's energy level increments by
#define DISRUPTOR_ENERGY_RECHARGE_INTERVAL 200		// How often the energy increments while recharging

#define DISRUPTOR_MIN_ACTIVATION_ENERGY 30      // Disruptor can be used after it is charged to at least 30 percent

#define DISRUPTOR_ENERGY_DEPLETE_EVENT 		0x1 // 0b00000001
#define DISRUPTOR_ENERGY_RECHARGE_EVENT 	0x2 // 0b00000010

// LED event flags
#define ENABLE_RED_LED_EVENT           0x0001
#define DISABLE_RED_LED_EVENT          0x0002
#define ENABLE_GREEN_LED_EVENT         0x0004
#define DISABLE_GREEN_LED_EVENT        0x0008

#define GYRO_SAMPLE_RATE 20 // Sample gyro at 1 ms frequency
#define LCD_UPDATE_RATE  100 // Update LCD screen every 100 ms

[[maybe_unused]] static uint8_t green_led_state; // 0 if off, 1 if on
[[maybe_unused]] static uint8_t red_led_state; // 0 if off, 1 if on

[[maybe_unused]] static uint8_t button_state; // 0 if not pressed, 1 if pressed

[[maybe_unused]] static uint8_t disruptor_energy_level; // 0-100 percent
[[maybe_unused]] static uint8_t disruptor_depleting; 	// 0 if recharging, 1 if depleting

[[maybe_unused]] static uint8_t green_led_timer_tick; // Counts from 0 to 10 - used for duty cycle of green led

// Gyroscope data
[[maybe_unused]] static int16_t gyro_velocity_x;
[[maybe_unused]] static int16_t gyro_velocity_y;
[[maybe_unused]] static int16_t gyro_angle_x = INITIAL_BOARD_ANGLE;
[[maybe_unused]] static int16_t gyro_angle_y = INITIAL_BOARD_ANGLE;


// =================================================================================================
/* Thread instantiation */
// 

// Disruptor task
[[maybe_unused]] static osThreadId_t disruptor_task;
[[maybe_unused]] static const osThreadAttr_t disruptor_task_attributes = {
    .name = "disruptor_task",
    .priority = osPriorityNormal
};

// Button task
[[maybe_unused]] static osThreadId_t button_task;
[[maybe_unused]] static const osThreadAttr_t button_task_attributes = {
    .name = "button_task",
    .priority = osPriorityNormal
};

// LCD display task
[[maybe_unused]] static osThreadId_t lcd_display_task;
[[maybe_unused]] static const osThreadAttr_t lcd_display_task_attributes = {
    .name = "lcd_display_task",
    .priority = osPriorityNormal
};

// LED output task
[[maybe_unused]] static osThreadId_t led_output_task;
[[maybe_unused]] static const osThreadAttr_t led_output_task_attributes = {
    .name = "led_output_task",
    .priority = osPriorityNormal
};

// Gyro angle task
[[maybe_unused]] static osThreadId_t gyro_angle_task;
[[maybe_unused]] static const osThreadAttr_t gyro_angle_task_attributes = {
    .name = "gyro_angle_task",
    .priority = osPriorityNormal
};

// Disruptor energy depletion timer
[[maybe_unused]] static osTimerId_t depletion_timer;
[[maybe_unused]] static const osTimerAttr_t depletion_timer_attributes = {
    .name = "depletion_timer"
};

// Disruptor energy recharge timer
[[maybe_unused]] static osTimerId_t recharge_timer;
[[maybe_unused]] static const osTimerAttr_t recharge_timer_attributes = {
    .name = "recharge_timer"
};

// Green led pwm duty cycle timer
[[maybe_unused]] static osTimerId_t green_led_pwm_timer;
[[maybe_unused]] static const osTimerAttr_t green_led_pwm_timer_attributes = {
    .name = "green_led_pwm_timer"
};


// =================================================================================================
/* ITC construct declarations */
// 

// Button press semaphore 
[[maybe_unused]] static osSemaphoreId_t button_semaphore;
[[maybe_unused]] static const osSemaphoreAttr_t button_semaphore_attributes = {
    .name = "button_semaphore"
};

// Disruptor energy mutex
[[maybe_unused]] static osMutexId_t energy_level_mutex;
[[maybe_unused]] static const osMutexAttr_t energy_level_mutex_attributes = {
    .name = "energy_level_mutex"
};

// Gyro angle mutex
[[maybe_unused]] static osMutexId_t gyro_angle_mutex;
[[maybe_unused]] static const osMutexAttr_t gyro_angle_mutex_attributes = {
    .name = "gyro_angle_mutex"
};

// Disruptor event flag group
[[maybe_unused]] static osEventFlagsId_t disruptor_event;
[[maybe_unused]] static const osEventFlagsAttr_t disruptor_event_attributes = {
    .name = "disruptor_event"
};

// Led output event flag group
[[maybe_unused]] static osEventFlagsId_t led_event;
[[maybe_unused]] static const osEventFlagsAttr_t led_event_attributes = {
    .name = "led_event"
};

void ApplicationInit(void);

void APPLICATION_enable_button_interrupts(void);

// Checks state of button - loads state into 'button_state' variable
void APPLICATION_sample_button(void);

// Checks speed of gyro rotation and stores in variable 'gyro_rotation'
void APPLICATION_sample_gyro(void); 

void RunDemoForLCD(void);

// Turn on/off LEDs
void APPLICATION_activate_green_led(void);
void APPLICATION_activate_red_led(void);
void APPLICATION_deactivate_green_led(void);
void APPLICATION_deactivate_red_led(void);
void APPLICATION_toggle_green_led(void);
void APPLICATION_toggle_red_led(void);

// Thread function declarations
void button_task_function(void *arg);
void disruptor_task_function(void *arg);
void lcd_display_task_function(void *arg);
void led_output_task_function(void *arg);
void gyro_angle_task_function(void *arg);

// Periodically depletes disruptor energy level
void depletion_timer_callback(void *arg);

// Periodically recharges disruptor energy level
void recharge_timer_callback(void *arg);

// Manages pwm of green led
void green_led_pwm_timer_callback(void *arg);

#endif /* INC_APPLICATIONCODE_H_ */
