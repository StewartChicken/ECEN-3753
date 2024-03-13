#ifndef APPLICATION_CODE_H
#define APPLICATION_CODE_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "gyro.h"
#include "cmsis_os.h"

// Possible button states
#define BUTTON_NOT_PRESSED 0
#define BUTTON_PRESSED 1

// Button interrupt information
#define BUTTON_IRQ_NUMBER 6
#define EXTI0_DEFAULT_PRIORITY 13

// Event flag mask for a button press
#define BUTTON_PRESSED_EVENT_MASK 0x00000001U

// Max capacity of LED message queue
#define LED_MESSAGE_QUEUE_SIZE 16

// Gyro sampled every 10 ms
#define GYRO_SAMPLE_RATE 10U

[[maybe_unused]] static uint8_t green_led_state; // 0 if off, 1 if on
[[maybe_unused]] static uint8_t red_led_state; // 0 if off, 1 if on

[[maybe_unused]] static uint8_t button_state; // 0 if not pressed, 1 if pressed
[[maybe_unused]] static uint8_t gyro_rotation; // 0-4 - defined in GyroRotationRates enum 

typedef enum {
    GYRO_FAST_CW_ROTATION = 0, // Clock-wise fast rotation - negative gyro values
    GYRO_SLOW_CW_ROTATION,     // Clock-wise slow rotation - negative gyro values
    GYRO_ZERO_ROTATION,        // Approximately zero rotation
    GYRO_FAST_CCW_ROTATION,    // Counter clockwise fast rotation - positive gyro values
    GYRO_SLOW_CCW_ROTATION     // Counter clockwise slow rotation - positive gyro values
} GyroRotationRates;

// LED message queue data structure
typedef struct {
    int8_t msg_button_state; // 0 if not pressed, 1 if pressed, -1 if unknown
    int8_t msg_gyro_rotation; // -1 if unknown - 0-4 for other states
} LedMessage_t;

// Button input task
[[maybe_unused]] static osThreadId_t button_input_task;
[[maybe_unused]] static const osThreadAttr_t button_input_task_attributes = {
    .name = "button_input_task",
    .priority = osPriorityAboveNormal
};

// Gyro input task
[[maybe_unused]] static osThreadId_t gyro_input_task;
[[maybe_unused]] static osThreadAttr_t gyro_input_task_attributes = {
    .name = "gyro_input_task",
    .priority = osPriorityNormal
};

// LED output task
[[maybe_unused]] static osThreadId_t led_output_task;
[[maybe_unused]] static const osThreadAttr_t led_output_task_attributes = {
    .name = "led_output_task",
    .priority = osPriorityNormal
};

// Button press event flag
[[maybe_unused]] static osEventFlagsId_t button_pressed_event;
[[maybe_unused]] static const osEventFlagsAttr_t button_pressed_event_attributes = {
    .name = "button_pressed_event"
};

// LED message queue
[[maybe_unused]] static osMessageQueueId_t led_message_queue;
[[maybe_unused]] static const osMessageQueueAttr_t led_message_queue_attributes = {
    .name = "led_message_queue"
};

// Gyro sample timer
[[maybe_unused]] static osTimerId_t gyro_timer;
[[maybe_unused]] static const osTimerAttr_t gyro_timer_attributes = {
    .name = "gyro_timer"
};

// Gyro semaphore
[[maybe_unused]] static osSemaphoreId_t gyro_semaphore;
[[maybe_unused]] static const osSemaphoreAttr_t gyro_semaphore_attributes = {
    .name = "gyro_semaphore"
};

void APPLICATION_init(void);

void APPLICATION_enable_button_interrupts(void);

// Checks state of button - loads state into 'button_state' variable
void APPLICATION_sample_button(void);

// Checks speed of gyro rotation and stores in variable 'gyro_rotation'
void APPLICATION_get_gyro_rotation_rate(void); 

// Checks if gyro is rotating clockwise/counter-clockwise
bool APPLICATION_is_gyro_rotating_cw(void);
bool APPLICATION_is_gyro_rotating_ccw(void);

// Turn on/off LEDs
void APPLICATION_activate_green_led(void);
void APPLICATION_activate_red_led(void);
void APPLICATION_deactivate_green_led(void);
void APPLICATION_deactivate_red_led(void);
void APPLICATION_toggle_green_led(void);
void APPLICATION_toggle_red_led(void);

void button_input_function(void *arg);
void gyro_input_function(void *arg);
void led_output_function(void *arg);
void gyro_timer_callback(void *arg);

#endif