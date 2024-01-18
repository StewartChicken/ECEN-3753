#include "Application_Code.h"

// Initialize GPIO ports and pins for the button and LEDs
void APPLICATION_init(void)
{
    GPIO_InitTypeDef Button_init;
    GPIO_InitTypeDef LED_init;
    
    // Button pin config
    Button_init.Pin = BUTTON_PIN;
    Button_init.Mode = GPIO_MODE_INPUT; // Input mode
    Button_init.Pull = GPIO_NOPULL;
    Button_init.Speed = GPIO_SPEED_FREQ_MEDIUM;

    // LED pin config
    LED_init.Pin = RED_LED_PIN | GREEN_LED_PIN;
    LED_init.Mode = GPIO_MODE_OUTPUT_PP; // Output push/pull mode
    LED_init.Pull = GPIO_NOPULL; // No pull
    LED_init.Speed = GPIO_SPEED_FREQ_MEDIUM;

    // Enable clock for both LED and Button ports
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Initialize button
    HAL_GPIO_Init(BUTTON_PORT, &Button_init);

    // Initialize LEDs
    HAL_GPIO_Init(LED_PORT, &LED_init);
}

// Toggle the red led then delay for 1 second
void APPLICATION_blink_led(void)
{
    HAL_GPIO_TogglePin(LED_PORT, RED_LED_PIN);

    // Delay for 1 second
    HAL_Delay(1000);
}

// Read button state and turns on green led if button is pressed
void APPLICATION_get_button_state(void)
{
    if(HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == BUTTON_PRESSED)
    {
        HAL_GPIO_WritePin(LED_PORT, GREEN_LED_PIN, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LED_PORT, GREEN_LED_PIN, GPIO_PIN_RESET);
    }
}
