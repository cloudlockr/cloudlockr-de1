/*
 * random_hex.c
 *
 * This module contains the function for generating random HEX display.
 * It is meant to be called from the main ARM controller.
 *
 *  Created on: April 1, 2021
 *      Author: dannsy
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

volatile unsigned *hex01 = (volatile unsigned *)0xFF200030;
volatile unsigned *hex23 = (volatile unsigned *)0xFF200040;
volatile unsigned *hex45 = (volatile unsigned *)0xFF200050;

/**
 * Function to generate random HEX display and returns the generated HEX as an int.
 * Call srand(current time) at the start of the controller main loop.
 */
int random_HEX()
{
    int random0 = rand() % 256;
    int random1 = rand() % 256;
    int random2 = rand() % 256;

    *hex01 = (unsigned)random0;
    *hex23 = (unsigned)random1;
    *hex45 = (unsigned)random2;

    int concat_random = (random2 << 16) + (random1 << 8) + random0;

    return concat_random;
}

/**
 * Function to set HEX display back to 0.
 */
void reset_HEX()
{
    *hex01 = (unsigned)0;
    *hex23 = (unsigned)0;
    *hex45 = (unsigned)0;
}

/**
 * Sample usage of the random HEX display generator function
 */
int main()
{
    time_t t;

    // Set seed as current time, DO THIS BEFORE MAIN CONTROLLER LOOP STARTS
    srand((unsigned)time(&t));

    // Setting HEX 3 times (it will be so fast that you likely won't see it because we reset the HEX right after)
    int verification_code0 = random_HEX();
    int verification_code1 = random_HEX();
    int verification_code2 = random_HEX();

    printf("Verification code 0 is: %x\n", verification_code0);
    printf("Verification code 1 is: %x\n", verification_code1);
    printf("Verification code 2 is: %x\n", verification_code2);

    // Resetting HEX back to 0
    reset_HEX();

    return 0;
}
