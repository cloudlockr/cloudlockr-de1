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
#include "memAddress.h"

/* Application module headers */
#include "HPS.h"
#include "UART.h"
#include "WIFI.h"
#include "Bluetooth.h"
#include "JsonParser.h"
#include "MPU9250.h"

/*------------------- Constants Define ---------------------------------------*/

#define TIME_FLAG_1MS   0x0001
#define TIME_FLAG_2MS   0x0002
#define TIME_FLAG_3MS   0x0004
#define TIME_FLAG_4MS   0x0008
#define TIME_FLAG_5MS   0x0010
#define TIME_FLAG_10MS  0x0020
#define TIME_FLAG_20MS  0x0040
#define TIME_FLAG_50MS  0x0080
#define TIME_FLAG_100MS 0x0100
#define TIME_FLAG_200MS 0x0210
#define TIME_FLAG_300MS 0x0410
#define TIME_FLAG_400MS 0x0810
#define TIME_FLAG_500MS 0x1000
#define TIME_FLAG_1SEC  0x2000

/*------------------- Local Variables ----------------------------------------*/
static uint32 main_u32TimeFlags = 0;
static uint32 main_freeCount = 0;



/*------------------- Local Function Prototype -------------------*/
static void init( void );
static void controller( void );


/*------------------- FUNCTIONS DELCARATIONS TO BE IMPLEMENTED IN OTHER FILES (TODO: REMOVE ONCE COMPLETED) -------------------*/

// int generate_display_hex_code( void );
// int verify(char* devicePassword, char* hexCode);
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
	HPS_Init();

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
    
    if ( MPU9250_Begin() )
    {
        printf( "MPU9250_Begin: success\n" );
    }
    else
    {
        printf( "MPU9250_Begin: fail\n" );
    }
    
    if ( MPU9250_EnableDrdyInt() )
    {
        printf( "EnableDrdyInt: success\n" );
    }
    else
    {
        printf( "EnableDrdyInt: fail\n" );
    }

}

void main_UpdateTimeFlag( void )
{
    static uint32 Count_ms  = 0;
    
    if ( HPS_ElapsedUs( main_freeCount, 1000 ) )
    {
	    main_freeCount = *PtimerCount;
        Count_ms++;
        main_u32TimeFlags |= TIME_FLAG_1MS;
    
        // Update flags    
        if ( ( Count_ms % 2 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_2MS;
        }
    
        if ( ( Count_ms % 3 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_3MS;
        }
    
        if ( ( Count_ms % 4 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_4MS;
        }
    
        if ( ( Count_ms % 5 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_5MS;
        }
    
        if ( ( Count_ms % 10 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_10MS;
        }
    
        if ( ( Count_ms % 20 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_20MS;
        }
    
        if ( ( Count_ms % 50 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_50MS;
        }
    
        if ( ( Count_ms % 100 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_100MS;
        }
    
        if ( ( Count_ms % 200 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_200MS;
        }
    
        if ( ( Count_ms % 300 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_300MS;
        }
    
        if ( ( Count_ms % 400 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_400MS;
        }
    
    
        if ( ( Count_ms % 500 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_500MS;
        }
    
        if ( ( Count_ms % 1000 ) == 0 )
        {   
            main_u32TimeFlags |= TIME_FLAG_1SEC;
        }
    }
}

static void controller( void )
{
	while (1)
	{
		// Wait for a request message to be received
		char* jsonString = bluetooth_wait_for_data();
		jsmntok_t* jsonTokens = str_to_json(jsonString);
		static int flag = 0;
    
		main_UpdateTimeFlag();
    
		// Blink user green LED at 2Hz.
		if ( main_u32TimeFlags & TIME_FLAG_500MS )
		{
			HPS_ToggleLedG();
		}

		// For sensor testing only - remove after verification.
#if 0
		//-----------------------------
		if ( main_u32TimeFlags & TIME_FLAG_1SEC )
		{
			flag++;
			if ( flag >= 2 )
			{
				getSensorKey();
				flag = 0;
			}
		}
		//=====================================
#endif
    
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
				//TODO: status = generate_display_hex_code();
				status = 1; // TODO: remove, placeholder until above function is implemented
				break;
			}
			case 2:
			{
				expectedNumValues = 3;
				allValues = get_json_values(jsonString, jsonTokens, expectedNumValues);

				//TODO: status = verify(allValues[1], allValues[2]);
				status = 1; // TODO: remove, placeholder until above function is implemented
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

				// TODO: status = set_device_password(allValues[1]);
				status = 1; // TODO: remove, placeholder until above function is implemented
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
    
    // Clear time flags  at the end of each iteration.
    main_u32TimeFlags = 0;
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




