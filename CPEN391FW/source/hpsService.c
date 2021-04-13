/**
 * This module contains a sleep function for the HPS
 */

#include <stdio.h>
#include <typeDef.h>
#include "memAddress.h"
#include "hpsService.h"

// Local functions
void config_hps_timer(void);
void config_gic(void);

// Global variables
volatile uint32 *Ptimer = (uint32 *)0xFFFEC600;
volatile uint32 *PtimerCount = (uint32 *)0xFFFEC604;
static int buttonsOld = 0;

/**
 * Initialize HPS modules. 
 * 
 */
void hps_init(void)
{
    // LEDG on.
    *GPIO1_DDR |= 0x1000000;
    *GPIO1_DR |= 0x1000000;

    // 200MHz private timer Initialization.
    *(Ptimer + 2) = 0;
    *(Ptimer) = 200000000;
    *(Ptimer + 1) = 0;
    *(Ptimer + 2) = 0x3;

    buttonsOld = *PUSHBUTTONS;
}

/**
 * Process the use of the switches, LEDS, and HEX display.
 * 
 */
void hps_process(void)
{
    int switches;
    int buttons;

    // Process switches
    switches = *SWITCHES;
    *LEDS = switches;
    *HEX0_1 = switches;
    *HEX2_3 = switches;
    *HEX4_5 = switches;

    //printf("Switches = %x\n", switches) ;

    buttons = *PUSHBUTTONS;
    if (buttons != buttonsOld)
    {
        buttonsOld = buttons;
        printf("Buttons = %x\n", buttons);
    }
}

// Elapsed time in us.
bool hps_elapsed_us(uint32 start, uint32 TimeUs)
{
    uint32 Count;

    Count = *PtimerCount;

    if (start >= Count)
    {
        if (start >= (Count + 200 * TimeUs))
        {
            return true;
        }
    }
    else
    {
        if ((start + (200000000 - Count)) >= 200 * TimeUs)
        {
            return true;
        }
    }

    return false;
}

void hps_toggle_ledg(void)
{
    *GPIO1_DR ^= 0x1000000;
}

/**
 * Delay for amount of time given by time interval time_us in ms.
 * Uses accurate private timer (ARM core 0).
 *
 * Params:
 *  time_us     Time interval in ms.
 */
void hps_ms_delay(unsigned int time_ms)
{
    uint32 start = *PtimerCount;

    while (!hps_elapsed_us(start, 1000 * time_ms))
    {
        // Busy wait
    }
}

/**
 * Delay for amount of time given by time interval time_us in us.
 * Uses accurate private timer (ARM core 0).
 * 
 * Params:
 *  time_us     Time interval in us.
 */
void hps_us_delay(unsigned int time_us)
{
    uint32 start = *PtimerCount;

    while (!hps_elapsed_us(start, time_us))
    {
        // Busy wait
    }
}

/**
 * Sleep for amount of time given by time interval time_us in us.
 * 
 * Params:
 *  time_us     int specifying the time interval to sleep for in us
 */
void hps_usleep(unsigned int time_us)
{
    int data;
    unsigned int i, j;

    for (i = 0; i < time_us; i++)
    {
        // CPU 200 MHz
        for (j = 0; j < 200; j++)
        {
            data >> 1;
        }
    }
}
