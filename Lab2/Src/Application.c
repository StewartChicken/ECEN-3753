#include "Application.h"

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

void APPLICATION_get_gyro_rotation_rate(void)
{
    int16_t gyro_velocity = Gyro_Get_Velocity();

    // Counter clockwise rotation
    if(gyro_velocity >= 0) 
    {
        if(gyro_velocity > 12000)
        {
            gyro_rotation = GYRO_FAST_CCW_ROTATION; // Fast counter clockwise rotation
        }
        else if(gyro_velocity > 200)
        {
            gyro_rotation = GYRO_SLOW_CCW_ROTATION; // Slow counter clockwise rotation
        }
        else 
        {
            gyro_rotation = GYRO_SLOW_CW_ROTATION; // Zero rotation (idle) - counted as clockwise rotation for this lab
        }
    }
    else // Clockwise rotation
    { 
        if(gyro_velocity < -12000) // Fast clockwise rotation
        {
            gyro_rotation = GYRO_FAST_CW_ROTATION;
        }
        else if(gyro_velocity < -200)
        {
            gyro_rotation = GYRO_SLOW_CW_ROTATION; // Slow clockwise rotation
        }
        else
        {
            gyro_rotation = GYRO_SLOW_CW_ROTATION; // Zero rotation (idle) - counted as clockwise rotation for this lab
        }
    }
}

// Checks if gyro is rotating clockwise/counter-clockwise
bool APPLICATION_is_gyro_rotating_cw(void)
{
    return (gyro_rotation ==  GYRO_FAST_CW_ROTATION || gyro_rotation == GYRO_SLOW_CW_ROTATION);
}

bool APPLICATION_is_gyro_rotating_ccw(void)
{
    return (gyro_rotation ==  GYRO_FAST_CCW_ROTATION || gyro_rotation == GYRO_SLOW_CCW_ROTATION);
}

void APPLICATION_update_led(void)
{
    // Turn on green LED if the button is pressed or if the gyros rotating counter-clockwise
    if(button_state == BUTTON_PRESSED || APPLICATION_is_gyro_rotating_ccw()) 
    {
        APPLICATION_activate_green_led();
    }
    else
    {
        APPLICATION_deactivate_green_led();
    }

    // Turn on redd LED if the button is pressed *and* if the gyro is rotating clockwise
    if(button_state == BUTTON_PRESSED && APPLICATION_is_gyro_rotating_cw())
    {
        APPLICATION_activate_red_led();
    }
    else
    {
        APPLICATION_deactivate_red_led();
    }
    
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

// Exception handlers

void HAL_SYSTICK_Callback(void)
{

#if INTERRUPT_MODE == 1
    ms_passed ++; // Called every one millisecond (systick interrupt)

    if(ms_passed == 99)
    {
        ms_passed = 0;

        // Sample button/gyro rotation
        APPLICATION_get_gyro_rotation_rate();

        // Update LED states
        APPLICATION_update_led();
    }
#endif
}

void EXTI0_IRQHandler()
{
	HAL_NVIC_DisableIRQ(BUTTON_IRQ_NUMBER);

	APPLICATION_sample_button();

	// Clear button's pending interrupt
    EXTI_HandleTypeDef button;
    button.Line = 0;

    HAL_EXTI_ClearPending(&button, EXTI_TRIGGER_RISING_FALLING);

	HAL_NVIC_EnableIRQ(BUTTON_IRQ_NUMBER);

}
