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
#include "hpsService.h"
#include "UART.h"
#include "JsonParser.h"

#define BUFFER_SIZE 500500 // 1 mb of data (assuming 2 bytes per char) + 500 bytes of extra data allowance
#define TIMEOUT_ITER 10000000 // Large number to represent the max number of iterations with no data arriving (~50 sec)
static int  bluetooth_count = 0;
static char bluetooth_data[BUFFER_SIZE];

/* Mocking settings */
#define MOCK_BLUETOOTH 0
static int mock_idx = 0;

/*
 * Sends a JSON message to the phone over bluetooth. Assumes that the string has already been
 * properly formatted as a valid JSON object and has special characters like quotations backslashed.
 * Must have "\v\n" at the end of the data string.
 */
void bluetooth_send_message( char* data )
{
#if !MOCK_BLUETOOTH
	UART_puts( UART_ePORT_BLUETOOTH, data );
#endif
}

/*
 * Special communication method for sending a specific status code
 * (most communications only require this)
 */
void bluetooth_send_status( int status )
{
#if !MOCK_BLUETOOTH
	// Format message and send
	char res_buffer[18];
	snprintf(res_buffer, sizeof(res_buffer), "{\"status\":%d}\v\n", status);
	UART_puts( UART_ePORT_BLUETOOTH, res_buffer );
#endif
}

/*
 * Waits for an entire bluetooth message to be received and processed
 */
char* bluetooth_wait_for_data( void )
{
#if MOCK_BLUETOOTH
	return mock_bluetooth_wait_for_data();
#else

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
#endif
}

char* mock_bluetooth_wait_for_data( void )
{
	char *json_str = NULL;
	if (mock_idx == 0)
	{
		json_str = "{\"type\":7,\"password\":\"1234567890abc\"}";
		mock_idx++;
	}
	else if (mock_idx == 1)
	{
		json_str = "{\"type\":1}";
		mock_idx++;
	}
	else if (mock_idx == 2)
	{
		json_str = "{\"type\":2,\"password\":\"1234567890abc\",\"hex\":\"ABCDEF\"}";
		mock_idx++;
	}
	else if (mock_idx == 3)
	{
		json_str = "{\"type\":3,\"fileId\":\"123\",\"packetNumber\":1,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"1234567890abcdeffedcba0987654321\"}";
		mock_idx++;
	}
	else if (mock_idx == 4)
	{
		json_str = "{\"type\":4,\"localEncryptionComponent\":\"0102ABCD\",\"fileId\":\"123\",\"location\":\"37.422|-122.084|5.285\"}";
		mock_idx++;
	}
	else
	{
		mock_idx = 0;
	}

	return json_str;
}
