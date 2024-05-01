/*
 * This is code interfaces with the random number generator on the STM32 board.
 * All it is responsible for is generating a random initial position and random
 * initial velocity for the ball. 
*/

#ifndef RNG_H
#define RNG_H

#include <stdint.h>
#include "stm32f429xx.h" // RNG is access macro

void RNG_enable();
void RNG_disable();
void RNG_reset();
void RNG_enable_clock();
void RNG_disable_clock();

uint32_t RNG_get_random_number(uint32_t max); // Get a 32 bit random number from the RNG register - 'max' is max value

#endif