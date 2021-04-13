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
#include <typeDef.h>

#include "constants.h"
#include "hpsService.h"
#include "UART.h"
#include "jsonParser.h"

static int bluetooth_count = 0;
static char bluetooth_data[BUFFER_SIZE];

// Mocking request index
int mock_idx = 0;

char *mock_bluetooth_wait_for_data(void)
{
	char *json_str = NULL;
	if (mock_idx == 0)
	{
		json_str = "{\"type\":7,\"password\":\"1234567890abc\"}";
		mock_idx++;
	}
	else if (mock_idx == 1)
	{
		json_str = "{\"type\":6,\"networkName\":\"networkName\",\"networkPassword\":\"networkPassword\"}";
		mock_idx++;
	}
	else if (mock_idx == 2)
	{
		json_str = "{\"type\":1}";
		mock_idx++;
	}
	else if (mock_idx == 3)
	{
		json_str = "{\"type\":2,\"password\":\"1234567890abc\",\"hex\":\"ABCDEF\"}";
		mock_idx++;
	}
	else if (mock_idx == 4)
	{
		json_str = "{\"type\":3,\"fileId\":\"d869c9d6-1227-40ca-a3e8-bc11db68a1ab\",\"packetNumber\":1,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"1234567890abcdeffedcba0987654321\"}";
		mock_idx++;
	}
	else if (mock_idx == 5)
	{
		json_str = "{\"type\":3,\"fileId\":\"d869c9d6-1227-40ca-a3e8-bc11db68a1ab\",\"packetNumber\":2,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"abcdef123456789001010101\"}";
		mock_idx++;
	}
	else if (mock_idx == 6)
	{
		json_str = "{\"type\":3,\"fileId\":\"d869c9d6-1227-40ca-a3e8-bc11db68a1ab\",\"packetNumber\":3,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"1234567890abcdef\"}";
		mock_idx++;
	}
	else if (mock_idx == 7)
	{
		json_str = "{\"type\":4,\"localEncryptionComponent\":\"0102ABCD\",\"fileId\":\"d869c9d6-1227-40ca-a3e8-bc11db68a1ab\",\"location\":\"37.422|-122.084|5.285\"}";
		mock_idx++;
	}
	else if (mock_idx == 8)
	{
		json_str = "{\"status\":1}";
		mock_idx++;
	}
	else if (mock_idx == 9)
	{
		json_str = "{\"status\":1}";
		mock_idx++;
	}
	else if (mock_idx == 10)
	{
		json_str = "{\"type\":3,\"fileId\":\"783cf156-aa19-4110-8484-732f1b0a1068\",\"packetNumber\":1,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"testing hello wowowow\"}";
		mock_idx++;
	}
	else if (mock_idx == 11)
	{
		json_str = "{\"type\":3,\"fileId\":\"783cf156-aa19-4110-8484-732f1b0a1068\",\"packetNumber\":2,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"how you doin' =)\"}";
		mock_idx++;
	}
	else if (mock_idx == 12)
	{
		json_str = "{\"type\":3,\"fileId\":\"783cf156-aa19-4110-8484-732f1b0a1068\",\"packetNumber\":3,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"120eujef98erfp949w8fyw\"}";
		mock_idx++;
	}
	else if (mock_idx == 13)
	{
		json_str = "{\"type\":4,\"localEncryptionComponent\":\"0102ABCD\",\"fileId\":\"783cf156-aa19-4110-8484-732f1b0a1068\",\"location\":\"37.422|-122.084|5.285\"}";
		mock_idx++;
	}
	else if (mock_idx == 14)
	{
		json_str = "{\"status\":1}";
		mock_idx++;
	}
	else if (mock_idx == 15)
	{
		json_str = "{\"status\":1}";
		mock_idx++;
	}
	else
	{
		printf(">>>>>>>>>    CloudLockr Firmware end    <<<<<<<<<\n");
		exit(0);
	}

	return json_str;
}

/*
 * Sends a JSON message to the phone over bluetooth. Assumes that the string has already been
 * properly formatted as a valid JSON object and has special characters like quotations backslashed.
 * Must have "\v\n" at the end of the data string.
 */
void bluetooth_send_message(char *data)
{
#if !MOCK_BLUETOOTH
	UART_puts(UART_ePORT_BLUETOOTH, data);
#endif
}

/*
 * Special communication method for sending a specific status code
 * (most communications only require this)
 */
void bluetooth_send_status(int status)
{
#if !MOCK_BLUETOOTH
	// Format message and send
	char res_buffer[18];
	snprintf(res_buffer, sizeof(res_buffer), "{\"status\":%d}\v\n", status);
	UART_puts(UART_ePORT_BLUETOOTH, res_buffer);
#endif
}

/*
 * Waits for an entire bluetooth message to be received and processed
 */
char *bluetooth_wait_for_data(void)
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
		if (UART_TestForReceivedData(UART_ePORT_BLUETOOTH))
		{
			c = (char)UART_getchar(UART_ePORT_BLUETOOTH);
			bluetooth_data[bluetooth_count] = c;
			bluetooth_count++;
			break;
		}
	}

	// Process all subsequent data (timing out if too much waiting elapses)
	while (i < TIMEOUT_ITER && c != '\n')
	{
		if (UART_TestForReceivedData(UART_ePORT_BLUETOOTH))
		{
			c = (char)UART_getchar(UART_ePORT_BLUETOOTH);
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
