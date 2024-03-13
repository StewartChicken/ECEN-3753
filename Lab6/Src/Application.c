/*
 * Application.c
 *
 *  Created on: March 8, 2024
 *      Author: Evan Poon
 */

#include "Application.h"

/**
  * @brief Initialize application objects: 
  *         - Initialize gyro input, button input, and led output tasks
  *         - Initialize periodic timer to sample gyro data
  *         - Initialize semaphore to fascilitate communication between timer and gyro input task
  *         - Initialize EventFlagGroup to signal button press events
  *         - Initialize Message queue to communicate between inputs and LED output
  * 
  * @retval None
  */
void APPLICATION_init(void)
{

    [[maybe_unused]] static osStatus_t init_status;

    APPLICATION_enable_button_interrupts();


    // =================================================================================================
    /* Button pressed event flag setup */
    // 

    // Create the button pressed event flag
    button_pressed_event = osEventFlagsNew(&button_pressed_event_attributes);

    if(button_pressed_event == NULL)
        while(1);


    // =================================================================================================
    /* LED message queue setup */
    // 

    // Create the led message queue
    led_message_queue = osMessageQueueNew(LED_MESSAGE_QUEUE_SIZE, sizeof(LedMessage_t), NULL);

    if(led_message_queue == NULL)
        while(1);

    // Set led initial state
    LedMessage_t led_initial_state;
    led_initial_state.msg_button_state = BUTTON_NOT_PRESSED;
    led_initial_state.msg_gyro_rotation = GYRO_ZERO_ROTATION; 
    init_status = osMessageQueuePut(led_message_queue, &led_initial_state, 0, 0); 

    if(init_status != osOK)
        while(1);


    // =================================================================================================
    /* Gyroscope sampling timer setup */
    //

    gyro_semaphore = osSemaphoreNew(1, 1, &gyro_semaphore_attributes);

    if(gyro_semaphore == NULL)
        while(1);

    
    // Initially, the semaphore is acquired - base value is 0
    init_status = osSemaphoreAcquire(gyro_semaphore, 500U);

    if(init_status != osOK)
        while(1);


    // =================================================================================================
    /* Gyroscope sampling timer setup */
    //

    gyro_timer = osTimerNew(gyro_timer_callback, osTimerPeriodic, (void *)0, &gyro_timer_attributes);

    if(gyro_timer == NULL)
        while(1);

    init_status = osTimerStart(gyro_timer, GYRO_SAMPLE_RATE);

    if(init_status != osOK)
        while(1);


    // =================================================================================================
    /* Thread initialization */
    //

    // Create the button input thread
    button_input_task = osThreadNew(button_input_function, (void *)0, &button_input_task_attributes);

    if(button_input_task == NULL)
    	while(1);

    // Create the gyro input thread
    gyro_input_task = osThreadNew(gyro_input_function, (void *)0, &gyro_input_task_attributes);

    if(gyro_input_task == NULL)
        while(1);

    // Create the led output thread
    led_output_task = osThreadNew(led_output_function, (void *)0, &led_output_task_attributes);

    if(led_output_task == NULL)
    	while(1);
         

    // =================================================================================================

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
void APPLICATION_get_gyro_rotation_rate(void)
{
    int16_t gyro_velocity = Gyro_Get_Velocity();

    // Clockwise rotation
    if(gyro_velocity >= 0) 
    {
        if(gyro_velocity > 12000)
        {
            gyro_rotation = GYRO_FAST_CW_ROTATION; // Fast clockwise rotation
        }
        else if(gyro_velocity > 200)
        {
            gyro_rotation = GYRO_SLOW_CW_ROTATION; // Slow clockwise rotation
        }
        else 
        {
            gyro_rotation = GYRO_SLOW_CW_ROTATION; // Zero rotation (idle) - counted as clockwise rotation for this lab
        }

    }
    else // Counter-clockwise rotation
    { 
        if(gyro_velocity < -12000) // Fast counter-clockwise rotation
        {
            gyro_rotation = GYRO_FAST_CCW_ROTATION;
        }
        else if(gyro_velocity < -200)
        {
            gyro_rotation = GYRO_SLOW_CCW_ROTATION; // Slow counter-clockwise rotation
        }
        else
        {
            gyro_rotation = GYRO_SLOW_CW_ROTATION; // Zero rotation (idle) - counted as clockwise rotation for this lab
        }
    }
}


/**
  * @brief Return true if gyroscope is rotating clockwise
  * @retval bool
  */
bool APPLICATION_is_gyro_rotating_cw(void)
{
    return (gyro_rotation ==  GYRO_FAST_CW_ROTATION || gyro_rotation == GYRO_SLOW_CW_ROTATION);
}

/**
  * @brief Return true if gyroscope is rotating counter-clockwise
  * @retval bool
  */
bool APPLICATION_is_gyro_rotating_ccw(void)
{
    return (gyro_rotation ==  GYRO_FAST_CCW_ROTATION || gyro_rotation == GYRO_SLOW_CCW_ROTATION);
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

/**
  * @brief Button input task function - waits for the button pressed event flag to raise.
  *        Once flag is raised, sends message containing button state information to led message queue 
  * 
  * @param arg - pointer to argument array
  * @retval None
  */
void button_input_function(void *arg)
{
    (void) &arg; // Remove warnings

    [[maybe_unused]] uint32_t event_flags;
    [[maybe_unused]] LedMessage_t message;
    [[maybe_unused]] osStatus_t status;

    while(1)
    {
        // Wait for button press event flag to be raised
        event_flags = osEventFlagsWait(button_pressed_event, BUTTON_PRESSED_EVENT_MASK, osFlagsWaitAny, osWaitForever);

        // Send message to LED message queue
        message.msg_button_state = button_state;
        message.msg_gyro_rotation = -1; // Gyro rotation is unknown by button task
        status = osMessageQueuePut(led_message_queue, &message, 0, 0); 

        // Clear event after message is sent
        event_flags = osEventFlagsClear(button_pressed_event, BUTTON_PRESSED_EVENT_MASK); 

    }
}

/**
  * @brief Sample gyroscope whenever semaphore token is available (timer period)
  *        Once data is sampled, sends message containing gyro rotation state to led message queue 
  * 
  * @param arg - pointer to argument array
  * @retval None
  */
void gyro_input_function(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] LedMessage_t message;
    [[maybe_unused]] osStatus_t status;

    uint8_t rotation_direction; // 0 means clockwise, 1 means counterclockwise

    while(1)
    {
        // Check if gyro resource is available
        status = osSemaphoreAcquire(gyro_semaphore, osWaitForever);

        // Sample gyro
        APPLICATION_get_gyro_rotation_rate();

        if(APPLICATION_is_gyro_rotating_cw())
        {
            rotation_direction = 0;
        }

        if(APPLICATION_is_gyro_rotating_ccw())
        {
            rotation_direction = 1;
        }

        message.msg_button_state = -1; // Button state is unknown by gyro task
        message.msg_gyro_rotation = rotation_direction; 
        status = osMessageQueuePut(led_message_queue, &message, 0, 0);

        // Yield after message is sent
        status = osThreadYield();
    }
}

/**
  * @brief Control led state - reads data from message queue to determine when to activate/deactivate
  *        specified leds. Receives data from gyro output and button output tasks. 
  * 
  * @param arg - pointer to argument arrat
  * @retval None
  */
void led_output_function(void *arg)
{
    (void) &arg; // Remove warnings

    [[maybe_unused]] LedMessage_t message;
    [[maybe_unused]] osStatus_t status;

    [[maybe_unused]] uint8_t task_button_state = 0; // 0 if not pressed, 1 if pressed
    [[maybe_unused]] uint8_t task_gyro_rotation_direction = 0; // 0 if clockwise, 1 if counterclockwise - no movement is clockwise

    while(1)
    {
        // Get message from message queue
        status = osMessageQueueGet(led_message_queue, &message, 0, osWaitForever);

        // If there is a message present, update the local button and gyro states
        if(status == osOK)
        {
            if(message.msg_button_state != -1) // If message contains information about the state of the button
            {
                task_button_state = message.msg_button_state;
            } 
            else if(message.msg_gyro_rotation != -1) { // If message contains information about the state of the gyro
                task_gyro_rotation_direction = message.msg_gyro_rotation; 
            } 
            else 
            { // Error - this should never happen
                // Error
            }

            // Clear button and gyro states
            message.msg_button_state = 0;
            message.msg_gyro_rotation = 0;
        }

        // Turn on green LED if the button is pressed or if the gyros rotating counter-clockwise
        if(task_button_state == BUTTON_PRESSED || task_gyro_rotation_direction == 1)
        {
            APPLICATION_activate_green_led();
        } 
        else 
        {
            APPLICATION_deactivate_green_led();
        }

        // Turn on red LED if the button is pressed *and* if the gyro is rotating clockwise
        if(task_button_state == BUTTON_PRESSED && task_gyro_rotation_direction == 0)
        {
            APPLICATION_activate_red_led();
        }
        else 
        {
            APPLICATION_deactivate_red_led();
        }
    }
}

/**
  * @brief Gyro timer callback function - Releases a semaphore periodically which 
  *        prompts the gyro input task to sample the gyroscope and send data to the led output task
  * 
  * @param arg - pointer to argument array
  * @retval None
  */
void gyro_timer_callback(void *arg)
{
    (void) &arg; // Remove warnings
    [[maybe_unused]] osStatus_t status;
    
    // Release semaphore token
    status = osSemaphoreRelease(gyro_semaphore);
}

/**
  * @brief EXTI0 interrupt handler - triggered whenever the external button is pressed
  *        Samples state of the button and updates the variable indicating whether or not it is pressed
  * 
  * @retval None
  */
void EXTI0_IRQHandler()
{
    HAL_NVIC_DisableIRQ(BUTTON_IRQ_NUMBER);

    // Update button state by reading external button pin
    APPLICATION_sample_button();

    [[maybe_unused]] uint32_t flags;
    flags = osEventFlagsSet(button_pressed_event, BUTTON_PRESSED_EVENT_MASK);

    // Clear button's pending interrupt
    EXTI_HandleTypeDef button;
    button.Line = 0;
    HAL_EXTI_ClearPending(&button, EXTI_TRIGGER_RISING_FALLING);

    HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);
}
