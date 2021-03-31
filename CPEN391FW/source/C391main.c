/*
 * C391main.c
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description
 * This file contains main function which is the entry point of CPEN391 firmware.
 */


/* Standard headers */
#include <stdio.h>

/* Application common headers */
#include "TypeDef.h"


/* Application module headers */
#include "HPS.h"
#include "UART.h"
#include "WIFI.h"
#include "Bluetooth.h"


#define SWITCHES    (volatile unsigned int *)(0xFF200000)
#define PUSHBUTTONS (volatile unsigned int *)(0xFF200010)

#define LEDS        (volatile unsigned int *)(0xFF200020)
#define HEX0_1      (volatile unsigned int *)(0xFF200030)
#define HEX2_3      (volatile unsigned int *)(0xFF200040)
#define HEX4_5      (volatile unsigned int *)(0xFF200050)


/*------------------- Local Function Prototype -------------------*/
static void AppInit( void );
static void mainLoop(void);


/*------------------- Local Function -------------------*/

/**************************************************************************
** Initialize all firmware modules.
**
***************************************************************************/
static void AppInit( void )
{
    // Initialize UART ports.
    UART_Init( UART_ePORT_WIFI );
    UART_Init( UART_ePORT_BLUETOOTH );
    
    // Initialize WIFI interface.
    //while ( WIFI_Init() == false)
    WIFI_Init();
    {
        // We need a sleep function.
        HPS_usleep(3 * 1000 * 1000); // 3 seconds
    }

}


static void mainLoop(void)
{
	int loopCount = 0;
	int ch;
    int switches;
    int buttons, buttonsOld;
    int loopCountOld = 0;
    

    // main routine/loop here
	while (1)
	{
		loopCount++;
        
        if ( ( loopCount >= loopCountOld ) && ( loopCount >= ( loopCountOld + 1500000 ) ) )
        {
            loopCountOld = loopCount;
            WIFI_Process();
            BLUETOOTH_Process();
        }

	    #if 1
        // Polling data from WIFI UART.
	    if ( UART_TestForReceivedData( UART_ePORT_WIFI ) )
	    {
	        ch = UART_getchar( UART_ePORT_WIFI );

	        //UART_putchar( UART_ePORT_WIFI, ch ); // send back whatever character received.

	        printf( "Wifi Received: %c", (char)ch );
	        printf( "- loopCount: %i\n", loopCount );
	    }
        else 
        #endif
        if ( UART_TestForReceivedData( UART_ePORT_BLUETOOTH ) )
        {
            ch = UART_getchar( UART_ePORT_BLUETOOTH );

	    UART_putchar( UART_ePORT_BLUETOOTH, ch ); // send back whatever character received.

	    printf( "DE1 received BT msg from frontend %c", (char)ch );
	    printf( "- loopCount: %i\n", loopCount );
		
	    BLUETOOTH_Receive( ch );	
        }
        
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
}


/*------------------- API Function -------------------*/

/**************************************************************************
** Main function of CPEN391 firmware.
** 
** Initialize all firmware modules and enter an infinite loop.
**
***************************************************************************/
int main(void)
{
	printf( ">>>>>>>>>    CPEN391 Firmware    <<<<<<<<<\n");

    AppInit();
    
    mainLoop();

	return 0;
}




