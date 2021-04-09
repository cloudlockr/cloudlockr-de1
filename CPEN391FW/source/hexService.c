/*
 * hexService.c
 *
 * This module contains the function for generating random HEX display.
 * It is meant to be called from the main ARM controller.
 *
 *  Created on: April 1, 2021
 *      Author: dannsy
 */

#include <stdlib.h>
#include "memAddress.h"
#include "hexService.h"

/**
 * Function to generate random HEX display and saves the display value to memory
 * Call srand(current time) at the start of the controller main loop.
 */
void generate_display_hex_code(void)
{
    unsigned random0 = rand() % 256;
    unsigned random1 = rand() % 256;
    unsigned random2 = rand() % 256;

    *HEX0_1 = random0;
    *HEX2_3 = random1;
    *HEX4_5 = random2;

    unsigned concat_random = (random2 << 16) + (random1 << 8) + random0;
    *HEX_ADDR = (unsigned)0xffffffff;
    *HEX_ADDR = concat_random;
}

/**
 * Function to set HEX display back to 0.
 */
void reset_hex(void)
{
    *HEX0_1 = (unsigned)0;
    *HEX2_3 = (unsigned)0;
    *HEX4_5 = (unsigned)0;
}

/**
 * Sample usage of the random HEX display generator function
 */
// #include <stdio.h>
// #include <time.h>

// int test()
// {
//     time_t t;

//     // Set seed as current time, DO THIS BEFORE MAIN CONTROLLER LOOP STARTS
//     srand((unsigned)time(&t));

//     // Setting HEX 3 times (it will be so fast that you likely won't see it because we reset the HEX right after)
//     int verification_code0 = random_HEX();
//     int verification_code1 = random_HEX();
//     int verification_code2 = random_HEX();

//     printf("Verification code 0 is: %x\n", verification_code0);
//     printf("Verification code 1 is: %x\n", verification_code1);
//     printf("Verification code 2 is: %x\n", verification_code2);

//     // Resetting HEX back to 0
//     reset_HEX();

//     return 0;
// }
