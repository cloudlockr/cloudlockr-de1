/**
 * This module contains a sleep function for the HPS
 */

#include "TypeDef.h"
#include "hpsService.h"

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
