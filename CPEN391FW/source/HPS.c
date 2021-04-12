/*
 * HSP.c
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description
 * Board Support Package (BSP) of Cyclone V SoC HPS.
 */


/* Standard headers */
#include <stdio.h>
 
#include "TypeDef.h"
#include "HPS.h"


/*------------------- Constants Define ---------------------------------------*/

#define SWITCHES    (volatile unsigned int *)(0xFF200000)
#define PUSHBUTTONS (volatile unsigned int *)(0xFF200010)

#define LEDS        (volatile unsigned int *)(0xFF200020)
#define HEX0_1      (volatile unsigned int *)(0xFF200030)
#define HEX2_3      (volatile unsigned int *)(0xFF200040)
#define HEX4_5      (volatile unsigned int *)(0xFF200050)

/*------------------- Type Define --------------------------------------------*/

/*------------------- Local Data ---------------------------------------------*/
volatile uint32 *Ptimer      = (uint32 *)0xFFFEC600;
volatile uint32 *PtimerCount = (uint32 *)0xFFFEC604;

static int buttonsOld = 0;


/*------------------- Global Data --------------------------------------------*/


/*------------------- Local Function Prototype -------------------------------*/


/*------------------- Local Function -----------------------------------------*/

void config_HPS_timer( void );
void config_GIC(void);

/*------------------- External Global Function -------------------------------*/


/*------------------- Global Function ----------------------------------------*/

void HPS_Init( void )
{
    // LEDG on.
    *GPIO1_DDR |= 0x1000000;
    *GPIO1_DR  |= 0x1000000;

    // 200MHz private timer Initialization.
    *(Ptimer + 2) = 0;
    *(Ptimer) = 200000000;
    *(Ptimer + 1) = 0;
    *(Ptimer + 2) = 0x3;
    
    buttonsOld = *PUSHBUTTONS;
    
}

void HPS_Process( void )
{
    int switches;
    int buttons;

    // Process switches
    switches = *SWITCHES ;
    *LEDS = switches;
    *HEX0_1 = switches;
    *HEX2_3 = switches;
    *HEX4_5 = switches;

    //printf("Switches = %x\n", switches) ;
    
    buttons = *PUSHBUTTONS;
    if ( buttons != buttonsOld )
    {
        buttonsOld = buttons;
        printf("Buttons = %x\n", buttons );
    }
}

// Elapsed time in us.
bool HPS_ElapsedUs( uint32 start, uint32 TimeUs )
{
    uint32 Count;
    
    Count = *PtimerCount;
    
    if ( start >= Count )
    {
        if (  start >= ( Count + 200 * TimeUs ) )
        {
            return true;
        }
    }
    else
    {
        if ( ( start + ( 200000000 - Count ) ) >= 200 * TimeUs )
        {
            return true;
        }
    }
    
    return false;
}

void HPS_ToggleLedG( void )
{
    *GPIO1_DR ^= 0x1000000;
}



/**************************************************************************
** Delay for amount of time given by time interval time_us in ms.
**
** Parameters
** time_us - Time interval in ms.
**
***************************************************************************/
void HPS_msDelay( unsigned int time_ms )
{
    uint32 start = *PtimerCount;
    
    while ( !HPS_ElapsedUs( start, 1000 * time_ms ) );
}

/**************************************************************************
** Delay for amount of time given by time interval time_us in us.
**
** Parameters
** time_us - Time interval in us.
**
***************************************************************************/
void HPS_usDelay( unsigned int time_us )
{
    uint32 start = *PtimerCount;
    
    while ( !HPS_ElapsedUs( start, time_us ) );
}

/**************************************************************************
** Sleep for amount of time given by time interval time_us in us.
**
** Parameters
** time_us - Time interval in us.
**
***************************************************************************/
void HPS_usleep( unsigned int time_us )
{
    int data;
    unsigned int i, j;
    
    for ( i = 0; i < time_us; i++ )
    {
    	// CPU 200 MHz
    	for ( j = 0; j < 200; j++ )
    	{
    		data >> 1;
    	}
    }
}