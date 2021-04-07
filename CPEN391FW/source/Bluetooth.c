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

#ifndef JSON_PARSER_H
#define JSON_PARSER_H
#include "JsonParser.h"
#endif

#define BUFFER_SIZE 1024
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

/*
 * Sends a message to the phone over bluetooth. Assumes that the string has already been
 * properly formatted with special characters like quotations backslashed.
 *
 * The main purpose of this function is to fragment the messages and send them in the
 * expected format.
 */
void bluetooth_send_message( char* data )
{
	// TODO: need to still implement (and convert bluetooth_process() to use it)
	(void) data;
}

/*
 * Returns a parsed json token after all data has been processed. NULL otherwise.
 */
char* bluetooth_process( char ch )
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
        
		// Reset state
        bluetooth_count = 0;
        fragment_count = 0;
        
        printf( "msg received:\n" );
		printf( bluetooth_data );
		printf( "\n" );
		printf("bluetooth buffer stopped at %d chars\n", bluetooth_count);

        return bluetooth_data;
    }

	return NULL;
}

/*
 * Waits for an entire bluetooth message to be received and processed
 */
char* bluetooth_wait_for_data()
{
	while (1)
	{
		if ( UART_TestForReceivedData( UART_ePORT_BLUETOOTH ) )
		{
			char ch = (char)UART_getchar( UART_ePORT_BLUETOOTH );
			char* fullJsonString = bluetooth_process( ch );

			if ( fullJsonString != NULL )
			{
				return fullJsonString;
			}
		}
	}
}
