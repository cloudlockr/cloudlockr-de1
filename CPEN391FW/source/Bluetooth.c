/*
 * Bluetooth.c
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description:
 * This module implements bluetooth related functionalities.
 *
 * Author:
 * zfrantzen
 */

#include <stdio.h>
#include <string.h>

#include "TypeDef.h"
#include "HPS.h"
#include "UART.h"
#include "JsonParser.h"

#define BUFFER_SIZE 500500 // 1 mb of data (assuming 2 bytes per char) + 500 bytes of extra data allowance
#define TIMEOUT_ITER 10000000 // Large number to represent the max number of iterations with no data arriving (~50 sec)
static int  bluetooth_count = 0;
static char bluetooth_data[BUFFER_SIZE];

/*
 * Sends a JSON message to the phone over bluetooth. Assumes that the string has already been
 * properly formatted as a valid JSON object and has special characters like quotations backslashed.
 * Must have "\v\n" at the end of the data string.
 */
void bluetooth_send_message( char* data )
{
	UART_puts( UART_ePORT_BLUETOOTH, data );
}

/*
 * Special communication method for sending a specific status code
 * (most communications only require this)
 */
void bluetooth_send_status( int status )
{
	// Format message and send
	char res_buffer[18];
	snprintf(res_buffer, sizeof(res_buffer), "{\"status\":%d}\v\n", status);
	UART_puts( UART_ePORT_BLUETOOTH, res_buffer );
}

/*
 * Waits for an entire bluetooth message to be received and processed
 */
char* bluetooth_wait_for_data( void )
{
	// Reset state
	bluetooth_count = 0;
	int i = 0;
	char c = ' ';
	
	// Wait for initial data to arrive
	while (1)
	{
		if ( UART_TestForReceivedData( UART_ePORT_BLUETOOTH ) )
		{
			c = (char)UART_getchar( UART_ePORT_BLUETOOTH );
			bluetooth_data[bluetooth_count] = c;
			bluetooth_count++;
			break;
		}
	}

	// Process all subsequent data (timing out if too much waiting elapses)
	while (i < TIMEOUT_ITER && c != '\n')
	{
		if ( UART_TestForReceivedData( UART_ePORT_BLUETOOTH ) )
		{
			c = (char)UART_getchar( UART_ePORT_BLUETOOTH );
			bluetooth_data[bluetooth_count] = c;

			bluetooth_count++;
			i = 0;

			continue;
		}

		i++;
	}
	
	// Return NULL if timeout occurs
	if (c != '\n')
	{
		return NULL;
	}

	// Otherwise, acknowledge the fragment and null terminate (and truncate line endings)
	bluetooth_send_status(2);
	bluetooth_data[bluetooth_count - 2] = 0;

	return bluetooth_data;
}
