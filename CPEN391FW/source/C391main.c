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
#include <stdlib.h>
#include <time.h>

/* Application common headers */
#include "TypeDef.h"
#include "memAddress.h"

/* Application module headers */
#include "HPS.h"
#include "UART.h"
#include "WIFI.h"
#include "Bluetooth.h"
#include "JsonParser.h"
#include "verificationService.h"
#include "hexService.h"

/*------------------- Local Function Prototype -------------------*/
static void init( void );
static void controller( void );


/*------------------- FUNCTIONS DELCARATIONS TO BE IMPLEMENTED IN OTHER FILES (TODO: REMOVE ONCE COMPLETED) -------------------*/

// char* upload_data(char* email, char* fileId, char* packetNumber, char* totalPackets, char* fileData);
// void download_data(char* localEncrpytionComponent, char* email, char* fileId);
// char* get_wifi_networks( void );
// int set_wifi_config(char* networkName, char* networkPassword);
// int set_device_password(char* password);



/*-------------------------------------------------------------------------------------------------*/

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
    time_t t;
    // Set seed as current time, used for random display HEX code
    srand((unsigned)time(&t));

    // Controller main loop
	while (1)
	{
		// Wait for a request message to be received
		char* jsonString = bluetooth_wait_for_data();
		jsmntok_t* jsonTokens = str_to_json(jsonString);

		// Check for JSON parsing errors
		if ( jsonTokens == NULL )
		{
			// Send error response and abort token processing
			bluetooth_send_status(0);
			continue;
		}

		// Get the first value to determine messageType
		char** messageTypeValues = get_json_values(jsonString, jsonTokens, 1);
		long messageType = strtol(messageTypeValues[0], NULL, 10);
		free_json_values_array(messageTypeValues, 1);

		// Direct the request to the appropriate handler function (pure functions that take inputs, not the JSON tokens)
		int status = -1;
		char* responseData = NULL;

		char** allValues;
		int expectedNumValues = 0;

		switch (messageType)
		{
			case 1:
			{
                generateDisplayHexCode();
				status = 1;
				break;
			}
			case 2:
			{
				expectedNumValues = 3;
				allValues = get_json_values(jsonString, jsonTokens, expectedNumValues);

                status = verify(allValues[1], allValues[2]);
				break;
			}
			case 3:
			{
				expectedNumValues = 6;
				allValues = get_json_values(jsonString, jsonTokens, expectedNumValues);

				// TODO: responseData = upload_data(allValues[1], allValues[2], allValues[3], allValues[4], allValues[5]);
				responseData = "{\"status\":1,\"localEncryptionComponent\":\"test\"}"; // TODO: remove, placeholder until above function is implemented
				break;
			}
			case 4:
			{
				expectedNumValues = 4;
				allValues = get_json_values(jsonString, jsonTokens, expectedNumValues);

				// TODO: download_data(allValues[1], allValues[2], allValues[3]);
				break;
			}
			case 5:
			{
				// TODO: responseData = get_wifi_networks();
				responseData = "{\"status\":1,\"networks\":\"[network1,network2]\"}"; // TODO: remove, placeholder until above function is implemented
				break;
			}
			case 6:
			{
				expectedNumValues = 3;
				allValues = get_json_values(jsonString, jsonTokens, expectedNumValues);

				// TODO: status = set_wifi_config(allValues[1], allValues[2]);
				status = 1; // TODO: remove, placeholder until above function is implemented
				break;
			}
			case 7:
			{
				expectedNumValues = 2;
				allValues = get_json_values(jsonString, jsonTokens, expectedNumValues);

                setPassword(allValues[1]);
                status = 1;
				break;
			}
			default:
			{
				// Send error status if messageType is invalid
				status = 0;
			}
		}

		// Free values (if they were collected)
		if ( expectedNumValues > 0 )
		{
			free_json_values_array(allValues, expectedNumValues);
		}

		// Pause (to prevent response message from being sent too quickly)
		HPS_usleep(1 * 1000 * 1000); // ~0.5 second

		// Send the response message (if applicable)
		if ( status != -1 )
		{
			// Send basic status response
			bluetooth_send_status(status);
		}
		else if ( responseData != NULL )
		{
			// Send full char message
			bluetooth_send_message(responseData);
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
    init();
    controller();
	return 0;
}




