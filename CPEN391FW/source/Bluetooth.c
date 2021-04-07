/*
 * Bluetooth.c
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description:
 * This module implements bluetooth related functionalities.
 */

#include <stdio.h>
#include <string.h>

#include "TypeDef.h"
#include "HPS.h"
#include "UART.h"
#include "Bluetooth.h"


#define BUFFER_SIZE 128     // 128 is enough?
static int  bluetooth_count = 0;
static int  fragment_count = 0;
static char bluetooth_data[BUFFER_SIZE];
static char last_two_data[2] = "";

typedef enum
{
    STATE_PARSE_MSG = 0,
    STATE_MSG_VALUE,  // may not be needed
    STATE_JSON_PARSE,
} STATE_T;


void BLUETOOTH_Receive( char ch )
{
    bool fullMessageReceived = false;
    bool fragmentReceived = false;
    int status = 1;

    // Process passed char (if buffer space is available)
	if ( bluetooth_count < BUFFER_SIZE )
   	{
			// Save only valid chars to the bluetooth_data buffer
			if ( ch != '\n' && ch != '\r' && ch != '\v')
			{
				bluetooth_data[bluetooth_count] = ch;
				bluetooth_count++;
				fragment_count++;
			}

			last_two_data[0] = last_two_data[1];
			last_two_data[1] = ch;
            
			// Check message end conditions
            if ( last_two_data[0] == '\v' && last_two_data[1] == '\n' )
            {
            	fullMessageReceived = true;
            }
            else if ( last_two_data[0] == '\r' && last_two_data[1] == '\n' )
            {
            	fragmentReceived = true;
            }
   	}
    else if ( bluetooth_count >= BUFFER_SIZE )
    {   
    	// Mark a fragmentation error
    	fullMessageReceived = true;
    	status = 3;

    	// Move the buffer cursor back to "erase" collected fragment data
    	bluetooth_count -= fragment_count;
    }

	// Respond to received full messages (either full or a fragment)
	if ( fragmentReceived )
	{
		fragment_count = 0;

		if ( status == 1 )
			// Assign the correct OK status
			status = 2;

		char res_buffer[18];
		snprintf(res_buffer, sizeof(res_buffer), "{\"status\":%d}\v\n", status);
		UART_puts( UART_ePORT_BLUETOOTH, res_buffer );
	}
	else if ( fullMessageReceived )
    {
		// NULL terminate the buffer
		bluetooth_data[bluetooth_count] = 0;

		// Acknowledge the fragment
		UART_puts( UART_ePORT_BLUETOOTH, "{\"status\":2}\v\n" );

        printf( "msg received:\n" );
        printf( bluetooth_data );
        printf( "\n" );
        
        printf("bluetooth buffer stopped at %d chars\n", bluetooth_count);
        bluetooth_count = 0;
        fragment_count = 0;
        
	    // TODO: add JSON parsing here

	    // TODO: return parse JSON data (need to change the return type, return NULL if no data to return yet)


        // TODO: remove this (only for testing purposes, should actually respond in the controller)
        HPS_usleep(3 * 1000 * 10000);
        UART_puts( UART_ePORT_BLUETOOTH, "{\"status\":1}\v\n" );
        /////////////////////
    }

}


void BLUETOOTH_Process( void )
{
    char buffer[120];
    static int count = 0;

    sprintf( buffer, "DE1 msg to frontend, count: %i\n", count );
    count++;
    
    UART_puts( UART_ePORT_BLUETOOTH, buffer );

}

