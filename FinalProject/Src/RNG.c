#include "RNG.h"

/**
 * @brief Enables RNG peripheral
*/
void RNG_enable()
{
    RNG_enable_clock();
    RNG->CR |= (RNG_CR_RNGEN);
}

/**
 * @brief Disable RNG peripheral
*/
void RNG_disable()
{
    RNG->CR &= ~(RNG_CR_RNGEN);
    RNG_disable_clock();
}

/**
 * @brief Reset RNG peripheral value
*/
void RNG_reset()
{
    RNG_disable();
    RNG_enable();
}

/**
 * @brief Enable clock for RNG peripheral
*/
void RNG_enable_clock()
{
    RCC->AHB2ENR |= (RCC_AHB2ENR_RNGEN);
}

/**
 * @brief Disable clock for RNG peripheral
*/
void RNG_disable_clock()
{
    RCC->AHB2ENR &= ~(RCC_AHB2ENR_RNGEN);
}

/**
 * @brief Get random number from RNG peripheral
 * 
 * @param max - max value that can be returned
 * 
 * @return uint32_t random value from 0 to the max value
*/
uint32_t RNG_get_random_number(uint32_t max)
{
    return RNG->DR % max;
}