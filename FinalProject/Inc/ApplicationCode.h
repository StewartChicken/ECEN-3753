/*
 * ApplicationCode.h
 *
 *  Created on: Nov 14, 2023
 *      Author: xcowa
 */

#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "LCD_Driver.h"
#include "Gyro_Driver.h"
#include "RNG.h"
#include "cmsis_os.h"
#include "Config.h"


//************************************************************************************************
// Config

#define GAME_UPDATE_FREQUENCY 50 // Update every 50 ms

//************************************************************************************************

// Possible button states
#define BUTTON_NOT_PRESSED 0
#define BUTTON_PRESSED 1

// Button interrupt information
#define BUTTON_IRQ_NUMBER 6
#define EXTI0_DEFAULT_PRIORITY 13

// Board is initially assumed to be at 180 degrees or flat
#define INITIAL_BOARD_ANGLE 180 

// Energy event flag masks
#define DEPLETE_ENERGY_EVENT 		0x1 // 0b00000001
#define RECHARGE_ENERGY_EVENT   	0x2 // 0b00000010

// LED event flags
#define ENABLE_RED_LED_EVENT           0x0001
#define DISABLE_RED_LED_EVENT          0x0002
#define ENABLE_GREEN_LED_EVENT         0x0004
#define DISABLE_GREEN_LED_EVENT        0x0008

#define GYRO_SAMPLE_RATE 20 // Sample gyro every 20 ms
#define LCD_UPDATE_RATE  100 // Update LCD screen every 100 ms

enum PinAtCenter {
    DRONE,
    MAZE
};


// This data structure will be used to represent holes within the game
// Collision detection will use this data
[[maybe_unused]] typedef struct {
    uint16_t x; // X position of hole center
    uint32_t y; // Y position of hole center
} HoleData_t;

// This data structure will be used to represent waypoints within the game
// Collision detection will use this data
// Waypoints are numbered incrementally starting from 0 (start)
[[maybe_unused]] typedef struct {
    int16_t x;         // X position of hole center
    int16_t y;         // Y position of hole center
    uint8_t number;    // Index of this waypoint 
    bool reached;         // True if this waypoint has been reached previously - false initially
} WaypointData_t;

/* Data structure for a cell - for now, it is assumed that the map cell count is always 6 - can change later
 * The rightmost five bits contain the map data 
 * 0b00000001  - Top wall
 * 0b00000010  - Bottom wall
 * 0b00000100  - Left wall 
 * 0b00001000  - Right wall
 * 0b00010000  - Hole
 * 0b00100000  - Waypoint
*/ 
//[[maybe_unused]] static uint8_t **cell_data; // 2D Array of 8-bit integers used for map generation
[[maybe_unused]] static uint8_t cell_data[6][6];

[[maybe_unused]] static HoleData_t *hole_data; // Dynamic array of holes that are generated within the map
[[maybe_unused]] static uint8_t num_holes;

[[maybe_unused]] static WaypointData_t waypoint_data[4]; // Dynamic array of waypoints that are generated within the map
[[maybe_unused]] static uint8_t current_waypoint; // The current waypoint the player must reach

[[maybe_unused]] static uint8_t green_led_state; // 0 if off, 1 if on
[[maybe_unused]] static uint8_t red_led_state; // 0 if off, 1 if on

[[maybe_unused]] static uint8_t green_led_timer_tick; // Counts from 0 to 10 - used for duty cycle of green led

// Gyro data
[[maybe_unused]] static int16_t gyro_angle_x = INITIAL_BOARD_ANGLE;
[[maybe_unused]] static int16_t gyro_angle_y = INITIAL_BOARD_ANGLE;

[[maybe_unused]] static uint32_t red_led_timer_tick; 
[[maybe_unused]] static uint32_t red_led_timer_period; // Rate the red led flashes

[[maybe_unused]] static uint8_t button_state; // 0 if not pressed, 1 if pressed

[[maybe_unused]] static uint8_t disruptor_active; // 0 if off, 1 if on
[[maybe_unused]] static uint8_t disruptor_can_be_activated = 1; // 0 if disruptor cannot be activated, 1 if it can be activated

[[maybe_unused]] static int32_t drone_energy; // Range from 0 to 15000 mJ
[[maybe_unused]] static int32_t drone_position_x, drone_position_y; // Pixel value 
[[maybe_unused]] static int32_t drone_velocity_x, drone_velocity_y; // Pixel value 

[[maybe_unused]] static uint8_t cellRow;
[[maybe_unused]] static uint8_t cellCol;

[[maybe_unused]] static bool game_won = false;
[[maybe_unused]] static bool game_lost = false;
[[maybe_unused]] static bool fell_into_hole = false;
[[maybe_unused]] static bool ran_out_of_time = false;
[[maybe_unused]] static bool exceeded_tilt = false;

[[maybe_unused]] static uint32_t game_tick;

// LCD display task
[[maybe_unused]] static osThreadId_t lcd_display_task;
[[maybe_unused]] static const osThreadAttr_t lcd_display_task_attributes = {
    .name = "lcd_display_task",
    .priority = osPriorityNormal
};

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

// Gyro angle task
[[maybe_unused]] static osThreadId_t gyro_angle_task;
[[maybe_unused]] static const osThreadAttr_t gyro_angle_task_attributes = {
    .name = "gyro_angle_task",
    .priority = osPriorityNormal
};

// LED output task
[[maybe_unused]] static osThreadId_t led_output_task;
[[maybe_unused]] static const osThreadAttr_t led_output_task_attributes = {
    .name = "led_output_task",
    .priority = osPriorityNormal
};

// Game task
[[maybe_unused]] static osThreadId_t game_task;
[[maybe_unused]] static const osThreadAttr_t game_task_attributes = {
    .name = "game_task",
    .priority = osPriorityNormal
};

// Energy recharge timer
[[maybe_unused]] static osTimerId_t energy_recharge_timer;
[[maybe_unused]] static const osTimerAttr_t energy_recharge_timer_attributes = {
    .name = "energy_recharge_timer"
};

// Energy depletion timer
[[maybe_unused]] static osTimerId_t energy_depletion_timer;
[[maybe_unused]] static const osTimerAttr_t energy_depletion_timer_attributes = {
    .name = "energy_depletion_timer"
};

// Green led pwm duty cycle timer
[[maybe_unused]] static osTimerId_t green_led_pwm_timer;
[[maybe_unused]] static const osTimerAttr_t green_led_pwm_timer_attributes = {
    .name = "green_led_pwm_timer"
};

// Red led control timer
[[maybe_unused]] static osTimerId_t red_led_timer;
[[maybe_unused]] static const osTimerAttr_t red_led_timer_attributes = {
    .name = "red_led_timer"
};

[[maybe_unused]] static osTimerId_t game_timer;
[[maybe_unused]] static const osTimerAttr_t game_timer_attributes = {
    .name = "game_timer"
};

// Gyro angle mutex
[[maybe_unused]] static osMutexId_t gyro_angle_mutex;
[[maybe_unused]] static const osMutexAttr_t gyro_angle_mutex_attributes = {
    .name = "gyro_angle_mutex"
};

// Drone position mutex
[[maybe_unused]] static osMutexId_t drone_position_mutex;
[[maybe_unused]] static const osMutexAttr_t drone_position_mutex_attributes = {
    .name = "drone_position_mutex"
};

// Disruptor energy event flag group
[[maybe_unused]] static osEventFlagsId_t energy_event;
[[maybe_unused]] static const osEventFlagsAttr_t energy_event_attributes = {
    .name = "energy_event"
};

// Led output event flag group
[[maybe_unused]] static osEventFlagsId_t led_event;
[[maybe_unused]] static const osEventFlagsAttr_t led_event_attributes = {
    .name = "led_event"
};

// Button press semaphore 
[[maybe_unused]] static osSemaphoreId_t button_semaphore;
[[maybe_unused]] static const osSemaphoreAttr_t button_semaphore_attributes = {
    .name = "button_semaphore"
};

void ApplicationInit(void);
void APPLICATION_configure_settings(void);
void APPLICATION_enable_button_interrupts(void);
void APPLICATION_sample_button(void);
void APPLICATION_sample_gyro(void); 

// Turn on/off LEDs
void APPLICATION_activate_green_led(void);
void APPLICATION_activate_red_led(void);
void APPLICATION_deactivate_green_led(void);
void APPLICATION_deactivate_red_led(void);
void APPLICATION_toggle_green_led(void);
void APPLICATION_toggle_red_led(void);

// Map generation functions
void APPLICATION_create_map(void);
void APPLICATION_draw_map(void);

// Map interaction functions
bool APPLICATION_is_over_hole(int32_t xCoor, int32_t yCoor);
int8_t APPLICATION_is_over_waypoint(int32_t xCoor, int32_t yCoor);
bool APPLICATION_check_horizontal_wall_collision(int32_t xCoor, int32_t yCoor); // True if drone is touching wall
bool APPLICATION_check_vertical_wall_collision(int32_t xCoor, int32_t yCoor); // True if drone is touching wall
bool APPLICATION_check_drone_overlap(int32_t drone_pos, int32_t line_pos);

int32_t APPLICATION_get_board_gravity_ratio(int16_t angle);

void lcd_display_task_function(void *arg);
void disruptor_task_function(void *arg);
void button_task_function(void *arg);
void gyro_angle_task_function(void *arg);
void led_output_task_function(void *arg);
void game_task_function(void *arg);

void energy_recharge_timer_callback(void *arg);
void energy_depletion_timer_callback(void *arg);
void green_led_pwm_timer_callback(void *arg);
void red_led_timer_callback(void *arg);
void game_timer_callback(void *arg);


#endif