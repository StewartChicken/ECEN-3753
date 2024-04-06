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

    // Set initial disruptor energy level to 100 %
    disruptor_energy_level = 100;


    // =================================================================================================
    /* Thread initialization */
    //

    // Create the button thread
    button_task = osThreadNew(button_task_function, (void *)0, &button_task_attributes);

    if(button_task == NULL)
    	while(1);


	// Create the disruptor thread
    disruptor_task = osThreadNew(disruptor_task_function, (void *)0, &disruptor_task_attributes);

    if(disruptor_task == NULL)
    	while(1);


    // Create the lcd display thread
    lcd_display_task = osThreadNew(lcd_display_task_function, (void *)0, &lcd_display_task_attributes);

    if(lcd_display_task == NULL)
    	while(1);


    // Create the led output thread
    led_output_task = osThreadNew(led_output_task_function, (void *)0, &led_output_task_attributes);

    if(led_output_task == NULL)
        while(1);

    // Create drone velocity thread
    gyro_angle_task = osThreadNew(gyro_angle_task_function, (void *)0, &gyro_angle_task_attributes);

    if(gyro_angle_task == NULL)
        while(1);


    // =================================================================================================
    /* Depletion timer initialization */
    //

    depletion_timer = osTimerNew(depletion_timer_callback, osTimerPeriodic, (void *)0, &depletion_timer_attributes);

    if(depletion_timer == NULL)
        while(1);

    // =================================================================================================
    /* Recharge timer initialization */
    //

    recharge_timer = osTimerNew(recharge_timer_callback, osTimerPeriodic, (void *)0, &recharge_timer_attributes);

    if(recharge_timer == NULL)
        while(1);

    // This timer always runs
    init_status = osTimerStart(recharge_timer, DISRUPTOR_ENERGY_RECHARGE_INTERVAL);


    // =================================================================================================
    /* Green led pwm timer initialization */
    //

    green_led_pwm_timer = osTimerNew(green_led_pwm_timer_callback, osTimerPeriodic, (void *)0, &green_led_pwm_timer_attributes);

    if(green_led_pwm_timer == NULL)
        while(1);

    init_status = osTimerStart(green_led_pwm_timer, 1U);

    // =================================================================================================
    /* Speed update semaphore initialization */
    //

    button_semaphore = osSemaphoreNew(1, 0, &button_semaphore_attributes);

    if(button_semaphore == NULL)
        while(1);

    // =================================================================================================
    /* Disruptor energy level mutex initialization */
    //

    energy_level_mutex = osMutexNew(&energy_level_mutex_attributes);

    if(energy_level_mutex == NULL)
        while(1);


    // =================================================================================================
    /* Gyro angle data mutex initialization */
    //

    gyro_angle_mutex = osMutexNew(&gyro_angle_mutex_attributes);

    if(gyro_angle_mutex == NULL)
        while(1);


    // =================================================================================================
    /* Disruptor event flag group initialization */
    //

    disruptor_event = osEventFlagsNew(&disruptor_event_attributes);

    if(disruptor_event == NULL)
        while(1);


    // =================================================================================================
    /* Led event flag group initialization */
    //

    led_event = osEventFlagsNew(&led_event_attributes);

    if(led_event == NULL)
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
    gyro_velocity_x = Gyro_Get_Velocity_X(); 
    gyro_velocity_y = Gyro_Get_Velocity_Y();

    // Obtain angular velocity in mdps
    double angular_velocity_x = ((double)gyro_velocity_x) * 0.07; 
    double angular_velocity_y = ((double)gyro_velocity_y) * 0.07;
    
    gyro_angle_x += (int)(angular_velocity_x * 0.01);
    gyro_angle_y += (int)(angular_velocity_y * 0.01);
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

void disruptor_task_function(void *arg)
{
	(void) &arg; // Remove warnings
	[[maybe_unused]] osStatus_t status;
	uint32_t flags;

	while(1)
	{
		
		flags = osEventFlagsWait(disruptor_event, 0x0003, osFlagsWaitAny, osWaitForever); // Wait for disruptor event to occur

		status = osMutexAcquire(energy_level_mutex, osWaitForever); // Wait for energy level data to be available

		if(flags & DISRUPTOR_ENERGY_DEPLETE_EVENT && disruptor_energy_level > 0)
		{
			disruptor_energy_level -= DISRUPTOR_ENERGY_DEPLETION_RATE;
		}
		else if(flags & DISRUPTOR_ENERGY_RECHARGE_EVENT && disruptor_energy_level < 100)
		{
			disruptor_energy_level += DISRUPTOR_ENERGY_RECHARGE_RATE;
		}

		status = osMutexRelease(energy_level_mutex);

	}
}

void button_task_function(void *arg)
{
	(void) &arg; // Remove warnings
	[[maybe_unused]] osStatus_t status;

	while(1)
	{
		status = osSemaphoreAcquire(button_semaphore, osWaitForever); // Wait for button press

		if(button_state == BUTTON_PRESSED)
        {
            // Start depleting energy level while button is pressed
            status = osTimerStart(depletion_timer, DISRUPTOR_ENERGY_DEPLETION_INTERVAL);
        } 
        else if(button_state == BUTTON_NOT_PRESSED)
        {
            status = osTimerStop(depletion_timer);
        }
	}

}


void lcd_display_task_function(void *arg)
{
	(void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;

    uint8_t energy_level;
    
	while(1)
	{

        // Acquiring mutually exclusive resources
        status = osMutexAcquire(energy_level_mutex, osWaitForever);
        
        energy_level = disruptor_energy_level;
        
        status = osMutexRelease(energy_level_mutex);

        LCD_Clear(0, LCD_COLOR_WHITE);
        LCD_SetTextColor(LCD_COLOR_BLACK);
	    LCD_SetFont(&Font12x12);

        LCD_DisplayNumber(100, 265, energy_level);

        status = osMutexAcquire(gyro_angle_mutex, osWaitForever);
        LCD_DisplayNumber(100, 240, gyro_angle_x);
        LCD_DisplayNumber(100, 215, gyro_angle_y);
        status = osMutexRelease(gyro_angle_mutex);

		osDelay(LCD_UPDATE_RATE);

	}
}

void led_output_task_function(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;
    uint32_t event_flags;

    while(1)
    {
        // Wait for an LED event 
        event_flags = osEventFlagsWait(led_event, 0x000F, osFlagsWaitAny, osWaitForever);

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

void gyro_angle_task_function(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;

    while(1)
    {
        status = osMutexAcquire(gyro_angle_mutex, osWaitForever);
        APPLICATION_sample_gyro();
        status = osMutexRelease(gyro_angle_mutex);

        osDelay(GYRO_SAMPLE_RATE);
    }
}

void depletion_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] uint32_t event_flags;
    
    event_flags = osEventFlagsSet(disruptor_event, DISRUPTOR_ENERGY_DEPLETE_EVENT);
    
}

void recharge_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] uint32_t event_flags;
    
    event_flags = osEventFlagsSet(disruptor_event, DISRUPTOR_ENERGY_RECHARGE_EVENT);
    
}

void green_led_pwm_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] uint32_t event_flags;

    green_led_timer_tick ++; // Increment timer tick

    // Reset cycle
    if(green_led_timer_tick == 9)
    {
        green_led_timer_tick = 0; // Reset tick
        event_flags = osEventFlagsSet(led_event, ENABLE_GREEN_LED_EVENT); // Enable green led
    } 
    else if(green_led_timer_tick == disruptor_energy_level / 10)
    {
        event_flags = osEventFlagsSet(led_event, DISABLE_GREEN_LED_EVENT); // Disable green led
    } 
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
    status = osSemaphoreRelease(button_semaphore);

    // Clear button's pending interrupt
    EXTI_HandleTypeDef button;
    button.Line = 0;
    HAL_EXTI_ClearPending(&button, EXTI_TRIGGER_RISING_FALLING);

    HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);
}
