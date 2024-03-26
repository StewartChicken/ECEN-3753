/*
 * ApplicationCode.c
 *
 *  Created on: Nov 14, 2023
 *      Author: xcowa
 */

#include "ApplicationCode.h"



void ApplicationInit(void)
{
	LTCD__Init();
    LTCD_Layer_Init(0);
    Gyro_Init();

	[[maybe_unused]] static osStatus_t init_status;

    APPLICATION_enable_button_interrupts();

    // Initial vehicle state values
    speed_data.speed = 0;
    speed_data.numIncrements = 0;
    speed_data.numDecrements = 0;

    direction_data.direction = 0;
    direction_data.numLeftTurns = 0;
    direction_data.numRightTurns = 0;

    // =================================================================================================
    /* Speed update semaphore initialization */
    //

    speed_update_semaphore = osSemaphoreNew(1, 0, &speed_update_semaphore_attributes);

    if(speed_update_semaphore == NULL)
        while(1);


    
    // =================================================================================================
    /* Vehicle control event flag group initialization */
    //

    vehicle_control_event = osEventFlagsNew(&vehicle_control_event_attributes);

    if(vehicle_control_event == NULL)
        while(1);

    

    // =================================================================================================
    /* Led output event flag group initialization */
    //

    led_output_event = osEventFlagsNew(&led_output_event_attributes);

    if(led_output_event == NULL)
        while(1);


    // =================================================================================================
    /* Speed data mutex initialization */
    //

    speed_data_mutex = osMutexNew(&speed_data_mutex_attributes);

    if(speed_data_mutex == NULL)
        while(1);



    // =================================================================================================
    /* Speed data mutex initialization */
    //

    direction_data_mutex = osMutexNew(&direction_data_mutex_attributes);

    if(direction_data_mutex == NULL)
        while(1);



    // =================================================================================================
    /* Button press timer initialization */
    //

    button_timer = osTimerNew(button_timer_callback, osTimerPeriodic, &button_press_duration, &button_timer_attributes);

    if(button_timer == NULL)
        while(1);


    // =================================================================================================
    /* Direction timer initialization */
    //

    direction_timer = osTimerNew(direction_timer_callback, osTimerPeriodic, &turn_duration, &direction_timer_attributes);

    if(direction_timer == NULL)
        while(1);


	// =================================================================================================
    /* Thread initialization */
    //

	// Create the speed update thread
    speed_task = osThreadNew(speed_task_function, (void *)0, &speed_task_attributes);

    if(speed_task == NULL)
    	while(1);
    
	// Create the direction update thread
    direction_task = osThreadNew(direction_task_function, (void *)0, &direction_task_attributes);

    if(direction_task == NULL)
    	while(1);

	// Create the lcd display thread
    lcd_display_task = osThreadNew(lcd_display_task_function, (void *)0, &lcd_display_task_attributes);

    if(lcd_display_task == NULL)
    	while(1);

	// Create the led output thread
    led_output_task = osThreadNew(led_output_task_function, (void *)0, &led_output_task_attributes);

    if(led_output_task == NULL)
    	while(1);

	// Create the vehicle control thread
    vehicle_control_task = osThreadNew(vehicle_control_task_function, (void *)0, &vehicle_control_task_attributes);

    if(vehicle_control_task == NULL)
    	while(1);

    
}

/**
  * @brief Enable button interrupts in NVIC - set priority to default priority
  * @retval None
  */
void APPLICATION_enable_button_interrupts(void)
{
        HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);
        HAL_NVIC_SetPriority(BUTTON_IRQ_NUMBER, EXTI0_DEFAULT_PRIORITY, EXTI0_DEFAULT_PRIORITY);
}

/**
  * @brief Read button pin and update application value
  * @retval None
  */
void APPLICATION_sample_button(void)
{
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        button_state = GPIO_PIN_SET;
    }
    else{
        button_state = GPIO_PIN_RESET;
    }
}

/**
  * @brief Sample gyroscope and record rotation rate in gyro_rotation variable
  * @retval None
  */
void APPLICATION_sample_gyro(void)
{
    int16_t gyro_velocity = Gyro_Get_Velocity();

    // Clockwise rotation
    if(gyro_velocity >= 0) 
    {
        if(gyro_velocity > 4500)
        {
            gyro_rotation = GYRO_FAST_CW_ROTATION; // Fast clockwise rotation
            gyro_position += gyro_velocity / 100; // Update gyro position

        }
        else if(gyro_velocity > 600)
        {
            gyro_rotation = GYRO_SLOW_CW_ROTATION; // Slow clockwise rotation
            gyro_position += gyro_velocity / 100; // Update gyro position

        }
        else 
        {
            gyro_rotation = GYRO_ZERO_ROTATION; // Zero rotation (idle)
        }

    }
    else // Counter-clockwise rotation
    { 
        if(gyro_velocity < -4500) // Fast counter-clockwise rotation
        {
            gyro_rotation = GYRO_FAST_CCW_ROTATION;
            gyro_position += gyro_velocity / 100; // Update gyro position

        }
        else if(gyro_velocity < -600)
        {
            gyro_rotation = GYRO_SLOW_CCW_ROTATION; // Slow counter-clockwise rotation
            gyro_position += gyro_velocity / 100; // Update gyro position

        }
        else
        {
            gyro_rotation = GYRO_ZERO_ROTATION; // Zero rotation (idle) 
        }
    }

}

void RunDemoForLCD(void)
{
	LCD_Clear(0,LCD_COLOR_WHITE);
	QuickDemo();
}


/**
  * @brief Turn on green led
  * @retval None
  */
void APPLICATION_activate_green_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
}

/**
  * @brief Turn on red led
  * @retval None
  */
void APPLICATION_activate_red_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
}

/**
  * @brief Turn off green led
  * @retval None
  */
void APPLICATION_deactivate_green_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
}

/**
  * @brief Turn off red led
  * @retval None
  */
void APPLICATION_deactivate_red_led(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}

/**
  * @brief Toggle green led
  * @retval None
  */
void APPLICATION_toggle_green_led(void)
{
    if(green_led_state == 1)
    {
        green_led_state = 0;
        APPLICATION_deactivate_green_led();
    } else {
        green_led_state = 1;
        APPLICATION_activate_green_led();
    }
}

/**
  * @brief Toggle red led
  * @retval None
  */
void APPLICATION_toggle_red_led(void)
{
    if(red_led_state == 1)
    {
        red_led_state = 0;
        APPLICATION_deactivate_red_led();
    } else {
        red_led_state = 1;
        APPLICATION_activate_red_led();
    }
}


// =================================================================================================
/* Thread function definitions */
//

void speed_task_function(void *arg)
{
	(void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;

	while(1)
	{
		// Wait until button is pressed
        status = osSemaphoreAcquire(speed_update_semaphore, osWaitForever);
        
        if(button_state == BUTTON_PRESSED)
        {
            // Start tracking how long the button is pressed - sample every 10 ms
            status = osTimerStart(button_timer, 10U);
        } 
        else if(button_state == BUTTON_NOT_PRESSED)
        {
            // Start tracking how long the button is pressed - sample every 10 ms
            status = osTimerStop(button_timer);

            status = osMutexAcquire(speed_data_mutex, osWaitForever);

            if(button_press_duration < 1000) // Button pressed for less than 1000 ms
            {
                speed_data.speed += SPEED_INCREMENT; // Increment speed if button is pressed briefly
                speed_data.numIncrements ++;

                [[maybe_unused]] uint32_t event_flags;
                event_flags = osEventFlagsSet(vehicle_control_event, SPEED_UPDATE_EVENT_FLAG_MSK);

            } else {
                if(speed_data.speed > 0)
                {
                    speed_data.speed -= SPEED_INCREMENT; // Decrement speed if button is held and released
                    speed_data.numDecrements ++;

                    [[maybe_unused]] uint32_t event_flags;
                    event_flags = osEventFlagsSet(vehicle_control_event, SPEED_UPDATE_EVENT_FLAG_MSK);
                }       
            }

            // Reset duration
            button_press_duration = 0;

            status = osMutexRelease(speed_data_mutex);
        }
        
	}
}

void direction_task_function(void *arg)
{
	(void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;
    [[maybe_unused]] uint32_t event_flags;

    uint8_t new_direction = 0;

	while(1)
	{
        APPLICATION_sample_gyro(); // Sample gyro

        // Straight
        if(gyro_position > INITIAL_DIRECTION_VALUE - 400 && gyro_position < INITIAL_DIRECTION_VALUE + 400)
        {
            new_direction = 0; // Vehicle is traveling straight
        } 
        // Slight left turn
        else if(gyro_position >= (INITIAL_DIRECTION_VALUE - 1000) - 600 && gyro_position <= (INITIAL_DIRECTION_VALUE - 1000) + 600) 
        {
            new_direction = 1; // Vehicle is slightly turning left
        }
        // Slight right turn
        else if(gyro_position >= (INITIAL_DIRECTION_VALUE + 1000) - 600 && gyro_position <= (INITIAL_DIRECTION_VALUE + 1000) + 600)
        {
            new_direction = 3; // Vehicle is slightly turning right
        }
        // Hard left turn
        else if(gyro_position < INITIAL_DIRECTION_VALUE - 1600)
        {
            new_direction = 2; // Vehicle is sharply turning left
        }
        // Hard right turn
        else if(gyro_position > INITIAL_DIRECTION_VALUE + 1600)
        {
            new_direction = 4; // Vehicle is sharply turning right
        }
        
        status = osMutexAcquire(direction_data_mutex, osWaitForever);
        direction_data.direction = new_direction;
        status = osMutexRelease(direction_data_mutex);

        event_flags = osEventFlagsSet(vehicle_control_event, DIRECTION_UPDATE_EVENT_FLAG_MSK);

		osDelay(GYRO_SAMPLE_RATE);
	}
}

void lcd_display_task_function(void *arg)
{
	(void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;

    uint8_t direction;
    uint32_t speed;

	while(1)
	{

        // Acquiring mutually exclusive resources
        status = osMutexAcquire(speed_data_mutex, osWaitForever);
        status = osMutexAcquire(direction_data_mutex, osWaitForever);

        direction = direction_data.direction;
        speed = speed_data.speed;

        // Release mutually exclusive resources
        status = osMutexRelease(speed_data_mutex);
        status = osMutexRelease(direction_data_mutex);

        LCD_Clear(0, LCD_COLOR_WHITE);
        LCD_SetTextColor(LCD_COLOR_BLACK);
	    LCD_SetFont(&Font12x12);

        LCD_DisplayString(10, 265, "Speed: ");
        LCD_DisplayNumber(100, 265, speed);
        LCD_DisplayString(140, 265, "mph");

        /* -- Direction indicator -- */

        LCD_DisplayString(10, 290, "Dir: ");

        if(direction == 0)
            LCD_DisplayString(100, 290, "Straight"); 

        if(direction == 1)
            LCD_DisplayString(100, 290, "Slight Left"); 

        if(direction == 2)
            LCD_DisplayString(100, 290, "Hard Left"); 

        if(direction == 3)
            LCD_DisplayString(100, 290, "Slight Right"); 

        if(direction == 4)
            LCD_DisplayString(100, 290, "Hard Right"); 
        
        /* -- Direction line -- */

        // Moving straight
        if(direction == 0)
            LCD_Draw_Thick_Line(120, 210, 120, 110, LCD_COLOR_GREEN);

        // Turning slightly left
        if(direction == 1)
            LCD_Draw_Thick_Line(120, 210, 50, 150, LCD_COLOR_GREEN);

        // Turning slightly right
        if(direction == 3)
            LCD_Draw_Thick_Line(120, 210, 190, 150, LCD_COLOR_GREEN);

        // Turning hard left
        if(direction == 2)
            LCD_Draw_Thick_Line(120, 210, 20, 210, LCD_COLOR_GREEN);
        
        // Turning hard right
        if(direction == 4)
            LCD_Draw_Thick_Line(120, 210, 220, 210, LCD_COLOR_GREEN);

		osDelay(LCD_UPDATE_RATE);

	}
}

void led_output_task_function(void *arg)
{
	(void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;
    [[maybe_unused]] uint32_t event_flags;

	while(1)
	{
		// Wait for an LED update 
		event_flags = osEventFlagsWait(led_output_event, 0x000F, osFlagsWaitAny, osWaitForever);

        if(event_flags & ENABLE_RED_LED_EVENT)
        {
            APPLICATION_activate_red_led();
        } 
        else if(event_flags & DISABLE_RED_LED_EVENT)
        {
            APPLICATION_deactivate_red_led();
        }

        if(event_flags & ENABLE_GREEN_LED_EVENT)
        {
            APPLICATION_activate_green_led();
        }
        else if(event_flags & DISABLE_GREEN_LED_EVENT)
        {
            APPLICATION_deactivate_green_led();
        }
	}
}

// TODO - speed warning if turning
void vehicle_control_task_function(void *arg)
{
	(void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;
    [[maybe_unused]] uint32_t event_flags;

    uint32_t speed;
    uint8_t direction;

	while(1)
	{
        // Wait for either a speed or direction update event flag to be raised
		event_flags = osEventFlagsWait(vehicle_control_event, 0x0003, osFlagsWaitAny, osWaitForever);

        // Acquiring mutually exclusive data
        status = osMutexAcquire(speed_data_mutex, osWaitForever);
        status = osMutexAcquire(direction_data_mutex, osWaitForever);

        speed = speed_data.speed;
        direction = direction_data.direction;

        status = osMutexRelease(speed_data_mutex);
        status = osMutexRelease(direction_data_mutex);

        if(direction != 0)
        {
            // If the timer is not running
            if(osTimerIsRunning(direction_timer) == 0)
            {
                osTimerStart(direction_timer, 10U);
            }

            // Turn duration is too long
            if(turn_duration > 5000)
            {
                event_flags = osEventFlagsSet(led_output_event, ENABLE_RED_LED_EVENT); // Enable direction warning light
            }
        }
        else if(direction == 0)
        {
            // Stop the running timer if the vehicle's direction is straight
            if(osTimerIsRunning(direction_timer) == 1)
            {
                osTimerStop(direction_timer);
                event_flags = osEventFlagsSet(led_output_event, DISABLE_RED_LED_EVENT); // Disable direction warning light
                turn_duration = 0; // Reset count
            }
        }
        
        // Update speed warning if needed
        if((direction == 0 && speed > 75) || (direction != 0 && speed > 45))
        {
            event_flags = osEventFlagsSet(led_output_event, ENABLE_GREEN_LED_EVENT);
        } else {
            event_flags = osEventFlagsSet(led_output_event, DISABLE_GREEN_LED_EVENT);
        }

        
	}
}

void button_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    
    *((int*) arg) += 10; // Increment button press duration by 10 ms
}

void direction_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    
    *((int*) arg) += 10; // Increment turn duration by 10 ms
}

/**
  * @brief EXTI0 interrupt handler - 
  * 
  * @retval None
  */
void EXTI0_IRQHandler()
{
    HAL_NVIC_DisableIRQ(BUTTON_IRQ_NUMBER);

    [[maybe_unused]] osStatus_t status;

    // Update button state by reading external button pin
    APPLICATION_sample_button();


    // Release semaphore token
    status = osSemaphoreRelease(speed_update_semaphore);

    // Clear button's pending interrupt
    EXTI_HandleTypeDef button;
    button.Line = 0;
    HAL_EXTI_ClearPending(&button, EXTI_TRIGGER_RISING_FALLING);

    HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);
}
