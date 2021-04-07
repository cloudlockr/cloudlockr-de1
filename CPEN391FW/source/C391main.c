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
#include "JsonParser.h"

#define SWITCHES    (volatile unsigned int *)(0xFF200000)
#define PUSHBUTTONS (volatile unsigned int *)(0xFF200010)

#define LEDS        (volatile unsigned int *)(0xFF200020)
#define HEX0_1      (volatile unsigned int *)(0xFF200030)
#define HEX2_3      (volatile unsigned int *)(0xFF200040)
#define HEX4_5      (volatile unsigned int *)(0xFF200050)


/*------------------- Local Function Prototype -------------------*/
static void init( void );
static void controller( void );

/*------------------- Local Function -------------------*/

/**************************************************************************
** Initialize all firmware modules.
**
***************************************************************************/
static void init( void )
{
    // Initialize UART ports.
    UART_Init( UART_ePORT_WIFI );
    UART_Init( UART_ePORT_BLUETOOTH );
    
    // Initialize WIFI interface.
    //while ( WIFI_Init() == false)
    //WIFI_Init();
    //{
        // We need a sleep function.
    //    HPS_usleep(3 * 1000 * 1000); // 3 seconds
    //}

}

static void controller( void )
{
	while (1)
	{
		// Wait for a request message to be received
		char* jsonString = bluetooth_wait_for_data();
		jsmntok_t* jsonTokens = str_to_json(jsonString);

		// Check for JSON parsing errors
		if ( jsonTokens == NULL )
		{
			// TODO: send error response
			continue;
		}

		// Get the message type
		jsmntok_t firstToken = jsonTokens[1];
		char value[2];
		memcpy(value, &jsonString + firstToken.start, 1);
		value[1] = 0;

		long messageType = strtol(value, NULL, 10);

		// Direct the request to the appropriate handler function (pure functions that take inputs, not the JSON tokens)
		switch (messageType)
		{
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			default:
				break; // TODO: send error response due to invalid message type
		}

		// TODO: remove this (only for testing purposes, should actually respond in the controller)
		//HPS_usleep(3 * 1000 * 10000);
		//UART_puts( UART_ePORT_BLUETOOTH, "{\"status\":1}\v\n" );
		/////////////////////
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
    init();
    controller();
	return 0;
}




