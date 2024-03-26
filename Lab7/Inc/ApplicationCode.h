/*
 * ApplicationCode.h
 *
 *  Created on: Nov 14, 2023
 *      Author: xcowa
 */

// Last thing did: If speed exceeds 75 mph while going straight, green led activates. If exceeds 45 while turning, green activates.
// Todo: If turning for longer than 5 seconds, red led activates. Update turning status. Draw line to indicate turn direction.
// For the turning, it should be based on the angle of the gyroscope. Slight angle = slight turn, hard angle = hard turn. When held at an
// angle for longer than 5 seconds, direction warning light (red led) should activate. Whenever the board is rotated in a given direction (if
// the event flag which indicates a directional change is raised), start a timer that determines if the direction has been held for too long.  

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

#define INITIAL_DIRECTION_VALUE 10000

#define SPEED_INCREMENT 5U // Vehicle speed adjusted in increments of 5

#define GYRO_SAMPLE_RATE 10 // Sample gyro at 10 ms frequency
#define LCD_UPDATE_RATE  100 // Update LCD screen every 100 ms

// Vehicle control event flags
#define SPEED_UPDATE_EVENT_FLAG_MSK             0x0001
#define DIRECTION_UPDATE_EVENT_FLAG_MSK         0x0002

// LED event flags
#define ENABLE_RED_LED_EVENT           0x0001
#define DISABLE_RED_LED_EVENT          0x0002
#define ENABLE_GREEN_LED_EVENT         0x0004
#define DISABLE_GREEN_LED_EVENT        0x0008

[[maybe_unused]] static uint8_t green_led_state; // 0 if off, 1 if on
[[maybe_unused]] static uint8_t red_led_state; // 0 if off, 1 if on

[[maybe_unused]] static uint8_t button_state; // 0 if not pressed, 1 if pressed
[[maybe_unused]] static uint8_t gyro_rotation; // 0-4 - defined in GyroRotationRates enum 
[[maybe_unused]] static int16_t gyro_position = INITIAL_DIRECTION_VALUE;

[[maybe_unused]] static uint32_t button_press_duration; // Used to track how long the button has been pressed for
[[maybe_unused]] static uint32_t turn_duration;         // Tracks the duration of a given direction change

// =================================================================================================
/* Data structure initialization */
// 

// Stores vehicle speed information - shared by multiple resources
[[maybe_unused]] static struct SpeedData_t {
    uint32_t speed; // Current speed of the vehicle
    uint32_t numIncrements; // Number of times speed has been incremented
    uint32_t numDecrements; // Number of times speed has been decremented
} speed_data;

[[maybe_unused]] static struct DirectionData_t { 
    uint8_t direction; // Current direction - 0, 1, 2, 3, 4 - straight, slight left, hard left, slight right, hard right
    uint32_t numLeftTurns; // Number of times a left turn has been made
    uint32_t numRightTurns; // Number of times a right turn has been made
} direction_data; 

typedef enum {
    GYRO_FAST_CW_ROTATION = 0, // Clock-wise fast rotation - negative gyro values
    GYRO_SLOW_CW_ROTATION,     // Clock-wise slow rotation - negative gyro values
    GYRO_ZERO_ROTATION,        // Approximately zero rotation
    GYRO_FAST_CCW_ROTATION,    // Counter clockwise fast rotation - positive gyro values
    GYRO_SLOW_CCW_ROTATION     // Counter clockwise slow rotation - positive gyro values
} GyroRotationRates;


// =================================================================================================
/* Thread instantiation */
// 

// Speed setpoint task
[[maybe_unused]] static osThreadId_t speed_task;
[[maybe_unused]] static const osThreadAttr_t speed_task_attributes = {
    .name = "speed_task",
    .priority = osPriorityNormal
};

// Vehicle direction task
[[maybe_unused]] static osThreadId_t direction_task;
[[maybe_unused]] static const osThreadAttr_t direction_task_attributes = {
    .name = "direction_task",
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

// Vehicle control task
[[maybe_unused]] static osThreadId_t vehicle_control_task;
[[maybe_unused]] static const osThreadAttr_t vehicle_control_task_attributes = {
    .name = "vehicle_control_task",
    .priority = osPriorityNormal
};

// Button press timer
[[maybe_unused]] static osTimerId_t button_timer;
[[maybe_unused]] static const osTimerAttr_t button_timer_attributes = {
    .name = "button_timer"
};

// Direction duration timer
[[maybe_unused]] static osTimerId_t direction_timer;
[[maybe_unused]] static const osTimerAttr_t direction_timer_attributes = {
    .name = "direction_timer"
};

// =================================================================================================
/* ITC construct declarations */
// 

// Speed update semaphore
[[maybe_unused]] static osSemaphoreId_t speed_update_semaphore;
[[maybe_unused]] static const osSemaphoreAttr_t speed_update_semaphore_attributes = {
    .name = "speed_update_semaphore"
};

// Vehicle control event flag group
[[maybe_unused]] static osEventFlagsId_t vehicle_control_event;
[[maybe_unused]] static const osEventFlagsAttr_t vehicle_control_event_attributes = {
    .name = "vehicle_control_event"
};

// LED output event flag group
[[maybe_unused]] static osEventFlagsId_t led_output_event;
[[maybe_unused]] static const osEventFlagsAttr_t led_output_event_attributes = {
    .name = "led_output_event"
};

// Speed data mutex
[[maybe_unused]] static osMutexId_t speed_data_mutex;
[[maybe_unused]] static const osMutexAttr_t speed_data_mutex_attributes = {
    .name = "speed_data_mutex"
};

// Direction data mutex
[[maybe_unused]] static osMutexId_t direction_data_mutex;
[[maybe_unused]] static const osMutexAttr_t direction_data_mutex_attributes = {
    .name = "direction_data_mutex"
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
void speed_task_function(void *arg);
void direction_task_function(void *arg);
void lcd_display_task_function(void *arg);
void led_output_task_function(void *arg);
void vehicle_control_task_function(void *arg);

// Button press timer callback
void button_timer_callback(void *arg);

// Direction chnage timer callback
void direction_timer_callback(void *arg);

#endif /* INC_APPLICATIONCODE_H_ */
