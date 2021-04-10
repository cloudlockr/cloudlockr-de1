/*
 * This module contains the function for generating random HEX display and clearing it
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
